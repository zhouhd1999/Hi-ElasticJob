#include "stdafx.h"
#include "master_service.h"
#include "http_service.h"

#if __cplusplus >= 201103L	// Support c++11 ?
# define USE_LAMBDA
#endif

#ifndef USE_LAMBDA
static bool http_get_default(const char* path, HttpRequest&, HttpResponse& res)
{
	acl::string buf;
	buf.format("default(%s): hello world!\r\n", path);
	res.setContentLength(buf.size());
	return res.write(buf);
}
#endif

static bool http_get_root(HttpRequest&, HttpResponse& res)
{
	acl::string buf("hello world!\r\n");
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_get_ok(HttpRequest&, HttpResponse& res)
{
	acl::string buf("ok: hello world!\r\n");
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_post_ok(HttpRequest& req, HttpResponse& res)
{
	acl::string buf;
	if (!req.getBody(buf)) {
		printf("getBody error!\r\n");
		return false;
	}

	printf("body: %s\r\n", buf.c_str());
	res.setContentLength(buf.size());
	return res.write(buf);
}

static bool http_get_json(HttpRequest&, HttpResponse& res)
{
	acl::json json;
	acl::json_node& root = json.get_root();
	root.add_number("code", 200)
		.add_text("status", "+ok")
		.add_child("data",
			json.create_node()
				.add_text("name", "value")
				.add_bool("success", true)
				.add_number("number", 100));
	return res.write(json);
}

static bool http_get_xml(HttpRequest&, HttpResponse& res)
{
	acl::xml1 xml;
	acl::xml_node& root = xml.get_root();
	root.add_child("test6", true, "text6")
		.add_child("test61", true, "text61")
			.add_attr("name61_1", "value61_1")
			.add_attr("name61_2", "value61_2")
			.add_attr("name61_3", "value61_3")
			.add_child("test611", true, "text611")
				.add_attr("name611_1", "value611_1")
				.add_attr("name611_2", "value611_2")
				.add_attr("name611_3", "value611_3")
				.get_parent()
			.get_parent()
		.add_child("test62", true, "text62")
			.add_attr("name62_1", "value62_1")
			.add_attr("name62_2", "value62_2")
			.add_attr("name62_3", "value62_3")
			.get_parent()
		.add_attr("name6_1", "value6_1")
		.add_attr("name6_2", "value6_2")
		.add_attr("name6_3", "value6_3");

	return res.write(xml);
}

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	master_service ms;
	http_service& service = ms.get_service();;

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab)
		.set_cfg_int64(var_conf_int64_tab)
		.set_cfg_str(var_conf_str_tab)
		.set_cfg_bool(var_conf_bool_tab);

	// Register http handlers according different url path
	service.Get("/", http_get_root)
		.Get("/ok", http_get_ok)
		.Post("/ok", http_post_ok)
		.Get("/json", http_get_json)
		.Get("/xml", http_get_xml)
#ifdef USE_LAMBDA
		.Get("/test1", [](HttpRequest&, HttpResponse& res) {
			acl::string buf("test1: hello world!\r\n");
			res.setContentLength(buf.size());
			return res.write(buf);
		})
		.Get("/test2", [&](HttpRequest&, HttpResponse& res) {
			acl::string buf("test2: hello world!\r\n");
			res.setContentLength(buf.size());
			return res.write(buf);
		})
		.Default([](const char* path, HttpRequest&, HttpResponse& res) {
			acl::string buf;
			buf.format("Default(%s): hello world!\r\n", path);
			res.setContentLength(buf.size());
			return res.write(buf);
		});
#else
		.Default(http_get_default);
#endif

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0) {
		// ��־�������׼���
		acl::log::stdout_open(true);

		// �����ĵ�ַ�б���ʽ��ip|port1,ip|port2,...
		const char* addrs = "|8888";
		printf("listen on: %s\r\n", addrs);

		// ����ʱ���ø�ֵ > 0 ��ָ������������ͻ������ӹ��̵�
		// �Ự������һ�����Ӵӽ��յ��رճ�֮Ϊһ���Ự������
		// ��������ӻỰ��������ֵ�����Թ��̽����������ֵ��
		// Ϊ 0������Թ�����Զ������
		unsigned int count = 0;

		// ���Թ�����ָ���̳߳�����̸߳���
		unsigned int max_threads = 100;

		// �������з�ʽ

		if (argc >= 3) {
			ms.run_alone(addrs, argv[2], count, max_threads);
		} else {
			ms.run_alone(addrs, NULL, count, max_threads);
		}

		printf("Enter any key to exit now\r\n");
		getchar();
	} else {
#if defined(_WIN32) || defined(_WIN64)
		// ��־�������׼���
		acl::log::stdout_open(true);

		// �����ĵ�ַ�б���ʽ��ip:port1,ip:port2,...
		const char* addrs = "127.0.0.1:8888";
		printf("listen on: %s\r\n", addrs);

		// ����ʱ���ø�ֵ > 0 ��ָ������������ͻ������ӹ��̵�
		// �Ự������һ�����Ӵӽ��յ��رճ�֮Ϊһ���Ự������
		// ��������ӻỰ��������ֵ�����Թ��̽����������ֵ��
		// Ϊ 0������Թ�����Զ������
		unsigned int count = 0;

		// ���Թ�����ָ���̳߳�����̸߳���
		unsigned int max_threads = 100;

		// �������з�ʽ
		ms.run_alone(addrs, NULL, count, max_threads);
		printf("Enter any key to exit now\r\n");
		getchar();
#else
		// acl_master ����ģʽ����
		ms.run_daemon(argc, argv);
#endif
	}

	return 0;
}
