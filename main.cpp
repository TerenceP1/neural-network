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
    relu.ker = "out=inp>0.0f?inp:0.01f*inp;";
    drelu.ker = "out=inp>0.0f?1.0f:0.01f;";
    dev.setFunc("relu", relu, drelu);
    ml::Device::func sig, dsig;
    sig.ker = "out=1.0f/(1.0f+exp(-inp));";
    dsig.ker = "float tmp=1.0f/(1.0f+exp(-inp));\n        out=tmp*(1.0f-tmp);";
    dev.setFunc("sig", sig, dsig);
    dev.compile();
    return 0;
}