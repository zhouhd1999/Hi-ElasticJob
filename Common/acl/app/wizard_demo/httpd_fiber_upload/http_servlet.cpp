#include "stdafx.h"
#include "http_servlet.h"

static acl::atomic_long __counter = 0;

http_servlet::http_servlet(acl::socket_stream* stream, acl::session* session)
: acl::HttpServlet(stream, session)
, fsize1_(-1)
, fsize2_(-1)
, fsize3_(-1)
{
	handlers_["/upload"] = &http_servlet::onUpload;
}

http_servlet::~http_servlet(void)
{
}

bool http_servlet::doError(request_t&, response_t& res)
{
	res.setStatus(400);
	res.setContentType("text/xml; charset=utf-8");

	// ���� http ��Ӧ��
	acl::string buf;
	buf.format("<root error='some error happened!' />\r\n");
	res.write(buf);
	res.write(NULL, 0);
	return false;
}

bool http_servlet::doOther(request_t&, response_t& res, const char* method)
{
	res.setStatus(400);
	res.setContentType("text/xml; charset=utf-8");
	// ���� http ��Ӧ��
	acl::string buf;
	buf.format("<root error='unkown request method %s' />\r\n", method);
	res.write(buf);
	res.write(NULL, 0);
	return false;
}

bool http_servlet::doGet(request_t& req, response_t& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(request_t& req, response_t& res)
{
	const char* ptr = req.getPathInfo();
	if (ptr == NULL || *ptr == 0) {
		logger_error("path null");
		return doError(req, res);
	}

	acl::string path(ptr);
	path.lower();

	// ���� uri path ���Ҷ�Ӧ�Ĵ��������Ӷ�ʵ�� HTTP ·�ɹ���

	std::map<std::string, handler_t>::iterator it =
		handlers_.find(path.c_str());

	if (it == handlers_.end()) {
		return onPage(req, res);
	}

	return (this->*it->second)(req, res);
}

// ȱʡ HTTP ���󣬽� upload.html ҳ�淵�ظ� HTTP �ͻ���
bool http_servlet::onPage(request_t& req, response_t& res)
{
	res.setContentType("text/html; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(req.isKeepAlive())	// �����Ƿ񱣳ֳ�����
		.setContentEncoding(true)		// �Զ�֧��ѹ������
		.setChunkedTransferEncoding(true);	// ���� chunk ���䷽ʽ

	const char* page_html = "upload.html";
	acl::string buf;

	if (!acl::ifstream::load(page_html, &buf)) {
		buf.format("load %s error %s", page_html, acl::last_serror());
	}

	res.setContentLength(buf.size());
	return res.write(buf) && res.write(NULL, 0);
}

bool http_servlet::onUpload(request_t& req, response_t& res)
{
	res.setContentType("text/xml; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(req.isKeepAlive())	// �����Ƿ񱣳ֳ�����
		.setContentEncoding(true)		// �Զ�֧��ѹ������
		.setChunkedTransferEncoding(true);	// ���� chunk ���䷽ʽ

	// ��� HTTP ������������ͣ������Ĳ������ͣ��� name&value ��ʽ
	// ���� MIME �������ͣ���������������
	acl::http_request_t request_type = req.getRequestType();
	if (request_type != acl::HTTP_REQUEST_MULTIPART_FORM) {
		logger_warn("should be acl::HTTP_REQUEST_MULTIPART_FORM");
		return onPage(req, res);
	}

	// �Ȼ�� Content-Type ��Ӧ�� http_ctype ����
	acl::http_mime* mime = req.getHttpMime();
	if (mime == NULL) {
		logger_error("http_mime null");
		return onPage(req, res);
	}

	// ���������ĳ���
	long long content_length = req.getContentLength();
	if (content_length <= 0) {
		logger_error("body empty");
		return onPage(req, res);
	}

	acl::string path;
	long long n = ++__counter;
	long long i = __counter.value();
	printf("i=%lld, n=%lld\r\n", i, n);
#if defined(_WIN32) || defined(_WIN64)
	path.format("%s\\mime_file.%u.%lld",
		var_cfg_var_path, (unsigned) _getpid(), n);
#else
	path.format("%s/mime_file.%u.%lld",
		var_cfg_var_path, (unsigned) getpid(), n);
#endif

	acl::meter_time(__FUNCTION__, __LINE__, "begin");

	acl::ofstream fp;
	if (!fp.open_write(path)) {
		logger_error("open %s error %s",
			path.c_str(), acl::last_serror());
		return doReply(req, res, "open file error");
	}

	// ����ԭʼ�ļ�����·��
	mime->set_saved_path(path);

	if (!upload(req, res, content_length, fp, *mime)) {
		reset();
		return false;
	}
	if (!parse(req, res, *mime)) {
		reset();
		return false;
	}

	reset();
	return true;
}

bool http_servlet::upload(request_t& req, response_t& res,
	long long content_length, acl::ofstream& fp, acl::http_mime& mime)
{
	// ���������
	acl::istream& in = req.getInputStream();
	bool finish = false;
	char buf[8192];

	//logger(">>>>>>>>>>read: %lld, total: %lld<<<<<",
	//	read_length_, content_length_);

	long long read_length = 0;

	// ��ȡ HTTP �ͻ�����������
	while (content_length > read_length) {
		size_t size = (size_t) (content_length - read_length);
		if (size > sizeof(buf)) {
			size = sizeof(buf);
		}
		int ret = in.read(buf, size);
		if (ret <= 0) {
			break;
		}

		//printf(">>>size: %d\r\n", ret);
		if (fp.write(buf, ret) == -1) {
			logger_error("write error %s", acl::last_serror());
			(void) doReply(req, res, "write error");
			return false;
		}

		read_length += ret;

		// �����õ����������������������н���
		// ����ٶ����������ݣ�����ֱ�Ӷ����������ٷ��� mime ��������
		if (!finish && mime.update(buf, (size_t) ret)) {
			finish = true;
		}
	}

	if (in.eof()) {
		logger_error("read error from http client");
		return false;
	}

	return true;
}

bool http_servlet::parse(request_t& req, response_t& res, acl::http_mime& mime)
{
	const char* ptr = req.getParameter("name1");
	if (ptr) {
		param1_ = ptr;
	}
	ptr = req.getParameter("name2");
	if (ptr) {
		param2_ = ptr;
	}
	ptr = req.getParameter("name3");
	if (ptr) {
		param3_ = ptr;
	}

	acl::string path;

	// �������е� MIME ��㣬�ҳ�����Ϊ�ļ����Ĳ��ֽ���ת��
	const std::list<acl::http_mime_node*>& nodes = mime.get_nodes();
	std::list<acl::http_mime_node*>::const_iterator cit = nodes.begin();
	for (; cit != nodes.end(); ++cit) {
		const char* name = (*cit)->get_name();
		if (name == NULL) {
			continue;
		}

		acl::http_mime_t mime_type = (*cit)->get_mime_type();
		if (mime_type == acl::HTTP_MIME_FILE) {
			const char* filename = (*cit)->get_filename();
			if (filename == NULL) {
				logger("filename null");
				continue;
			}

			// �е����������IE���ϴ��ļ�ʱ������ļ�·��������
			// ��Ҫ�Ƚ�·��ȥ��
			filename = acl_safe_basename(filename);
#if defined(_WIN32) || defined(_WIN64)
			path.format("%s\\%s", var_cfg_var_path, filename);
#else
			path.format("%s/%s", var_cfg_var_path, filename);
#endif
			(void) (*cit)->save(path.c_str());

			if (strcmp(name, "file1") == 0) {
				file1_ = filename;
				fsize1_ = get_fsize(var_cfg_var_path, filename);
			} else if (strcmp(name, "file2") == 0) {
				file2_ = filename;
				fsize2_ = get_fsize(var_cfg_var_path, filename);
			} else if (strcmp(name, "file3") == 0) {
				file3_ = filename;
				fsize3_ = get_fsize(var_cfg_var_path, filename);
			}
		}
	}

	// �������ص�ĳ���ļ���ת��
	const acl::http_mime_node* node = mime.get_node("file1");
	if (node && node->get_mime_type() == acl::HTTP_MIME_FILE) {
		ptr = node->get_filename();
		if (ptr) {
			// �е����������IE���ϴ��ļ�ʱ������ļ�·��������
			// ��Ҫ�Ƚ�·��ȥ��
			ptr = acl_safe_basename(ptr);
#if defined(_WIN32) || defined(_WIN64)
			path.format("%s\\1_%s", var_cfg_var_path, ptr);
#else
			path.format("%s/1_%s", var_cfg_var_path, ptr);
#endif
			(void) node->save(path.c_str());
		}
	}

	return doReply(req, res, "OK");
}

bool http_servlet::doReply(request_t& req, response_t& res, const char* info)
{
	// ���� xml ��ʽ��������
	acl::xml1 body;

	body.get_root().add_child("root", true)
		.add_child("content_type", true)
			.add_attr("type", (int) req.getRequestType())
			.get_parent()
		.add_child("info", true)
			.set_text(info)
			.get_parent()
		.add_child("params", true)
			.add_child("param", true)
				.add_attr("name1", param1_)
			.get_parent()
			.add_child("param", true)
				.add_attr("name2", param2_)
			.get_parent()
			.add_child("param", true)
				.add_attr("name3", param3_)
			.get_parent()
		.add_child("files", true)
			.add_child("file", true)
				.add_attr("filename", file1_)
				.add_attr("fsize", fsize1_)
				.get_parent()
			.add_child("file", true)
				.add_attr("filename", file2_)
				.add_attr("fsize", fsize2_)
				.get_parent()
			.add_child("file", true)
				.add_attr("filename", file3_)
				.add_attr("fsize", fsize3_);
	acl::string buf;
	body.build_xml(buf);

	logger(">>%s<<", buf.c_str());
	return res.write(buf) && res.write(NULL, 0);
}

long long http_servlet::get_fsize(const char* dir, const char* filename)
{
	acl::string path;
#if defined(_WIN32) || defined(_WIN64)
	path.format("%s\\%s", dir, filename);
#else
	path.format("%s/%s", dir, filename);
#endif
	acl::ifstream in;
	if (in.open_read(path) == false) {
		logger_error("open %s error %s", path.c_str(), acl::last_serror());
		return -1;
	}
	return in.fsize();
}

void http_servlet::reset(void)
{
	param1_.clear();
	param2_.clear();
	param3_.clear();
	file1_.clear();
	file2_.clear();
	file3_.clear();
	fsize1_ = -1;
	fsize2_ = -1;
	fsize3_ = -1;
}
