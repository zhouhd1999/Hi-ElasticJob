#include <map>
#include <string>
#include <vector>
#include <set>

namespace elasticjob
{
    struct ParallelItem
    {

        using Pair = std::pair<std::string, std::string>;
        std::vector<Pair> limits;
        void Parse(const std::string &limits_str);
    };

    class ParallelStrategy
    {
    public:
        ParallelStrategy(const std::string &tpl);
        ~ParallelStrategy();

        bool LoadLimits(const std::map<std::string, int> &limits);

        bool RecalcuLimit(const ParallelItem &item, const std::string &job_id);
        void Clear();

        bool AddLimitItem(const ParallelItem &item, const std::string &job_id);
        bool RemoveLimitItem(const ParallelItem &item, const std::string &job_id);
        bool RemoteLimitByJobID(const std::string &job_id);

        std::string GetStat();

    private:
        struct Record
        {
            int current = 0;
            int limits = 0;
            std::set<std::string> job_ids;
        };

        bool Islimited(const ParallelItem &item);
        bool Apply(const ParallelItem &item, const std::string &job_id);

    private:
        using LimitMap = std::map<std::string, int>;
        using UsingMap = std::map<ParallelItem::Pair, Record>;
        LimitMap parallel_limits_;
        UsingMap usings_;
    };
} // namespace elasticjob
