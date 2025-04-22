// ML
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_ext.h>
#include <CL/cl_platform.h>
#include <iostream>
#include <time.h>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;
namespace ml
{
    void testing()
    {
        cout << "testing123 ooooh" << endl;
    }
    void log(int errorlevel, string inp)
    {
        time_t cTime = time(NULL);
        string ctm = asctime(localtime(&cTime));
        ctm = ctm.substr(0, ctm.length() - 1);
        switch (errorlevel)
        {
        case 0:
            cout << "[INFO " << ctm << "] " << inp << endl;
            break;
        case 1:
            cout << "[WARNING " << ctm << "] " << inp << endl;
            break;
        case 2:
            cout << "[ERROR " << ctm << "] " << inp << endl;
            break;
        case 3:
            cout << "[DEBUG " << ctm << "] " << inp << endl;
            break;
        default:
            log(2, "Function ml::log called with invalid errorlevel");
            break;
        }
    }
    string slurp(string nm)
    {
        ifstream in(nm);
        stringstream a;
        a << in.rdbuf();
        return a.str();
    }
    class Matrix;
    class Device
    {
    public:
        struct func
        {
            string ker;
            float (*cpuF)(float in);
        };
    friend class Matrix;
    private:
        bool isGpu;
        cl_platform_id platform;
        cl_device_id device;
        map<string, pair<func, func>> activations; // {activation function name, {code to activation function,code to derivative of activation function}}
        bool compiled = false;
        cl_context context;
        cl_program program;
        cl_command_queue queue2;

    public:
        
        Device(bool gpu)
        {
            isGpu = gpu;
            if (gpu)
            {
                log(0, "Using GPU");
                clGetPlatformIDs(1, &platform, nullptr);
                clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
                char *name; // CL_DEVICE_NAME
                size_t len;
                char *vendor; // CL_DEVICE_VENDOR

                clGetDeviceInfo(
                    device,
                    CL_DEVICE_NAME,
                    NULL,
                    NULL,
                    &len);
                name = new char[len];
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_NAME,
                    len,
                    name,
                    NULL);
                string nstr(name, len);
                delete[] name;
                log(0, "OpenCL GPU name: " + nstr);
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_VENDOR,
                    NULL,
                    NULL,
                    &len);
                vendor = new char[len];
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_VENDOR,
                    len,
                    vendor,
                    NULL);
                string nstr2(vendor, len);
                delete[] vendor;
                log(0, "OpenCL GPU vendor: " + nstr2);
                char *version;
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_VERSION,
                    NULL,
                    NULL,
                    &len);
                version = new char[len];
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_VERSION,
                    len,
                    version,
                    NULL);
                string nstr3(version, len);
                delete[] version;
                log(0, "OpenCL supported version: " + nstr3);
                cl_uint max_dims;
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                    sizeof(cl_uint),
                    &max_dims,
                    NULL);
                log(0, "Finished initializing GPU");
            }
            else
            {
                log(0, "No GPU was asked");
                log(1, "GPU is prefered, please switch if possible");
            }
        }
        ~Device()
        {
            log(0, "Cleaning up resources from ml::device...");
            if (compiled)
            {
                log(0, "Releasing program, queue, and context...");
                clReleaseProgram(program);
                clReleaseCommandQueue(queue2);
                clReleaseContext(context);
            }
        }
        void setFunc(string name, func fc, func dFc)
        {
            if (compiled)
            {
                log(2, "Device::setFunc called after compilation");
                log(2, "Terminating...");
                exit(1);
            }
            if (activations.find(name) == activations.end())
            {
                activations.insert({name, {fc, dFc}});
            }
            else
            {
                activations[name] = {fc, dFc};
            }
        }
        void compile()
        {
            if (!isGpu)
            {
                log(2, "Device::compile called with CPU mode set");
                exit(1);
            }
            if (compiled)
            {
                log(2, "Device::compile called twice");
                exit(1);
            }
            log(0, "Generating code...");
            string code=slurp("kernel.cl");
            for (pair<string, pair<func, func>> i : activations)
            {
                // Paremeter der is derivative or not
                code += "__kernel void " + i.first + "(__global float *a, __global float *b,int der)\n{\n    // This function is auto generated\n    int ind=get_global_id(0);\n    float inp=a[ind];\n    float out;\n    if(der){\n        " + i.second.second.ker + "\n    }\n    else\n    {\n        " + i.second.first.ker + "\n    }\n    b[ind]=out;\n}\n\n";
            }
            log(0, "Code:");
            log(0, code);
            log(0, "Compiling code...");
            context = clCreateContext(
                NULL,
                1,
                &device,
                NULL,
                NULL,
                NULL);
            queue2 = clCreateCommandQueueWithProperties(
                context,
                device,
                NULL,
                NULL);
            const char *prg = code.c_str();
            const size_t len2 = code.length();
            program = clCreateProgramWithSource(
                context,
                1,
                &prg,
                &len2,
                NULL);
            log(3, "Program made...");
            cl_int err = clBuildProgram(
                program,
                1,
                &device,
                NULL,
                NULL,
                NULL);
            log(0, "Compiled");
            string errC = to_string(err);
            log(0, "Compilation result:");
            size_t logL;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logL);
            char *blog = new char[logL];
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logL, blog, NULL);
            log(0, blog);
            if (err != CL_SUCCESS)
            {
                log(2, "Compilation failed with error code " + errC);
                exit(err);
            }
            else
            {
                log(0, "Compilation success");
            }
            delete[] blog;
            compiled = true;
        }
    };
    class Matrix
    {
    public:
        typedef Device::func func;
    private:
        int rows, cols;
        cl_mem clBuf;
        bool clLoad; // true if clBuf is valid
        bool isGpu;
        float *buf;
        bool cpuLoad; // true if buf is valid
        cl_platform_id platform;
        cl_device_id device;
        map<string, pair<func, func>> activations; // {activation function name, {code to activation function,code to derivative of activation function}}
        cl_context context;
        cl_program program;
        cl_command_queue queue2;
    public:
        Matrix(){}

        Matrix(Device& dv,int r, int c, bool gpuLoad)
        {
            isGpu=dv.isGpu;
            rows=r;
            cols=c;
            if (dv.isGpu)
            {
                if (!dv.compiled)
                {
                    dv.compile();
                }
                platform=dv.platform;
                device=dv.device;
                context=dv.context;
                program=dv.program;
                queue2=dv.queue2;
                activations=dv.activations;
            }
            else
            {
                activations=dv.activations;
                if (gpuLoad)
                {
                    log(2,"Matrix can't GPU load when in CPU mode");
                    exit(1);
                }
            }
            if (gpuLoad)
            {
                clLoad=true;
                cpuLoad=false;
                clBuf=clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(float)*r*c,NULL,NULL);
            }
            else
            {
                clLoad=false;
                cpuLoad=true;
                buf=new float[r*c];
            }
        }

        ~Matrix()
        {
            if (clLoad) clReleaseMemObject(clBuf);
            if (cpuLoad) delete[] buf;
        }

        void operator=(Matrix& mtr)
        {
            rows=mtr.rows;
            cols=mtr.cols;
        }
    };
}
