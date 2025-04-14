// ML
#include <iostream>
#include <time.h>
#include <string>
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
}