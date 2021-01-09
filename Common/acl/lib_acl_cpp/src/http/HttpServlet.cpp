#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/session/memcache_session.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/HttpSession.hpp"
#include "acl_cpp/http/HttpServletRequest.hpp"
#include "acl_cpp/http/HttpServletResponse.hpp"
#include "acl_cpp/http/HttpServlet.hpp"
#endif

#ifndef ACL_CLIENT_ONLY

namespace acl
{

HttpServlet::HttpServlet(socket_stream* stream, session* session)
: req_(NULL)
, res_(NULL)
, stream_(stream)
{
	init();

	if (session == NULL) {
		session_ = NEW memcache_session("127.0.0.1|11211");
		session_ptr_ = session_;
	} else {
		session_ = session;
		session_ptr_ = NULL;
	}
}

HttpServlet::HttpServlet(socket_stream* stream,
	const char* memcache_addr /* = "127.0.0.1:11211" */)
: req_(NULL)
, res_(NULL)
, stream_(stream)
{
	init();

	session_     = NEW memcache_session(memcache_addr);
	session_ptr_ = session_;
}

HttpServlet::HttpServlet()
{
	init();

	req_         = NULL;
	res_         = NULL;
	stream_      = NULL;
	session_     = NULL;
	session_ptr_ = NULL;
}

void HttpServlet::init()
{
	first_             = true;
	local_charset_[0]  = 0;
	rw_timeout_        = 60;
	parse_body_limit_  = 0;
}

HttpServlet::~HttpServlet(void)
{
	delete req_;
	delete res_;
	delete session_ptr_;
}

#define COPY(x, y) ACL_SAFE_STRNCPY((x), (y), sizeof((x)))

HttpServlet& HttpServlet::setLocalCharset(const char* charset)
{
	if (charset && *charset) {
		COPY(local_charset_, charset);
	} else {
		local_charset_[0] =0;
	}
	return *this;
}

HttpServlet& HttpServlet::setRwTimeout(int rw_timeout)
{
	rw_timeout_ = rw_timeout;
	return *this;
}

HttpServlet& HttpServlet::setParseBody(bool)
{
	return *this;
}

HttpServlet& HttpServlet::setParseBodyLimit(int length)
{
	if (length > 0) {
		parse_body_limit_ = length;
	}
	return *this;
}

static bool upgradeWebsocket(HttpServletRequest& req, HttpServletResponse& res)
{
	const char* ptr = req.getHeader("Connection");
	if (ptr == NULL) {
		return false;
	}
	if (acl_strcasestr(ptr, "Upgrade") == NULL) {
		return false;
	}
	ptr = req.getHeader("Upgrade");
	if (ptr == NULL) {
		return false;
	}
	if (strcasecmp(ptr, "websocket") != 0) {
		return false;
	}
	const char* key = req.getHeader("Sec-WebSocket-Key");
	if (key == NULL || *key == 0) {
		logger_warn("no Sec-WebSocket-Key");
		return false;
	}

	http_header& header = res.getHttpHeader();
	header.set_upgrade("websocket");
	header.set_ws_accept(key);
	return true;
}

bool HttpServlet::start(void)
{
	socket_stream* in;
	socket_stream* out;
	bool cgi_mode;

	bool first = first_;
	if (first_) {
		first_ = false;
	}

	if (stream_ == NULL) {
		// ������Ϊ�գ��� CGI ģʽ��������׼�������
		// ��Ϊ������
		in = NEW socket_stream();
		in->open(ACL_VSTREAM_IN);

		out = NEW socket_stream();
		out->open(ACL_VSTREAM_OUT);
		cgi_mode = true;
	} else {
		in = out = stream_;
		cgi_mode = false;
	}

	// �� HTTP �������ظ���������£��Է���һ����Ҫ����ɾ������/��Ӧ����
	delete req_;
	delete res_;

	res_ = NEW HttpServletResponse(*out);
	req_ = NEW HttpServletRequest(*res_, *session_, *in, local_charset_,
			parse_body_limit_);

	// ���� HttpServletRequest ����
	res_->setHttpServletRequest(req_);

	if (rw_timeout_ >= 0) {
		req_->setRwTimeout(rw_timeout_);
	}

	res_->setCgiMode(cgi_mode);

	string method_s(32);
	http_method_t method = req_->getMethod(&method_s);

	// ���������ֵ�Զ��趨�Ƿ���Ҫ���ֳ�����
	if (!cgi_mode) {
		res_->setKeepAlive(req_->isKeepAlive());
	}

	bool  ret;

	switch (method) {
	case HTTP_METHOD_GET:
		if (upgradeWebsocket(*req_, *res_)) {
			if (res_->sendHeader() == false) {
				logger_error("sendHeader error!");
				return false;
			}
			ret = doWebSocket(*req_, *res_);
		} else {
			ret = doGet(*req_, *res_);
		}
		break;
	case HTTP_METHOD_POST:
		ret = doPost(*req_, *res_);
		break;
	case HTTP_METHOD_PUT:
		ret = doPut(*req_, *res_);
		break;
	case HTTP_METHOD_PATCH:
		ret = doPatch(*req_, *res_);
		break;
	case HTTP_METHOD_CONNECT:
		ret = doConnect(*req_, *res_);
		break;
	case HTTP_METHOD_PURGE:
		ret = doPurge(*req_, *res_);
		break;
	case HTTP_METHOD_DELETE:
		ret = doDelete(*req_, *res_);
		break;
	case HTTP_METHOD_HEAD:
		ret = doHead(*req_, *res_);
		break;
	case HTTP_METHOD_OPTION:
		ret = doOptions(*req_, *res_);
		break;
	case HTTP_METHOD_PROPFIND:
		ret = doPropfind(*req_, *res_);
		break;
	case HTTP_METHOD_OTHER:
		ret = doOther(*req_, *res_, method_s.c_str());
		break;
	default:
		ret = false; // �п�����IOʧ�ܻ�δ֪����
		if (req_->getLastError() == HTTP_REQ_ERR_METHOD) {
			doUnknown(*req_, *res_);
		} else if (first) {
			doError(*req_, *res_);
		}
		break;
	}

	if (in != out) {
		// ����Ǳ�׼���������������Ҫ�Ƚ����������׼����������
		// Ȼ������ͷ������������������ڲ����Զ��ж�������Ϸ���
		// �������Ա�֤��ͻ��˱��ֳ�����
		in->unbind();
		out->unbind();
		delete in;
		delete out;
	}

	return ret;
}

bool HttpServlet::doRun(void)
{
	bool ret = start();
	if (req_ == NULL || res_ == NULL) {
		return ret;
	}
	if (!ret) {
		return false;
	}

	// ���ظ��ϲ�����ߣ�true ��ʾ�������ֳ����ӣ������ʾ��Ͽ�����
	return req_->isKeepAlive()
		&& res_->getHttpHeader().get_keep_alive();
}

bool HttpServlet::doRun(session& session, socket_stream* stream /* = NULL */)
{
	stream_  = stream;
	session_ = &session;
	return doRun();
}

bool HttpServlet::doRun(const char* memcached_addr, socket_stream* stream)
{
	memcache_session session(memcached_addr);
	return doRun(session, stream);
}

bool HttpServlet::doGet(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doGet yet!");
	return false;
}

bool HttpServlet::doWebSocket(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doWebSocket yet!");
	return false;
}

bool HttpServlet::doPost(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doPost yet!");
	return false;
}

bool HttpServlet::doPut(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doPut yet!");
	return false;
}

bool HttpServlet::doPatch(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doPatch yet!");
	return false;
}

bool HttpServlet::doConnect(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doConnect yet!");
	return false;
}

bool HttpServlet::doPurge(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doPurge yet!");
	return false;
}

bool HttpServlet::doDelete(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doDelete yet!");
	return false;
}

bool HttpServlet::doHead(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doHead yet!");
	return false;
}

bool HttpServlet::doOptions(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doOptions yet!");
	return false;
}

bool HttpServlet::doPropfind(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doPropfind yet!");
	return false;
}

bool HttpServlet::doOther(HttpServletRequest&, HttpServletResponse&,
	const char* method)
{
	(void) method;
	logger_error("child not implement doOther yet!");
	return false;
}

bool HttpServlet::doUnknown(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doUnknown yet!");
	return false;
}

bool HttpServlet::doError(HttpServletRequest&, HttpServletResponse&)
{
	logger_error("child not implement doError yet!");
	return false;
}

} // namespace acl

#endif // ACL_CLIENT_ONLY
