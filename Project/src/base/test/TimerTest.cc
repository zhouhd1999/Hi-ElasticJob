
#include "../Timer.h"

#include <iostream>
#include <memory>
#include <functional>

using namespace base;
using namespace std;

void callBack()
{
    cout << "this is timer test" << endl;
}

int main()
{
    unique_ptr<Timer> timer = unique_ptr<Timer>(new Timer(callBack, "test"));
    timer->start(1000);
    while (1);
    return 0;
}