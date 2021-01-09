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

class master_service : public acl::master_trigger
{
public:
	master_service(void);
	~master_service(void);

protected:
	/**
	 * @override
	 * ��������ʱ�䵽ʱ���ô˺���
	 */
	void on_trigger(void);

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

	/**
	 * @override
	 * �������յ� SIGHUP �źź�Ļص�����
	 */
	bool proc_on_sighup(acl::string&);
};
