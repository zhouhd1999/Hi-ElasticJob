#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/thread_mutex.hpp"
#include "sslbase_conf.hpp"
#include <vector>

namespace acl
{

/**
 * SSL ֤��У�鼶�����Ͷ���
 */
typedef enum
{
	MBEDTLS_VERIFY_NONE,	// ��У��֤��
	MBEDTLS_VERIFY_OPT,	// ѡ����У�飬����������ʱ�����ֺ�У��
	MBEDTLS_VERIFY_REQ	// Ҫ��������ʱУ��
} mbedtls_verify_t;

class mbedtls_io;

/**
 * SSL ���Ӷ���������࣬�������һ���������Ϊȫ�ֶ���������ÿһ�� SSL
 * ���Ӷ������֤�����ã����������ȫ���Ե�֤�顢��Կ����Ϣ��ÿһ�� SSL ����
 * (mbedtls_io) ���ñ������setup_certs ��������ʼ�������֤�顢��Կ����Ϣ
 */
class ACL_CPP_API mbedtls_conf : public sslbase_conf
{
public:
	/**
	 * ���캯��
	 * @param server_side {bool} ����ָ���Ƿ���˻��ǿͻ��ˣ���Ϊ true ʱ
	 *  Ϊ�����ģʽ������Ϊ�ͻ���ģʽ
	 * @param verify_mode {mbedtls_verify_t} SSL ֤��У�鼶��
	 */
	mbedtls_conf(bool server_side = false,
		mbedtls_verify_t verify_mode = MBEDTLS_VERIFY_NONE);
	~mbedtls_conf(void);

	/**
	 * @override
	 */
	bool load_ca(const char* ca_file, const char* ca_path);

	/**
	 * @override
	 */
	bool append_key_cert(const char* crt_file, const char* key_file,
		const char* key_pass = NULL);

	/**
	 * @override
	 */
	bool add_cert(const char* crt_file);

	/**
	 * @override
	 */
	bool set_key(const char* key_file, const char* key_pass = NULL);

	/**
	 * @override
	 */
	void enable_cache(bool on);

public:
	/**
	 * mbedtls_io::open �ڲ�����ñ�����������װ��ǰ SSL ���Ӷ����֤��
	 * @param ssl {void*} SSL ���Ӷ���Ϊ ssl_context ����
	 * @return {bool} ���� SSL �����Ƿ�ɹ�
	 */
	bool setup_certs(void* ssl);

	/**
	 * �����������������ض���
	 * @return {void*}������ֵΪ entropy_context ����
	 */
	void* get_entropy(void)
	{
		return entropy_;
	}

public:
	/**
	 * ��� mbedtls �ֳ������⣬���Ե��ñ���������������̬���ȫ·��
	 * @param libmbedcrypto {const char*} libmbedcrypto ��̬���ȫ·��
	 * @param libmbedx509 {const char*} libmbedx509 ��̬���ȫ·��
	 * @param libmbedtls {const char*} libmbedtls ��̬���ȫ·��
	 */
	static void set_libpath(const char* libmbedcrypto,
		const char* libmbedx509, const char* libmbedtls);

	/**
	 * ��� mbedtls �ϳ�һ���⣬���Ե��ñ���������һ����̬���ȫ·��
	 * @param libmbedtls {const char*} libmbedtls ��̬���ȫ·��
	 */
	static void set_libpath(const char* libmbedtls);

	/**
	 * ��ʽ���ñ���������̬���� mbedtls ��̬��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool load(void);

public:
	// @override sslbase_conf
	sslbase_io* create(bool nblock);

private:
	friend class mbedtls_io;

	unsigned init_status_;
	unsigned cert_status_;
	thread_mutex lock_;

	bool  server_side_;

	void* conf_;
	void* entropy_;
	void* rnd_;
	void* cacert_;
	void* pkey_;
	void* cert_chain_;
	void* cache_;
	mbedtls_verify_t verify_mode_;
	std::vector<std::pair<void*, void*> > cert_keys_;

private:
	bool init_once(void);
	bool init_rand(void);
	void free_ca(void);
};

} // namespace acl
