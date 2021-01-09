#include <acl-lib/acl/lib_acl.h>//acl c 库
#include <acl-lib/acl_cpp/lib_acl.hpp>// acl c++ 库

int main() {
      acl::string addr = "10.110.120.23";
      printf("addr is %s", addr.c_str());
      return 0;
}
