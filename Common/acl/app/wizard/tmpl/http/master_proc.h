#pragma once

class http_service;

class master_service : public acl::master_proc
{
public:
	master_service(void);
	~master_service(void);

	http_service& get_service(void) const;

public:
	/**
	 * ���� CGI ��ʽ����ʱ�����
	 */
	void do_cgi(void);

protected:
	/**
	 * @override
	 * �麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * ע���ú������غ������ӽ��ᱻ�رգ��û���Ӧ�����رո���
	 */
	void on_accept(acl::socket_stream* stream);

	/**
	 * @override
	 * �ڽ�������ʱ���������ÿ�ɹ�����һ�����ص�ַ������ñ�����
	 * @param ss {acl::server_socket&} ��������
	 */
	void proc_on_listen(acl::server_socket& ss);

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

private:
	http_service* service_;
};
