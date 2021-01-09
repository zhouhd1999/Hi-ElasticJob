#pragma once
#include "fiber_cpp_define.hpp"
#include <list>
#include <stdlib.h>
#include "fiber_event.hpp"
#include "fiber_cond.hpp"

#if !defined(_WIN32) && !defined(_WIN64)

namespace acl {

/**
 * ����Э��֮�䣬�߳�֮���Լ�Э�����߳�֮�����Ϣͨ�ţ�ͨ��Э����������
 * ��Э���¼���ʵ��
 *
 * ʾ����
 *
 * class myobj
 * {
 * public:
 *     myobj(void) {}
 *     ~myobj(void) {}
 *
 *     void test(void) { printf("hello world\r\n"); }
 * };
 *
 * acl::fiber_tbox<myobj> fiber_tbox;
 *
 * void thread_producer(void)
 * {
 *     myobj* o = new myobj;
 *     fiber_tbox.push(o);
 * }
 *
 * void thread_consumer(void)
 * {
 *     myobj* o = fiber_tbox.pop();
 *     o->test();
 *     delete o;
 * }
 */

template<typename T>
class fiber_tbox
{
public:
	/**
	 * ���췽��
	 * @param free_obj {bool} �� fiber_tbox ����ʱ���Ƿ��Զ���鲢�ͷ�
	 *  δ�����ѵĶ�̬����
	 */
	fiber_tbox(bool free_obj = true) : size_(0), free_obj_(free_obj) {}

	~fiber_tbox(void)
	{
		clear(free_obj_);
	}

	/**
	 * ������Ϣ������δ�����ѵ���Ϣ����
	 * @param free_obj {bool} �ͷŵ��� delete ����ɾ����Ϣ����
	 */
	void clear(bool free_obj = false)
	{
		if (free_obj) {
			for (typename std::list<T*>::iterator it =
				tbox_.begin(); it != tbox_.end(); ++it) {

				delete *it;
			}
		}
		tbox_.clear();
	}

	/**
	 * ������Ϣ����
	 * @param t {T*} �ǿ���Ϣ����
	 * @param notify_first {bool} ���������Ϊ true�����ڲ��������Ϣ��
	 *  ������֪ͨ�������ʽ����������Ƚ�����֪ͨ��ʽ���� fiber_tbox ����
	 *  ���������ڱȽϳ�ʱ���ò�����Ϊ false ��Ч�ʸ��ߣ���� fiber_tbox
	 *  ������������ڽ϶�(�磺�ȴ��ߵ��� pop ��ֱ������ fiber_tbox ����),
	 *  �򱾲���Ӧ����Ϊ true���Ա��� push �߻�û����ȫ����ǰ�� fiber_tbox
	 *  ������ǰ���ٶ�����ڴ�Ƿ�����
	 * @return {bool}
	 */
	bool push(T* t, bool notify_first = true)
	{
		// �ȼ���
		if (event_.wait() == false) {
			abort();
		}

		// ������������Ϣ����
		tbox_.push_back(t);
		size_++;

		if (notify_first) {
			if (cond_.notify() == false) {
				abort();
			}
			if (event_.notify() == false) {
				abort();
			}
			return true;
		} else {
			if (event_.notify() == false) {
				abort();
			}
			if (cond_.notify() == false) {
				abort();
			}
			return true;
		}
	}

	/**
	 * ������Ϣ����
	 * @param wait_ms {int} >= 0 ʱ���õȴ���ʱʱ��(���뼶��)��
	 *  ������Զ�ȴ�ֱ��������Ϣ��������
	 * @param found {bool*} �ǿ�ʱ��������Ƿ�����һ����Ϣ������Ҫ����
	 *  �������ݿն���ʱ�ļ��
	 * @return {T*} �� NULL ��ʾ���һ����Ϣ���󣬷��� NULL ʱ����Ҫ����һ
	 *  ����飬��������� push ��һ���ն���NULL������������Ҳ���� NULL��
	 *  ����ʱ��Ȼ��Ϊ�����һ����Ϣ����ֻ����Ϊ�ն������ wait_ms ����
	 *  Ϊ -1 ʱ���� NULL ��Ȼ��Ϊ�����һ������Ϣ������� wait_ms ����
	 *  ���� 0 ʱ���� NULL����Ӧ�ü�� found ������ֵΪ true ���� false ��
	 *  �ж��Ƿ�����һ������Ϣ����
	 */
	T* pop(int wait_ms = -1, bool* found = NULL)
	{
		bool found_flag;
		if (event_.wait() == false) {
			abort();
		}
		while (true) {
			T* t = peek(found_flag);
			if (found_flag) {
				if (event_.notify() == false) {
					abort();
				}
				if (found) {
					*found = found_flag;
				}
				return t;
			}

			// ע�����˳�򣬱����ȵ��� wait ���ж� wait_ms
			if (!cond_.wait(event_, wait_ms) && wait_ms >= 0) {
				if (event_.notify() == false) {
					abort();
				}
				if (found) {
					*found = false;
				}
				return NULL;
			}
		}
	}

	/**
	 * ���ص�ǰ��������Ϣ�����е���Ϣ����
	 * @return {size_t}
	 */
	size_t size(void) const
	{
		return size_;
	}

public:
	void lock(void)
	{
		if (event_.wait() == false) {
			abort();
		}
	}

	void unlock(void)
	{
		if (event_.notify() == false) {
			abort();
		}
	}

private:
	fiber_tbox(const fiber_tbox&) {}
	const fiber_tbox& operator=(const fiber_tbox&);

private:
	std::list<T*> tbox_;
	size_t        size_;
	bool          free_obj_;
	fiber_event   event_;
	fiber_cond    cond_;

	T* peek(bool& found_flag)
	{
		typename std::list<T*>::iterator it = tbox_.begin();
		if (it == tbox_.end()) {
			found_flag = false;
			return NULL;
		}
		found_flag = true;
		size_--;
		T* t = *it;
		tbox_.erase(it);
		return t;
	}
};

} // namespace acl

#endif
