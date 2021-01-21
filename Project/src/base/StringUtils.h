#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <mysql++.h>
#include <iostream>
#include <vector>

#include <boost/format.hpp>

using namespace std;

namespace base
{
    // 将字符串按pattern分割放入vector
    bool String2Array(vector<string>& vStr, string str, const char pattern = ',')
    {
        string::size_type pos;
        str += pattern;
        for(unsigned int i = 0; i < str.size(); i++)
        {
            pos = str.find(pattern, i);
            if(pos < str.size())
            {
                std::string s = str.substr(i, pos-i);
                vStr.push_back(s);
                i = pos;
            }
        }
        return true;
    }

    // 将mysqlpp::String转化为std::string
    string to_str(const mysqlpp::String& in)
    {
        string temp;
        in.to_string(temp);
        return temp;
    }

    // 将mysqlpp::String转化为int
    int to_int(const mysqlpp::String& in)
    {
        string temp;
        in.to_string(temp);
        return atoi(temp.c_str());
    }

    // 在字符串前后加上"'"
    string sqlfmt(string str)
    {
        return "'" + str + "'";
    }

    // 将时间格式转化为字符串并在前后带上"'"
    string sqlfmt(time_t time)
    {
        stringstream ss;
        ss << put_time(localtime(&time),"%F %X");
        return sqlfmt(ss.str());
    }

    // 将vector<string>转化为字符串并在前后带上"'"
    string sqlfmt(vector<string> vStr)
    {
        string str;
        for (unsigned int i = 0; i < vStr.size() - 1; ++i)
        {
            str += vStr[i] + ",";
        }
        str += vStr[vStr.size()-1];
        return sqlfmt(str);
    }

    // 使用boost库格式化字符串
    string strfmt(const char * _Format, const uint16_t data)
    {
        boost::format fmt(_Format);
        fmt % data;
        return fmt.str();
    }
}

#endif
