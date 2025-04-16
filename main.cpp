#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_ext.h>
#include <CL/cl_platform.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include "ml.hpp"
using namespace std;

int main()
{
    // xor problem test
    ml::Device dev(true);
    ml::Device::func relu, drelu;
    relu.ker = "out=inp>0?inp:0.01*inp;";
    drelu.ker = "out=inp>0?1:0.01;";
    dev.setFunc("relu", relu, drelu);
    dev.compile();
    return 0;
}