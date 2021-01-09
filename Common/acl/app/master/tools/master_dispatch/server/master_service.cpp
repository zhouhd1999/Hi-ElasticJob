#include "stdafx.h"
#include "client/ManagerTimer.h"
#include "client/ClientConnection.h"
#include "status/StatusConnection.h"
#include "status/StatusTimer.h"
#include "server/ServerConnection.h"
#include "server/ServerTimer.h"
#include "rpc_manager.h"
#include "allow_list.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_backend_service;
char *var_cfg_status_servers;
char *var_cfg_status_service;
char *var_cfg_session_addr;  // memcache ��������ַ���Ա�����ʹ��
char *var_cfg_rpc_addr;
char *var_cfg_manager_allow;
char *var_cfg_service_name;
char *var_cfg_nic_names;
char *var_cfg_net_addrs;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "backend_service", "dispatch.sock", &var_cfg_backend_service },
	{ "status_servers", "", &var_cfg_status_servers },
	{ "status_service", "1080", &var_cfg_status_service },
	{ "session_addr", "127.0.0.1:11211", &var_cfg_session_addr },
	{ "rpc_addr", "127.0.0.1:0", &var_cfg_rpc_addr },
	{ "manager_allow", "127.0.0.1:127.0.0.1, 192.168.0.0:192.168.255.255",
		&var_cfg_manager_allow },
	{ "service_name", "dispatch_service", &var_cfg_service_name },
	{ "nic_names", "em; eth", &var_cfg_nic_names },
	{ "net_addrs", "192.168;10.0", &var_cfg_net_addrs },

	{ 0, 0, 0 }
};

acl::master_bool_tbl var_conf_bool_tab[] = {
	{ 0, 0, 0 }
};

