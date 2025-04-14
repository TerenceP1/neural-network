// ML
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_ext.h>
#include <CL/cl_platform.h>
#include <iostream>
#include <time.h>
#include <string>
#include <map>
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
        default:
            log(2, "Function ml::log called with invalid errorlevel");
            break;
        }
    }
    class device
    {
    private:
        bool isGpu;
        cl_platform_id platform;
        cl_device_id device2;

    public:
        device(bool gpu)
        {
            isGpu = gpu;
            if (gpu)
            {
                log(0, "Using GPU");
                clGetPlatformIDs(1, &platform, nullptr);
                clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device2, nullptr);
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
                log(0, "OpenCL GPU name: " + nstr);
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_NAME,
                    NULL,
                    NULL,
                    &len);
                vendor = new char[len];
                clGetDeviceInfo(
                    device,
                    CL_DEVICE_NAME,
                    len,
                    vendor,
                    NULL);
                string nstr2(vendor, len);
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
                log(1, "GPU is preffered, please switch if possible");
            }
        }
        ~device()
        {
            log(0, "Cleaning up resources from ml::device...");
        }
    };
}