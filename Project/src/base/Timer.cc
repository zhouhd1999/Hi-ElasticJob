#include "Timer.h"
#include "ThreadUtils.h"

namespace base
{
    Timer::~Timer()
    {
	    if (!m_bexit)
	    {
		    stop();
	    }
    }

    void Timer::start(uint32_t imsec, bool immediately_run)
    {
        if (imsec == 0 || imsec == std::numeric_limits<uint32_t>::max())
        {
            return;
        }

        m_imsec = imsec;
        m_immediately_run = immediately_run;
        m_thread = boost::thread(boost::bind(&Timer::run, this));
    }


    void Timer::stop()
    {
        m_bexit = true;
        cond_.notify_all();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void Timer::run()
    {
        std::string thread_name = thread_name_;
        if (thread_name.empty())
            thread_name = "cc-timer";
        SetThreadName(thread_name);

        if(m_immediately_run)
        {
            m_fun();
        }
        while(!m_bexit)
        {
            {
                boost::unique_lock<boost::mutex> lock(mutex_cond_);
                boost::posix_time::milliseconds msFreezingTime = boost::posix_time::milliseconds(m_imsec);
                cond_.timed_wait(lock,msFreezingTime,[this](){return m_bexit; });
            }
            if(m_bexit)
            {
                return;
            }
            m_fun();
        }
    }
}
