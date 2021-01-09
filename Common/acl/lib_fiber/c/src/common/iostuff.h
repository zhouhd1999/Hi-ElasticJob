#ifndef	IOSTUFF_INCLUDE_H
#define	IOSTUFF_INCLUDE_H
#include <fiber/fiber_define.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define CLOSE_ON_EXEC   1  /**< ��־λ, ���� exec ���Զ��رմ򿪵������� */
#define PASS_ON_EXEC    0

#define BLOCKING        0  /**< ������д��־λ */
#define NON_BLOCKING    1  /**< ��������д��־λ */

/**
 * �����׽ӿ�Ϊ�����������
 * @param fd {int} �׽���
 * @param on {int} �Ƿ����ø��׽���Ϊ������, BLOCKING �� NON_BLOCKING
 * @return {int} >= 0: �ɹ�, ����ֵ > 0 ��ʾ����֮ǰ�ı�־λ; -1: ʧ��
 */
int non_blocking(socket_t fd, int on);

/**
 * ���뼶��˯��
 * @param delay {unsigned} ����ֵ
 */
void doze(unsigned delay);

/**
 * �����ļ���������־λ�������� exec ����������Զ����ر�
 * @param fd {int} �ļ�������
 * @param on {int} 1 �� 0
 * @return {int} 0: ok; -1: error
 */
int close_on_exec(int fd, int on);

/**
 * �趨��ǰ���̿��Դ�����ļ�������ֵ
 * @param limit {int} �趨�����ֵ
 * @return {int} >=0: ok; -1: error
 */
int open_limit(int limit);

/**
 * �жϸ���ĳ���ļ��������Ƿ����׽���
 * @param fd {int} �ļ�������
 * @return {int} != 0: ��; 0: ��
 */
int issock(int fd);

//int read_wait(int fd, int timeout);
void tcp_nodelay(socket_t fd, int onoff);

// in read_wait.c
int read_wait(socket_t fd, int delay);

#ifdef SYS_UNIX
int sane_socketpair(int domain, int type, int protocol, int result[2]);
#endif

#ifdef	__cplusplus
}
#endif

#endif
