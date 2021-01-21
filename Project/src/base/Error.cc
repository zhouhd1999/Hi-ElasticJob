#include "Error.h"

#include <ostream>

// 使用err_ == -1表示错误码协议定义的0错误
//     err_ == 0 表示正常
#define SPECIAL_ZERO_CODE -1

namespace base
{
    ErrorCode::ErrorCode() : err_(0), service_code_(0) {}

    ErrorCode::ErrorCode(const ErrorCode& ec) : err_(ec.err_), service_code_(ec.service_code_), desc_(ec.desc_) {}

    ErrorCode::ErrorCode(ErrorCode&& ec) : err_(0), service_code_(0) { Swap(ec); }

    ErrorCode::ErrorCode(int err, int service_code /*= 0*/, const std::string& desc /*= std::string()*/)
        : err_(err), service_code_(service_code), desc_(desc) {}

    ErrorCode::ErrorCode(const std::string& err, int service_code /*= 0*/, const std::string& desc /*= std::string()*/)
        : err_(atoi(err.c_str())), service_code_(service_code), desc_(desc)
    {
    }

    ErrorCode& ErrorCode::operator=(const ErrorCode& other)
    {
        this->err_          = other.err_;
        this->service_code_ = other.service_code_;
        this->desc_         = other.desc_;
        return *this;
    }

    ErrorCode& ErrorCode::operator=(ErrorCode&& other)
    {
        Swap(other);
        return *this;
    }

    bool ErrorCode::operator==(const ErrorCode& other) const
    {
        return this->err_ == other.err_ && this->service_code_ == other.service_code_;
    }

    void ErrorCode::Swap(ErrorCode& other)
    {
        std::swap(err_, other.err_);
        std::swap(service_code_, other.service_code_);
        desc_.swap(other.desc_);
    }

    std::string ErrorCode::ToString() const
    {
        if (err_ == 0)
            return std::string("0");

        return std::string(std::to_string(service_code_));
    }

    // static
    ErrorCode ErrorCode::FromString(const std::string& err)
    {
        if (err.empty() || err == "0")
            return ErrorCode(0, 0);

        int service_code = 0;
        int code         = 0;

        return ErrorCode(code, service_code);
    }

    ErrorCode ErrorCode::With(std::string desc) const
    {
        ErrorCode err_code(this->GetErr(), this->GetServiceCode(), desc);

        return std::move(err_code);
    }
}

std::ostream& operator<<(std::ostream& out, const base::ErrorCode& err_code)
{
    out << "ErrorCode(" << err_code.ToString() << ", '" << err_code.GetDesc() << "')";
    return out;
}
