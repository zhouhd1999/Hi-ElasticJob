#include "GuidUtil.h"

#include <algorithm>
#include <string>

using namespace std;

namespace base
{
    string CreateGuid()
    {
        // 将GUID转换成string
	    char szGuid[64] = { 0 }; 

        uuid_t struGuid;
        uuid_generate(reinterpret_cast<unsigned char *>(&struGuid));
        uuid_unparse(struGuid, szGuid);
        string str_tmp = szGuid;
        str_tmp.erase(remove(str_tmp.begin(),str_tmp.end(),'-'),str_tmp.end()); //去除原生uuid中的'-'
        return str_tmp;
    }
}