#pragma once
#include "fiber_cpp_define.hpp"

#if !defined(_WIN32) && !defined(_WIN64)

struct ACL_FIBER_COND;

namespace acl {

class fiber_event;

/**
 * ������Э��֮�䣬�߳�֮�䣬Э�����߳�֮�����������
 */
class FIBER_CPP_API fiber_cond
{
public:
	fiber_cond(void);
	~fiber_cond(void);

	/**
	 * �ȴ����������¼�������
	 * @param event {fiber_event&}
	 * @param timeout {int} ��ʱ�ȴ�ʱ�䣨���룩
	 * @return {bool} �ɹ�ʱ���� true�����򷵻� false ��ʾ��ʱ
	 */
	bool wait(fiber_event& event, int timeout = -1);

	/**
	 * ���������������ϵĵȴ��ߣ����û�еȴ�����ֱ�ӷ��أ�������Ϊ��
	 * �߳�������������
	 * @return {bool} �ɹ����� true�����򷵻� false ��ʾʧ��
	 */
	bool notify(void);

public:
	/**
	 * ���� C �汾��������������
	 * @return {ACL_FIBER_COND*}
	 */
	ACL_FIBER_COND* get_cond(void) const
	{
		return cond_;
	}

private:
	ACL_FIBER_COND* cond_;

	fiber_cond(const fiber_cond&);
	void operator=(const fiber_cond&);
};

}

#endif
