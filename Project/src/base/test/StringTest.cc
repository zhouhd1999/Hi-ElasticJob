#include "../StringUtils.h"

#include <iostream>
using namespace std;

int main()
{
    string listen_port = base::strfmt("0.0.0.0:%d", 8080);
    cout << listen_port << endl;
    return 0;
}