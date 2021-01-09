#include "stdafx.h"
#include "http_servlet.h"

http_servlet::http_servlet(const char* filepath, acl::socket_stream* conn,
	acl::session* session)
	: acl::HttpServlet(conn, session)
{
	if (filepath && *filepath)
		filepath_ = filepath;
}

http_servlet::~http_servlet(void)
{

}

bool http_servlet::reply(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, int status, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	acl::string  buf;
	buf.vformat(fmt, ap);
	va_end(ap);
	res.setStatus(status)
		.setKeepAlive(req.isKeepAlive())
		.setContentType("text/html; charset=utf-8")
		.setContentLength(buf.length());
	return res.write(buf);
}

bool http_servlet::doError(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	acl::string hdr;
	req.getClient()->sprint_header(hdr);
	logger("error request head:\r\n%s\r\n", hdr.c_str());

	(void) reply(req, res, 400, "unknown request method\r\n");
	return false;
}

bool http_servlet::doUnknown(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	acl::string hdr;
	req.getClient()->sprint_header(hdr);
	logger("request head:\r\n%s\r\n", hdr.c_str());

	(void) reply(req, res, 400, "unknown request method\r\n");
	return false;
}

bool http_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	// �����Ҫ http session ���ƣ��������ע�ͣ�����Ҫ��֤
	// �� master_service.cpp �ĺ��� thread_on_read �����õ�
	// memcached ������������
	/*
	const char* sid = req.getSession().getAttribute("sid");
	if (*sid == 0)
		req.getSession().setAttribute("sid", "xxxxxx");
	sid = req.getSession().getAttribute("sid");
	*/

	// ���ԣ����ͻ�������ͷ��¼����־��
	acl::string hdr;
	req.getClient()->sprint_header(hdr);
	logger("request head:\r\n%s\r\n", hdr.c_str());

	long long int range_from, range_to;
	if (req.getRange(range_from, range_to) == false)
		return transfer_file(req, res);
	else
		return transfer_file(req, res, range_from, range_to);
}

// ��ͨ���ع���
bool http_servlet::transfer_file(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	acl::ifstream in;

	// ֻ����ʽ���ļ���
	if (in.open_read(filepath_) == false)
	{
		logger_error("open file %s error %s", filepath_.c_str(),
			acl::last_serror());
		return reply(req, res, 500, "open %s error %s",
				filepath_.c_str(), acl::last_serror());
	}

	long long int fsize = in.fsize();
	if (fsize <= 0)
		return reply(req, res, 500, "invalid file size: %lld", fsize);

	acl::string hdr_entry;
	acl::string filename;
	filename.basename(in.file_path());  // ���ļ�ȫ·������ȡ�ļ���
	hdr_entry.format("attachment;filename=\"%s\"", filename.c_str());

	// ���� HTTP ��Ӧͷ�е��ֶ�
	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(fsize)
		.setContentType("application/octet-stream")
		// ���� HTTP ͷ�е��ļ���
		.setHeader("Content-Disposition", hdr_entry.c_str());

	acl::string hdr;
	res.getHttpHeader().build_response(hdr);
	logger("response head:\r\n%s\r\n", hdr.c_str());

	long long n = 0;
	char  buf[8192];

	// ���ļ����ж�ȡ���ݲ������ݷ����ͻ���
	while (!in.eof())
	{
		int ret = in.read(buf, sizeof(buf), false);
		if (ret == -1)
			break;
		if (res.write(buf, ret) == false)
			return false;
		n += ret;
		//acl_doze(100);  // ��Ϣ 100 ms ���ڲ���
	}

	return n == fsize ? true : false;
}

// ֧�ֶϵ����������ݴ������
bool http_servlet::transfer_file(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res,
	long long range_from, long long range_to)
{
	if (range_from < 0)
		return reply(req, res, 400, "invalid range_from: %lld",
				range_from);
	if (range_to > 0 && range_to < range_from)
		return reply(req, res, 400, "range_from(%lld) > range_to(%lld)",
				range_from, range_to);

	long long length;
	if (range_to >= range_from)
		length = range_to - range_from + 1;
	else
		length = -1;

	acl::ifstream in;
	// ֻ����ʽ���ļ���
	if (in.open_read(filepath_) == false)
	{
		logger_error("open file %s error %s", filepath_.c_str(),
			acl::last_serror());
		return reply(req, res, 500, "open %s error %s",
			filepath_.c_str(), acl::last_serror());
	}

	long long int fsize = in.fsize();
	if (fsize <= 0)
		return reply(req, res, 500, "invalid file size: %lld", fsize);

	// ���ƫ��λ�ó������ļ��ܳ��ȣ��򷵻ش���
	if (range_to >= fsize)
		return reply(req, res, 400, "range_to(%lld) >= fsize(%lld)",
			range_to, fsize);

	// ����ͻ���Ҫ���ָ��ƫ��λ�����ļ�β�������¼�����Ҫ��ȡ�����ݳ��ȣ�
	// ��ֵ��Ϊʵ��Ҫ������ͻ��˵ĳ���
	if (length == -1)
		length = fsize - range_from;

	// ��λ�ļ�ƫ��λ��
	if (in.fseek(range_from, SEEK_SET) < 0)
		return reply(req, res, 500, "fseek(%lld) error %s",
			range_from, acl::last_serror());

	acl::string hdr_entry;
	acl::string filename;
	filename.basename(in.file_path());  // ���ļ�ȫ·������ȡ�ļ���
	hdr_entry.format("attachment;filename=\"%s\"", filename.c_str());

	// ���� HTTP ��Ӧͷ�е��ֶ�
	res.setStatus(206)			// ��Ӧ״̬ 206 ��ʾ��������
		.setKeepAlive(req.isKeepAlive())// �Ƿ񱣳ֳ�����
		.setContentLength(length)	// ʵ��Ҫ��������ݳ���
		.setContentType("application/octet-stream")  // ��������
		// ���� HTTP ͷ�е��ļ���
		.setHeader("Content-Disposition", hdr_entry.c_str())
		// ���ñ��δ����������ʼƫ��λ�ü����ݵ��ܳ���
		.setRange(range_from, range_to > 0
				? range_to : fsize - 1, fsize);
	
	// ���ԣ��� HTTP ��Ӧͷ���ڱ�����־��
	acl::string hdr;
	res.getHttpHeader().build_response(hdr);
	logger("response head:\r\n%s\r\n", hdr.c_str());

	char  buf[8192];
	int   ret;
	size_t size;

	// ���ļ�ָ��λ�ö�ȡ���ݣ��������ݴ�����ͻ���
	while (!in.eof() && length > 0)
	{
		size = sizeof(buf) > (size_t) length ?
				(size_t) length : sizeof(buf);

		// ����������Ϊ false ��ʾ������һ�ζ����������ش������������ٷ���
		ret = in.read(buf, size, false);
		if (ret == -1)
		{
			printf("read over: %s\r\n", acl::last_serror());
			break;
		}
		if (res.write(buf, ret) == false)
			return false;
		length -= ret;
		//acl_doze(100);  // ��Ϣ 100 ms ���ڲ���
	}

	if (length != 0)
	{
		logger_error("read file failed");
		return false;
	}

	return true;
}
