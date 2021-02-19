#include "ParallelStrategy.h"

#include "../../base/StringUtils.h"
#include "muduo/base/Logging.h"

#include <sstream>

using namespace base;

namespace elasticjob
{
    /**
         * @brief 解析并行策略的限制
         *
         * @param strategy
         * @return std::map<std::string, int>
         */
    std::map<std::string, int> ParseParallelStrategyLimit(const std::string &limits_str)
    {
        std::map<std::string, int> parsed;
        std::vector<std::string> limits;
        base::String2Array(limits, limits_str);

        std::vector<std::string> kv;
        for (auto &item : limits)
        {
            kv.clear();
            base::String2Array(kv, item, '=');
            if (kv.size() != 2 || kv[0].empty() || kv[1].empty())
            {
                LOG_ERROR << item << " is invalidate limit value";
                continue;
            }

            int num = atoi(kv[1].c_str());
            if (num <= 0)
            {
                LOG_ERROR << item << " is invalidate limit value, value must greater than 0";
                continue;
            }

            parsed.insert(std::make_pair(kv[0], num));
        }

        return parsed;
    }

    std::string StripString(const std::string &str)
    {
        size_t spos = str.find_first_not_of(' ');
        if (spos == std::string::npos)
            return std::string();
        size_t epos = str.find_last_not_of(' ');
        if (epos == std::string::npos)
            return std::string();

        return str.substr(spos, epos - spos + 1);
    }

    void ParallelItem::Parse(const std::string &limits_str)
    {
        limits.clear();

        std::vector<std::string> parsed;
        base::String2Array(parsed, limits_str);

        for (auto &item : parsed)
        {
            std::vector<std::string> temp;
            base::String2Array(temp, item, ':');
            if (temp.size() != 2)
            {
                LOG_ERROR << item << " is invaldate";
                continue;
            }

            limits.push_back(std::make_pair(StripString(temp[0]), StripString(temp[1])));
        }
    }

    ParallelStrategy::ParallelStrategy(const std::string &limits)
    {
        parallel_limits_ = ParseParallelStrategyLimit(limits);
    }

    ParallelStrategy::~ParallelStrategy() {}

    bool ParallelStrategy::RecalcuLimit(const ParallelItem &item, const std::string &job_id)
    {
        return Apply(item, job_id);
    }

    bool ParallelStrategy::LoadLimits(const std::map<std::string, int> &limits)
    {
        for (const auto &limit : limits)
        {
            parallel_limits_[limit.first] = limit.second;
        }

        return true;
    }

    void ParallelStrategy::Clear()
    {
        usings_.clear();
    }

    bool ParallelStrategy::AddLimitItem(const ParallelItem &item, const std::string &job_id)
    {
        if (Islimited(item))
        {
            return false;
        }

        return Apply(item, job_id);
    }

    bool ParallelStrategy::RemoveLimitItem(const ParallelItem &item, const std::string &job_id)
    {
        for (auto &i : item.limits)
        {
            auto iter2 = usings_.find(i);
            if (iter2 == usings_.end())
            {
                continue;
            }
            else
            {
                LOG_INFO << job_id << " remove limit: " << i.first << ":" << i.second;

                iter2->second.current -= 1;
                auto job_iter = iter2->second.job_ids.find(job_id);
                assert(job_iter != iter2->second.job_ids.end());
                if (job_iter != iter2->second.job_ids.end())
                    iter2->second.job_ids.erase(job_iter);

                assert(iter2->second.current >= 0);
                if (iter2->second.current <= 0)
                {
                    usings_.erase(iter2);
                }
            }
        }

        return true;
    }

    bool ParallelStrategy::RemoteLimitByJobID(const std::string &job_id)
    {
        for (auto iter = usings_.begin(); iter != usings_.end();)
        {
            auto job_id_iter = iter->second.job_ids.find(job_id);
            if (job_id_iter != iter->second.job_ids.end())
            {
                LOG_INFO << job_id << " remove limit: " << iter->first.first << ":" << iter->first.second;

                iter->second.job_ids.erase(job_id_iter);
                iter->second.current--;
                if (iter->second.current <= 0)
                {
                    iter == usings_.erase(iter);
                }
                else
                {
                    iter++;
                }
            }
            else
            {
                iter++;
            }
        }

        return true;
    }

    bool ParallelStrategy::Islimited(const ParallelItem &item)
    {
        for (auto &i : item.limits)
        {
            auto iter = parallel_limits_.find(i.first);
            // there is no limit for this;
            if (iter == parallel_limits_.end())
                continue;

            auto iter2 = usings_.find(i);
            if (iter2 == usings_.end())
            {
                continue;
            }
            else
            {
                if (iter2->second.current + 1 > iter2->second.limits)
                    return true;
            }
        }

        return false;
    }

    bool ParallelStrategy::Apply(const ParallelItem &item, const std::string &job_id)
    {
        for (auto &i : item.limits)
        {
            auto iter = parallel_limits_.find(i.first);
            // there is no limit for this;
            if (iter == parallel_limits_.end())
                continue;

            LOG_INFO << job_id << " add limit: " << i.first << ":" << i.second;

            auto iter2 = usings_.find(i);
            if (iter2 == usings_.end())
            {
                Record r;
                r.current = 1;
                r.limits = iter->second;
                r.job_ids.insert(job_id);
                usings_.insert(std::make_pair(i, r));
            }
            else
            {
                iter2->second.current += 1;
                assert(iter2->second.job_ids.find(job_id) == iter2->second.job_ids.end());
                iter2->second.job_ids.insert(job_id);
            }
        }

        return true;
    }

    std::string ParallelStrategy::GetStat()
    {
        std::ostringstream os;
        os << "Limits: ";
        for (auto &item : parallel_limits_)
        {
            os << item.first << "(" << item.second << "), ";
        }
        os << ", Using:";
        for (auto &us : usings_)
        {
            os << us.first.first << ":" << us.first.second << "(max: " << us.second.limits << ", now:" << us.second.current << "),";
        }
        return os.str();
    }

} // namespace elasticjob
