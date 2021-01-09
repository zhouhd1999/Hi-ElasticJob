#pragma once

//////////////////////////////////////////////////////////////////////////////
// ����������

extern char *var_cfg_str;
extern acl::master_str_tbl var_conf_str_tab[];

extern int  var_cfg_bool;
extern acl::master_bool_tbl var_conf_bool_tab[];

extern int  var_cfg_int;
extern acl::master_int_tbl var_conf_int_tab[];

extern long long int  var_cfg_int64;
extern acl::master_int64_tbl var_conf_int64_tab[];

//////////////////////////////////////////////////////////////////////////////

//class acl::aio_socket_stream;

class master_service : public acl::master_aio
{
public:
	master_service(void);
	~master_service(void);

protected:
	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * @return {bool} �ú���������� false ��֪ͨ��������ܲ��ٽ���
	 *  Զ�̿ͻ������ӣ�����������տͻ�������
	 */
	bool on_accept(acl::aio_socket_stream* stream);

	/**
	 * @override
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	void proc_on_init(void);

	/**
	 * @override
	 * �������˳�ǰ���õĻص�����
	 */
	void proc_on_exit(void);
};
