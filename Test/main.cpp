#include "CppUTest/CommandLineTestRunner.h"

extern "C"
{
#include "Uassert.h"
}

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}

void Uassert(bool condition)
{
    (void)(condition);
}
