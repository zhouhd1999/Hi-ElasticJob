#include "stdafx.h"
#include "configure.h"
#include "tcp_service.h"
#include "udp_service.h"

static void init_configure(acl::master_base& base)
{
	// �������ò�����
	base.set_cfg_int(var_conf_int_tab);
	base.set_cfg_int64(var_conf_int64_tab);
	base.set_cfg_str(var_conf_str_tab);
	base.set_cfg_bool(var_conf_bool_tab);
}

static void run_tcp_service(int argc, char* argv[])
{
	tcp_service& ts = acl::singleton2<tcp_service>::get_instance();
	init_configure(ts);

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		// ��־�������׼���
		acl::log::stdout_open(true);

		// �����ĵ�ַ�б���ʽ��ip:port1,ip:port2,...
		const char* addrs = ":8888";
		printf("listen on: %s\r\n", addrs);

		// ����ʱ���ø�ֵ > 0 ��ָ������������ͻ������ӹ��̵�
		// �Ự������һ�����Ӵӽ��յ��رճ�֮Ϊһ���Ự������
		// ��������ӻỰ��������ֵ�����Թ��̽����������ֵ��
		// Ϊ 0������Թ�����Զ������
		unsigned int count = 0;

		// ���Թ�����ָ���̳߳�����̸߳���
		unsigned int max_threads = 100;

		// �������з�ʽ

		if (argc >= 3)
			ts.run_alone(addrs, argv[2], count, max_threads);
		else
			ts.run_alone(addrs, NULL, count, max_threads);

		printf("Enter any key to exit now\r\n");
		getchar();
	}
	else
	{
#ifdef	WIN32
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
		ts.run_alone(addrs, NULL, count, max_threads);
		printf("Enter any key to exit now\r\n");
		getchar();
#else
		// acl_master ����ģʽ����
		ts.run_daemon(argc, argv);
#endif
	}
}

static void run_udp_service(int argc, char* argv[])
{
	udp_service& us = acl::singleton2<udp_service>::get_instance();
	init_configure(us);

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		// ��־�������׼���
		acl::log::stdout_open(true);

		// �����ĵ�ַ�б���ʽ��ip:port1,ip:port2,...
		const char* addrs = "0.0.0.0:8390";
		printf("bind on: %s\r\n", addrs);

		// ����ʱ���ø�ֵ > 0 ��ָ������������ͻ������ӹ��̵�
		// �Ự������һ�����Ӵӽ��յ��رճ�֮Ϊһ���Ự������
		// ��������ӻỰ��������ֵ�����Թ��̽����������ֵ��
		// Ϊ 0������Թ�����Զ������
		unsigned int count = 0;

		// �������з�ʽ
		if (argc >= 3)
			us.run_alone(addrs, argv[2], count);
		else
			us.run_alone(addrs, NULL, count);

		printf("Enter any key to exit now\r\n");
		getchar();
	}
	else
	{
#ifdef	WIN32
		// ��־�������׼���
		acl::log::stdout_open(true);

		// �����ĵ�ַ�б���ʽ��ip:port1,ip:port2,...
		const char* addrs = "0.0.0.0:8390";
		printf("bind on: %s\r\n", addrs);

		// ����ʱ���ø�ֵ > 0 ��ָ������������ͻ������ӹ��̵�
		// �Ự������һ�����Ӵӽ��յ��رճ�֮Ϊһ���Ự������
		// ��������ӻỰ��������ֵ�����Թ��̽����������ֵ��
		// Ϊ 0������Թ�����Զ������
		unsigned int count = 0;

		// �������з�ʽ
		us.run_alone(addrs, NULL, count);
		printf("Enter any key to exit now\r\n");
		getchar();
#else
		// acl_master ����ģʽ����
		us.run_daemon(argc, argv);
#endif
	}
}

static bool check_service(int argc, char* argv[])
{
	bool is_tcp_service = false;

	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] != '-')
			continue;
		switch (argv[i][1])
		{
		case 'M':
			if (i + 1 < argc && !strcasecmp(argv[i + 1], "tcp"))
			{
				is_tcp_service = true;
				break;
			}
		default:
			break;
		}
	}

	return is_tcp_service;
}

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	bool tcp_mode = check_service(argc, argv);

	if (tcp_mode)
		run_tcp_service(argc, argv);
	else
		run_udp_service(argc, argv);
	return 0;
}
