#pragma once
#include "fiber_cpp_define.hpp"

struct ACL_FIBER_MUTEX;
struct ACL_FIBER_RWLOCK;

namespace acl {

/**
 * ��������ͬһ�߳��ڲ���Э��֮����л���Ļ�����
 */
class FIBER_CPP_API fiber_mutex
{
public:
	fiber_mutex(void);
	~fiber_mutex(void);

	/**
	 * �ȴ�������
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�ڲ�����
	 */
	bool lock(void);

	/**
	 * ���Եȴ�������
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool trylock(void);

	/**
	 * ������ӵ�����ͷ�����֪ͨ�ȴ���
	 * @return {bool} ���� true ��ʾ֪ͨ�ɹ��������ʾ�ڲ�����
	 */
	bool unlock(void);

private:
	ACL_FIBER_MUTEX* lock_;

	fiber_mutex(const fiber_mutex&);
	void operator=(const fiber_mutex&);
};

/**
 * ��������ͬһ�߳��ڵ�Э��֮����л���Ķ�д��
 */
class FIBER_CPP_API fiber_rwlock
{
public:
	fiber_rwlock(void);
	~fiber_rwlock(void);

	/**
	 * �Ӷ���
	 */
	void rlock(void);

	/**
	 * ���ԼӶ���
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool tryrlock(void);

	/**
	 * �����
	 */
	void runlock(void);

	/**
	 * ��д��
	 */
	void wlock(void);

	/**
	 * ���Լ�д��
	 * @return {bool} ���� true ��ʾ�����ɹ��������ʾ�����ڱ�ռ��
	 */
	bool trywlock(void);

	/**
	 * ��д��
	 */
	void wunlock(void);

private:
	ACL_FIBER_RWLOCK* rwlk_;

	fiber_rwlock(const fiber_rwlock&);
	void operator=(const fiber_rwlock&);
};

} // namespace acl
