#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"
#include "http_header.hpp"

#ifndef ACL_CLIENT_ONLY

namespace acl {

class session;
class socket_stream;
class HttpServletRequest;
class HttpServletResponse;

/**
 * ���� HTTP �ͻ�������Ļ��࣬������Ҫ�̳и���
 */
class ACL_CPP_API HttpServlet : public noncopyable
{
public:
	/**
	 * ���캯��
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @param session {session*} ÿһ�� HttpServlet ����һ�� session ����
	 */
	HttpServlet(socket_stream* stream, session* session);

	/**
	 * ���캯��
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @param memcache_addr {const char*}
	 */
	HttpServlet(socket_stream* stream,
		const char* memcache_addr = "127.0.0.1|11211");

	HttpServlet(void);
	virtual ~HttpServlet(void) = 0;

	session& getSession(void) const
	{
		return *session_;
	}

	socket_stream* getStream(void) const
	{
		return stream_;
	}

	/**
	 * ���ñ����ַ�������������˱����ַ��������ڽ��� HTTP ��������ʱ����
	 * �Զ���������ַ���תΪ�����ַ������ú��������� doRun ֮ǰ���ò���Ч
	 * @param charset {const char*} �����ַ����������ָ��Ϊ�գ�
	 *  ����������ַ���
	 * @return {HttpServlet&}
	 */
	HttpServlet& setLocalCharset(const char* charset);

	/**
	 * ���� HTTP �Ự���� IO ��д��ʱʱ�䣻�ú��������� doRun ǰ���ò���Ч
	 * @param rw_timeout {int} ��д��ʱʱ��(��)
	 * @return {HttpServlet&}
	 */
	HttpServlet& setRwTimeout(int rw_timeout);

	/**
	 * ��� POST �������÷��������Ƿ���Ҫ�������������ݣ�Ĭ��Ϊ�������ú�
	 * �������� doRun ֮ǰ���ò���Ч����������Ϊ�������� MIME ��ʽ����ʹ
	 * ���ñ����������˽������ݣ�Ҳ�������������н���
	 * @param on {bool} �Ƿ���Ҫ����
	 * @return {HttpServlet&}
	 * xxxx: �÷����Ѿ���������
	 */
	HttpServlet& setParseBody(bool on);

	/**
	 * ��� POST �������÷������ý������������󳤶ȣ���������壬�ú���
	 * ������ doRun ֮ǰ���ò���Ч
	 * @param length {int} ��󳤶����ƣ��������������峤�ȹ�����ֱ��
	 *  ���� false�������ֵ <= 0 ���ڲ������������峤�ȣ����øú���ǰ
	 *  �ڲ�ȱʡֵΪ 0
	 * @return {HttpServlet&}
	 */
	HttpServlet& setParseBodyLimit(int length);
	
	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯����
	 * @return {bool} ���ش����������� false ��ʾ����ʧ�ܣ���Ӧ�ر����ӣ�
	 *  ���� true ��ʾ����ɹ������ô˺�����Ӧ�ü���ͨ���ж�����/��Ӧ������
	 *  �Ƿ���Ҫ���ֳ�������ȷʵ�����Ƿ񱣳ֳ�����
	 */
	bool start(void);

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯����
	 * �ú������Ȼ���� start ���̣�Ȼ����� start �ķ��ؽ��������/��Ӧ
	 * �����Ƿ�Ҫ�󱣳ֳ������������Ƿ���Ҫ��ͻ��˱��ֳ�����
	 * @return {bool} ���ش����������� false ��ʾ����ʧ�ܻ���ɹ��Ҳ�����
	 *  �����ӣ�Ӧ�ر�����
	 */
	virtual bool doRun(void);

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯��
	 * @param session {session&} �洢 session ���ݵĶ���
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @return {bool} ���ش�����
	 */
	virtual bool doRun(session& session, socket_stream* stream = NULL);

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯����
	 * ���ñ�������ζ�Ų��� memcached ���洢 session ����
	 * @param memcached_addr {const char*} memcached ��������ַ����ʽ��IP:PORT
	 * @param stream {socket_stream*} ����ͬ��
	 * @return {bool} ���ش�����
	 */
	virtual bool doRun(const char* memcached_addr, socket_stream* stream);

protected:
	/**
	 * �� HTTP ����Ϊ GET ��ʽʱ���õ��麯��
	 */
	virtual bool doGet(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ websocket ��ʽʱ���õ��麯��
	 */
	virtual bool doWebSocket(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ POST ��ʽʱ���õ��麯��
	 */
	virtual bool doPost(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ PUT ��ʽʱ���õ��麯��
	 */
	virtual bool doPut(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ PATCH ��ʽʱ���õ��麯��
	 */
	virtual bool doPatch(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ CONNECT ��ʽʱ���õ��麯��
	 */
	virtual bool doConnect(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ PURGE ��ʽʱ���õ��麯�����÷�������� SQUID �Ļ���
	 * ʱ���õ�
	 */
	virtual bool doPurge(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ DELETE ��ʽʱ���õ��麯��
	 */
	virtual bool doDelete(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ HEAD ��ʽʱ���õ��麯��
	 */
	virtual bool doHead(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ OPTION ��ʽʱ���õ��麯��
	 */
	virtual bool doOptions(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ����Ϊ PROPFIND ��ʽʱ���õ��麯��
	 */
	virtual bool doPropfind(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP ���󷽷�δ֪ʱ���õ��麯��
	 * @param method {const char*} ����δ֪�����󷽷�
	 */
	virtual bool doOther(HttpServletRequest&, HttpServletResponse&,
		const char* method);

	/**
	 * �� HTTP ���󷽷�δ֪ʱ���õ��麯��
	 */
	virtual bool doUnknown(HttpServletRequest&, HttpServletResponse&);

	/**
	 * �� HTTP �������ʱ���õ��麯��
	 */
	virtual bool doError(HttpServletRequest&, HttpServletResponse&);

protected:
	HttpServletRequest* req_;
	HttpServletResponse* res_;

private:
	session* session_;
	session* session_ptr_;
	socket_stream* stream_;
	bool first_;
	char local_charset_[32];
	int  rw_timeout_;
	int  parse_body_limit_;

	void init();
};

} // namespace acl

#endif // ACL_CLIENT_ONLY
