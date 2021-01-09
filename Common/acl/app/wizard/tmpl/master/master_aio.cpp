#include "stdafx.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_str;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "str", "test_msg", &var_cfg_str },

	{ 0, 0, 0 }
};

int  var_cfg_bool;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "bool", 1, &var_cfg_bool },

	{ 0, 0, 0 }
};

int  var_cfg_int;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "int", 120, &var_cfg_int, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

//////////////////////////////////////////////////////////////////////////////

// acl::aio_callback ��������ඨ��
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client) : client_(client) {}

	~io_callback(void) {}

protected:
	/** 
	 * @override
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص����� 
	 * @param data {char*} ���������ݵ�ַ 
	 * @param len {int} ���������ݳ��� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	bool read_callback(char* data, int len)  
	{
		if (strncmp(data, "quit", 4) == 0) {
			// ������ʽ�����첽���Ĺرչ��̣�Ҳ����ֱ�ӷ��� false
			// ֪ͨ�첽����Զ��رո��첽��
			// client_->close();
			return false;
		}
		client_->write(data, len);
		return true;
	}

	/** 
	 * @override
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	bool write_callback(void)
	{
		return true;  
	}

	/** 
	 * @override
	 * ʵ�ָ����е��麯�����ͻ������Ĺرջص����� 
	 */  
	void close_callback(void)
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶  
		delete this;  
	}

	/** 
	 * @override
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	bool timeout_callback(void)
	{
		// ���� false ֪ͨ�첽��ܹرո��첽��
		return false;
	}

private:
	acl::aio_socket_stream* client_;
};

//////////////////////////////////////////////////////////////////////////////

master_service::master_service(void)
{
}

master_service::~master_service(void)
{
}

bool master_service::on_accept(acl::aio_socket_stream* client)
{
	logger("connect from %s, fd %d", client->get_peer(true),
		client->sock_handle());

	// �����첽�ͻ������Ļص���������첽�����а�
	io_callback* callback = new io_callback(client);

	// ע���첽���Ķ��ص�����
	client->add_read_callback(callback);

	// ע���첽����д�ص�����
	client->add_write_callback(callback);

	// ע���첽���Ĺرջص�����
	client->add_close_callback(callback);

	// ע���첽���ĳ�ʱ�ص�����
	client->add_timeout_callback(callback);

	// д��ӭ��Ϣ
	// client->format("hello, you're welcome\r\n");

	// ���첽����һ�����ݣ�����������һ������ʱ�ص� acl::aio_callback
	// �е� read_callback �麯��
	client->gets(10, false);  

	// ���첽���ж�ȡ���������ݣ����������ݺ�ص� acl::aio_callback
	// �е� read_callback �麯��
	// client->read();  

	// ����첽���Ķ�״̬���������ݿɶ�ʱ���ص� acl::aio_callback
	// �е� read_wakeup �麯��
	// client->read_wait();
	return true;
}

void master_service::proc_on_listen(acl::server_socket& ss)
{
	logger(">>>listen %s ok<<<", ss.get_addr());
}

void master_service::proc_on_init(void)
{
	logger(">>>proc_on_init<<<");
}

void master_service::proc_on_exit(void)
{
	logger(">>>proc_on_exit<<<");
}

bool master_service::proc_on_sighup(acl::string&)
{
	logger(">>>proc_on_sighup<<<");
	return true;
}
