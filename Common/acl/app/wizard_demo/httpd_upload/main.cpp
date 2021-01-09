#include "stdafx.h"
#include "master_service.h"

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_int64(var_conf_int64_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

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
			ms.run_alone(addrs, argv[2], count, max_threads);
		else
			ms.run_alone(addrs, NULL, count, max_threads);

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
