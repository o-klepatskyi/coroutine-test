#include "log.h"

void f()
{
    LogError("something");
}

int main(int, char**)
{
    f();
    return 0;
}