#include "CppUTest/CommandLineTestRunner.h"

extern "C"
{
#include "Uassert.h"
}

bool uassertAsserted;

int main(int ac, char** av)
{
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return CommandLineTestRunner::RunAllTests(ac, av);
}

void Uassert(bool condition)
{
    if(!condition)
        uassertAsserted = true;
}