int   var_cfg_manage_timer;
int   var_cfg_conn_expired;
int   var_cfg_status_timer;
int   var_cfg_server_timer;
int   var_cfg_rw_timeout;
int   var_cfg_rpc_nthreads;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "manager_timer", 1, &var_cfg_manage_timer, 0, 0 },
	{ "conn_expired", 10, &var_cfg_conn_expired, 0, 0 },
	{ "status_timer", 1, &var_cfg_status_timer, 0, 0 },
	{ "server_timer", 1, &var_cfg_server_timer, 0, 0 },
	{ "rw_timeout", 30, &var_cfg_rw_timeout, 0, 0 },
	{ "rpc_nthreads", 100, &var_cfg_rpc_nthreads, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {
	{ 0, 0 , 0 , 0, 0 }
};

// ���� IP ��ַ�����Ȳ������� IP
acl::string var_cfg_local_addr;
;
//////////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::on_accept(acl::aio_socket_stream* client)
{
	if (0)
		logger("connect from %s, fd %d", client->get_peer(true),
			client->sock_handle());

	acl_non_blocking(client->sock_handle(), ACL_BLOCKING);

	IConnection* conn;

	// ���ݿͻ������ӷ���˿ںŵĲ�ͬ�����ֲ�ͬ�ķ���Ӧ��Э��
	const char* local = client->get_local(true);
	if (acl_strrncasecmp(local, var_cfg_backend_service,
		strlen(var_cfg_backend_service)) == 0)
	{
		// ������������������ں�˷���ģ�������
		conn = new ServerConnection(client);
	}
	else if (acl_strrncasecmp(local, var_cfg_status_service,
		strlen(var_cfg_status_service)) == 0)
	{
		const char* ip = client->get_peer();
		if (ip == NULL || *ip == 0)
		{
			logger_error("can't get peer ip");
			return false;
		}
		if (allow_list::get_instance().allow_manager(ip) == false)
		{
			logger_warn("deny manager ip: %s", ip);
			return false;
		}

		// �������������״̬�㱨������
		conn = new StatusConnection(client);
	}
	else
		// ����������������ǰ�˿ͻ���ģ�������
		conn = new ClientConnection(client, var_cfg_conn_expired);

	conn->run();

	return true;
}

void master_service::find_addr_include(acl::string& name, acl::string& addr,
	const char* nic_names, const char* net_addrs)
{
	ACL_IFCONF *ifconf;	/* ������ѯ������� */
	ACL_IFADDR *ifaddr;	/* ÿ��������Ϣ���� */
	ACL_ITER iter;		/* �������� */

	/* ��ѯ��������������Ϣ */
	ifconf = acl_get_ifaddrs();

	if (ifconf == NULL)
	{
		logger_error("acl_get_ifaddrs error: %s, use 127.0.0.1",
			acl::last_serror());
		name = "lo";
		addr = "127.0.0.1";
		return;
	}

	acl::string nic_names_buf(nic_names);
	acl::string net_addrs_buf(net_addrs);
	const std::vector<acl::string>& names = nic_names_buf.split2(";, \t");
	const std::vector<acl::string>& addrs = net_addrs_buf.split2(";, \t");

	bool find_nic;

	/* ����������������Ϣ, �����ҳ�ƥ������������ַ��ѡ�� */
	acl_foreach(iter, ifconf)
	{
		ifaddr = (ACL_IFADDR*) iter.data;
		const char* ptr = ifaddr->name;

		if (ptr == NULL || *ptr == 0)
		{
			logger_error("ifaddr->name NULL");
			continue;
		}

		// �ҵ�������ƥ��ĵ�ַ
		find_nic = false;
		for (std::vector<acl::string>::const_iterator cit
			= names.begin(); cit != names.end(); ++cit)
		{
			if ((*cit).ncompare(ptr, (*cit).size(), false) == 0)
			{
				find_nic = true;
				name = ptr;
				break;
			}
		}

		if (find_nic == false)
			continue;

		// �ҵ� IP ��ַƥ��ĵ�ַ
		for (std::vector<acl::string>::const_iterator cit
			= addrs.begin(); cit != addrs.end(); ++cit)
		{
			if (strstr(ifaddr->addr, (*cit).c_str()) != NULL)
			{
				addr = ifaddr->addr;
				break;
			}
		}

		if (!addr.empty())
			break;
	}

	/* �ͷŲ�ѯ��� */
	acl_free_ifaddrs(ifconf);

	if (addr.empty())
	{
		logger_warn("no addr matched, use 127.0.0.1 instead");
		name = "lo";
		addr = "127.0.0.1";
	}
}

void master_service::proc_on_init()
{
	acl::string name, addr;
	find_addr_include(name, addr, var_cfg_nic_names, var_cfg_net_addrs);

	var_cfg_local_addr << addr << ":" << var_cfg_service_name;

	logger("-----local addr: %s------", var_cfg_local_addr.c_str());

	if (var_cfg_manager_allow && *var_cfg_manager_allow)
		allow_list::get_instance()
			.set_allow_manager(var_cfg_manager_allow);

	if (var_cfg_manage_timer <= 0)
		var_cfg_manage_timer = 1;

	// ������̨��ʱ������������δ�����ǰ�˿ͻ�������
	manage_timer_ = new ManagerTimer();
	manage_timer_->keep_timer(true);
	manage_timer_->set_task(1, var_cfg_manage_timer * 1000000);

	// ���û��෽�����ö�ʱ������
	proc_set_timer(manage_timer_);

	// ���������״̬��������������״̬�㱨��ʱ������ʱ��״̬������
	// �㱨����״̬
	if (var_cfg_status_servers && *var_cfg_status_servers
		&& var_cfg_status_timer > 0)
	{
		// ����������״̬�㱨��ʱ��
		status_timer_ = new StatusTimer();
		status_timer_->keep_timer(true);
		status_timer_->set_task(1, var_cfg_status_timer * 1000000);
		proc_set_timer(status_timer_);
	}

	// ���������״̬����������״̬���ܶ�ʱ�����Եȴ�����˵���������
	if (var_cfg_status_service && *var_cfg_status_service
		&& var_cfg_server_timer > 0)
	{
		// ����ͳ�����з�����״̬��ʱ��
		server_timer_ = new ServerTimer();
		server_timer_->keep_timer(true);
		server_timer_->set_task(1, var_cfg_server_timer * 1000000);
		proc_set_timer(server_timer_);
	}

	// ���û��ຯ������첽������
	acl::aio_handle* handle = get_handle();
	assert(handle != NULL);

	// ��ʼ�� RPC ���
	rpc_manager::get_instance().init(handle, var_cfg_rpc_nthreads,
		var_cfg_rpc_addr);
}

void master_service::proc_on_exit()
{
}
