#pragma once

/**
 * ��������������񽻸����̴߳���Ϊ�˷���ʹ�ã�����������Ϊ������
 */
class rpc_manager : public acl::singleton<rpc_manager>
{
public:
	rpc_manager(void);
	~rpc_manager(void);

	/**
	 * ������ʼ������
	 * @param handle {acl::aio_handle&} �첽������
	 * @param max_threads {int} ���̳߳ص�����߳�����
	 * @param addr {const char*} rpc ��������ĵ�ַ�����Ϊ�գ����ڲ���ϵ
	 *  ͳ�Զ�����һ�� 127.0.0.1:PORT ��ַ���м������ǿ�ʱ�����ָ���ĵ�ַ
	 */
	void init(acl::aio_handle& handle, int max_threads = 10,
		const char* addr = NULL);

	/**
	 * ����һ���������̣����ù��̽������̴߳���
	 * @param req {acl::rpc_request*} �����������
	 */
	void fork(acl::rpc_request* req);

private:
	// �첽��Ϣ���
	acl::aio_handle* handle_;

	// �첽 RPC ͨ�ŷ�����
	acl::rpc_service* service_;
};
