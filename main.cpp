#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_ext.h>
#include <CL/cl_platform.h>
#include <iostream>
#include <vector>
#include "ml.hpp"
using namespace std;

int main()
{
    // xor problem test
    cout << "Hello?\n";
    ml::testing();
    ml::log(0, "Testing123...");
    ml::log(1, "Testing123...");
    ml::log(2, "Testing123...");
    ml::log(3, "Testing123...");
    return 0;
}