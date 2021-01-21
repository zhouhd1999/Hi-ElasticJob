#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>

namespace base
{
    class ErrorCode
    {
    public:
        ErrorCode();
        ErrorCode(const ErrorCode& ec);
        ErrorCode(ErrorCode&& ec);
        explicit ErrorCode(int err, int service_code = 0, const std::string& desc = std::string());
        explicit ErrorCode(const std::string& err, int service_code = 0, const std::string& desc = std::string());

        ErrorCode& operator=(ErrorCode&& other);
        ErrorCode& operator=(const ErrorCode& other);

        static ErrorCode FromString(const std::string& err);

        int GetErr() const { return err_; }
        int GetServiceCode() const { return service_code_; }

        std::string GetDesc() const { return desc_; }
        void SetDesc(std::string desc) { desc_ = desc; }

        ErrorCode With(std::string err) const;

        bool IsOk() const { return err_ == 0; }
        bool IsFailed() const { return err_ != 0; }

        std::string ToString() const;

        bool operator==(const ErrorCode& other) const;
        bool operator!=(const ErrorCode& other) const { return !(*this == other); }

        // Return true if failed.
        explicit operator bool() const { return IsFailed(); }

        void Swap(ErrorCode& other);

    private:
        int err_;               // 0表示正常，其他表示异常
        int service_code_;      // 错误码
        std::string desc_;      // 错误描述信息
    };
}

std::ostream& operator<<(std::ostream& out, const base::ErrorCode& err_code);

#endif
