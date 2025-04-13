#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <queue>
using namespace std;

vector<vector<double>> conv(vector<vector<double>> inp, double mtr[3][3])
{
    auto out = inp;
    int width = inp[0].size(), height = inp.size();
    for (int a = 0; a < height; a++)
    {
        for (int b = 0; b < width; b++)
        {
            out[a][b] = 0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    int inc1 = a + i - 1, inc2 = b + j - 1;
                    if (inc1 >= 0 && inc1 < height)
                    {
                        if (inc2 >= 0 && inc2 < width)
                        {
                            out[a][b] += mtr[i][j] * inp[inc1][inc2];
                        }
                        else
                        {
                            out[a][b] += 255 * mtr[i][j];
                        }
                    }
                    else
                    {
                        out[a][b] += 255 * mtr[i][j];
                    }
                }
            }
        }
    }
    return out;
}

vector<vector<double>> sobel(vector<vector<double>> inp)
{
    double gr1[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}};
    double gr2[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}};
    auto out = inp;
    int width = inp[0].size(), height = inp.size();
    vector<vector<double>> cvn1 = conv(inp, gr1), cvn2 = conv(inp, gr2);
    vector<vector<double>> res = inp;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            res[i][j] = sqrt((cvn1[i][j]) * (cvn1[i][j]) + (cvn2[i][j]) * (cvn2[i][j]));
        }
    }
    return res;
}

vector<vector<double>> dst(vector<vector<cv::Vec3b>> img, int color[3])
{
    int width = img[0].size(), height = img.size();
    vector<vector<double>> res(height);
    for (int i = 0; i < height; i++)
    {
        vector<double> tmp(width);
        for (int j = 0; j < width; j++)
        {
            tmp[j] = sqrt((color[0] - img[i][j][0]) * (color[0] - img[i][j][0]) + (color[1] - img[i][j][1]) * (color[1] - img[i][j][1]) + (color[2] - img[i][j][2]) * (color[2] - img[i][j][2]));
        }
        res[i] = tmp;
    }
    return res;
}

vector<vector<double>> filterhalf(vector<vector<double>> inp)
{
    int width = inp[0].size(), height = inp.size();
    double mVal = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (inp[i][j] > mVal)
            {
                mVal = inp[i][j];
            }
        }
    }
    auto res = inp;
    for (int i = 0; i < height; i++)
    {
        vector<double> tmp(width);
        for (int j = 0; j < width; j++)
        {
            tmp[j] = inp[i][j] < 0.4 * mVal ? 0 : inp[i][j];
        }
        res[i] = tmp;
    }
    return res;
}

vector<vector<double>> filter(vector<vector<double>> inp, double acc)
{
    int width = inp[0].size(), height = inp.size();
    double mVal = 255;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (inp[i][j] < mVal)
            {
                mVal = inp[i][j];
            }
        }
    }
    auto res = inp;
    for (int i = 0; i < height; i++)
    {
        vector<double> tmp(width);
        for (int j = 0; j < width; j++)
        {
            tmp[j] = 255 - inp[i][j] < acc * (255 - mVal) ? 255 : inp[i][j];
        }
        res[i] = tmp;
    }
    return res;
}

// Hu moments
vector<vector<double>> huImg;
double huArea;
inline double huRaw(int x, int y)
{
    // Raw moment
    int width = huImg[0].size(), height = huImg.size();
    double res = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            res += (double)(pow(i, x) * pow(j, y)) * huImg[i][j];
        }
    }
    return res;
}

int ctX, ctY;
inline void huCent()
{
    // Calculate centroid
    huArea = huRaw(0, 0);
    double xI = huRaw(1, 0), yI = huRaw(0, 1);
    ctX = xI / huArea;
    ctY = yI / huArea;
}

inline double huCtMmt(int x, int y)
{
    // Hu central moment
    int width = huImg[0].size(), height = huImg.size();
    double res = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            res += (double)(pow(i - ctX, x)) * (double)(pow(j - ctY, y)) * huImg[i][j];
        }
    }
    return res;
}

double huTable[4][4]; // Stores normalized central moments
inline void huNormCtmmt(int x, int y)
{
    huTable[x][y] = huCtMmt(x, y) / pow(huArea, (double)(x + y) / 2.0 + 1.0);
}

double huRes[7]; // Output
inline void huMoment(vector<vector<double>> inp)
{
    huImg = inp;
    huCent();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            huNormCtmmt(i, j);
        }
    }
    huRes[0] = huTable[2][0] + huTable[0][2];
    huRes[1] = (huTable[2][0] - huTable[0][2]) * (huTable[2][0] - huTable[0][2]) + 4.0 * huTable[1][1] * huTable[1][1];
    huRes[2] = (huTable[3][0] - 3.0 * huTable[1][2]) * (huTable[3][0] - 3.0 * huTable[1][2]) + (3.0 * huTable[2][1] - huTable[0][3]) * (3.0 * huTable[2][1] - huTable[0][3]);
    huRes[3] = (huTable[3][0] + huTable[1][2]) * (huTable[3][0] + huTable[1][2]) + (huTable[2][1] + huTable[0][3]) * (huTable[2][1] + huTable[0][3]);
    huRes[4] = (huTable[3][0] - 3 * huTable[1][2]) *
                   (huTable[3][0] + huTable[1][2]) *
                   ((huTable[3][0] + huTable[1][2]) * (huTable[3][0] + huTable[1][2]) -
                    3 * (huTable[2][1] + huTable[0][3]) * (huTable[2][1] + huTable[0][3])) +
               (3 * huTable[0][1] - huTable[0][3]) *
                   (huTable[2][1] + huTable[1][3]) *
                   (3 * (huTable[3][0] + huTable[1][2]) * (huTable[3][0] + huTable[1][2]) -
                    (huTable[2][1] + huTable[0][3]));
    huRes[0]=-copysign(1.0,huRes[0])*log10(abs(huRes[0]));
    huRes[1]=-copysign(1.0,huRes[1])*log10(abs(huRes[1]));
    huRes[2]=-copysign(1.0,huRes[2])*log10(abs(huRes[2]));
    huRes[3]=-copysign(1.0,huRes[3])*log10(abs(huRes[3]));
}

int iWidth, iHeight;
bool isValid(int x, int y)
{
    return (x >= 0) && (x < iHeight) && (y >= 0) && (y < iWidth);
}

double huOval[4];
double huRhombus[4];
double huSquiggle[4];

void huFetch(){
    cv::Mat oval=cv::imread("shapes/oval.jpg"),
    rhombus=cv::imread("shapes/rhombus.jpg"),
    squiggle=cv::imread("shapes/squiggle.jpg");
    vector<vector<cv::Vec3b>> cOval(oval.rows);
    for (int i = 0; i < oval.rows; i++)
    {
        vector<cv::Vec3b> tmp(oval.cols);
        for (int j = 0; j < oval.cols; j++)
        {
            tmp[j] = oval.at<cv::Vec3b>(i, j);
        }
        cOval[i] = tmp;
    }
    vector<vector<double>> bOval(oval.rows);
    for (int i = 0; i < oval.rows; i++)
    {
        vector<double> tmp(oval.cols);
        for (int j = 0; j < oval.cols; j++)
        {
            tmp[j] = 255-cOval[i][j][0];
            if (tmp[j]<128){tmp[j]=0;}else{tmp[j]=255;}
        }
        bOval[i] = tmp;
    }
    huMoment(bOval);
    huOval[0]=huRes[0];
    huOval[1]=(huRes[1]);
    huOval[2]=(huRes[2]);
    huOval[3]=huRes[3];
    cout<<"Oval Hu moment: "<<(huRes[0])<<", "<<(huRes[1])<<", "<<(huRes[2])<<", "<<huRes[3]<<endl;
    vector<vector<cv::Vec3b>> cSquiggle(squiggle.rows);
    for (int i = 0; i < squiggle.rows; i++)
    {
        vector<cv::Vec3b> tmp(squiggle.cols);
        for (int j = 0; j < squiggle.cols; j++)
        {
            tmp[j] = squiggle.at<cv::Vec3b>(i, j);
        }
        cSquiggle[i] = tmp;
    }
    vector<vector<double>> bSquiggle(squiggle.rows);
    for (int i = 0; i < squiggle.rows; i++)
    {
        vector<double> tmp(squiggle.cols);
        for (int j = 0; j < squiggle.cols; j++)
        {
            tmp[j] = 255-cSquiggle[i][j][0];
            if (tmp[j]<128){tmp[j]=0;}else{tmp[j]=255;}
        }
        bSquiggle[i] = tmp;
    }
    huMoment(bSquiggle);
    huSquiggle[0]=(huRes[0]);
    huSquiggle[1]=(huRes[1]);
    huSquiggle[2]=(huRes[2]);
    huSquiggle[3]=(huRes[3]);
    cout<<"Squiggle Hu moment: "<<(huRes[0])<<", "<<(huRes[1])<<", "<<(huRes[2])<<", "<<huRes[3]<<endl;
    vector<vector<cv::Vec3b>> cRhombus(rhombus.rows);
    for (int i = 0; i < rhombus.rows; i++)
    {
        vector<cv::Vec3b> tmp(rhombus.cols);
        for (int j = 0; j < rhombus.cols; j++)
        {
            tmp[j] = rhombus.at<cv::Vec3b>(i, j);
        }
        cRhombus[i] = tmp;
    }
    vector<vector<double>> bRhombus(rhombus.rows);
    for (int i = 0; i < rhombus.rows; i++)
    {
        vector<double> tmp(rhombus.cols);
        for (int j = 0; j < rhombus.cols; j++)
        {
            tmp[j] = 255-cRhombus[i][j][0];
            if (tmp[j]<128){tmp[j]=0;}else{tmp[j]=255;}
        }
        bRhombus[i] = tmp;
    }
    huMoment(bRhombus);
    huRhombus[0]=(huRes[0]);
    huRhombus[1]=(huRes[1]);
    huRhombus[2]=(huRes[2]);
    huRhombus[3]=(huRes[3]);
    cout<<"Rhombus Hu moment: "<<(huRes[0])<<", "<<(huRes[1])<<", "<<(huRes[2])<<", "<<huRes[3]<<endl;
    
}

void test_fill(vector<vector<double>> inp, char* file)
{
    // Uses floodfill to find shading
    // inp is sobel edge and filtered
    int width = inp[0].size(), height = inp.size();
    vector<vector<bool>> visArr(height);
    for (int i = 0; i < height; i++)
    {
        vector<bool> tmp(width, false);
        visArr[i] = tmp;
    }
    // fill from corner to detect and remove background
    // then, for each unvisited "pocket" in visArr,
    // use floodfill to find the number of connected reigons
    // in the pocket. Then, 1=full, 2=empty, more=partial

    // Floodfill from corner:
    queue<pair<int, int>> fQueue; // queue of places to fill
    fQueue.push({0, 0});
    iHeight = height, iWidth = width;
    int ind = 0;
    while (!fQueue.empty())
    {
        ind++;
        if (ind % 1000 == 0)
        {
            // cout << "DEBUG THING: queue length is " << fQueue.size() << ", current element to be popped is {" << fQueue.back().first << ", " << fQueue.back().second << "}\n";
        }
        pair<int, int> current = fQueue.front();
        fQueue.pop();
        visArr[current.first][current.second] = true;
        if (isValid(current.first + 1, current.second))
        {
            if (!visArr[current.first + 1][current.second])
            {
                if (inp[current.first + 1][current.second] == 0)
                {
                    fQueue.push({current.first + 1, current.second});

                    visArr[current.first + 1][current.second] = true;
                }
            }
        }
        if (isValid(current.first - 1, current.second))
        {
            if (!visArr[current.first - 1][current.second])
            {
                if (inp[current.first - 1][current.second] == 0)
                {
                    fQueue.push({current.first - 1, current.second});
                    visArr[current.first - 1][current.second] = true;
                }
            }
        }
        if (isValid(current.first, current.second + 1))
        {
            if (!visArr[current.first][current.second + 1])
            {
                if (inp[current.first][current.second + 1] == 0)
                {
                    fQueue.push({current.first, current.second + 1});
                    visArr[current.first][current.second + 1] = true;
                }
            }
        }
        if (isValid(current.first, current.second - 1))
        {
            if (!visArr[current.first][current.second - 1])
            {
                if (inp[current.first][current.second - 1] == 0)
                {
                    fQueue.push({current.first, current.second - 1});

                    visArr[current.first][current.second - 1] = true;
                }
            }
        }
    }
    cv::Mat backg(height, width, CV_8UC(3));
    for (int i = 0; i < backg.rows; i++)
    {
        for (int j = 0; j < backg.cols; j++)
        {
            backg.at<cv::Vec3b>(i, j)[0] = (uchar)min((int)(visArr[i][j] * 255), 255);
            backg.at<cv::Vec3b>(i, j)[1] = (uchar)min((int)(visArr[i][j] * 255), 255);
            backg.at<cv::Vec3b>(i, j)[2] = (uchar)min((int)(visArr[i][j] * 255), 255);
        }
    }
    for (int k = 0; k < height; k++)
    {
        for (int m = 0; m < width; m++)
        {
            if (!visArr[k][m])
            {
                cout << "Thing at (" << k << ',' << m << ")\n";
                // floodfill
                vector<vector<bool>> visArr2(height);
                for (int i = 0; i < height; i++)
                {
                    vector<bool> tmp(width, true);
                    visArr2[i] = tmp;
                }

                // look for shapes

                queue<pair<int, int>> fQueue2; // queue of places to fill
                vector<pair<int, int>> places;
                fQueue2.push({k, m});
                iHeight = height, iWidth = width;
                int ind = 0;
                cv::Vec3b randclr;
                randclr[0] = rand() % 256;
                randclr[1] = rand() % 256;
                int xMin=1000000,xMax=0,yMin=1000000,yMax=0;
                randclr[2] = rand() % 256;
                while (!fQueue2.empty())
                {
                    ind++;
                    if (ind % 1000 == 0)
                    {
                        // cout << "DEBUG THING: queue length is " << fQueue.size() << ", current element to be popped is {" << fQueue.back().first << ", " << fQueue.back().second << "}\n";
                    }
                    pair<int, int> current = fQueue2.front();
                    fQueue2.pop();
                    backg.at<cv::Vec3b>(current.first, current.second) = randclr;
                    if (current.first<xMin) xMin=current.first;
                    if (current.first>xMax) xMax=current.first;
                    if (current.second<yMin) yMin=current.second;
                    if (current.second>yMax) yMax=current.second;
                    visArr[current.first][current.second] = true;
                    visArr2[current.first][current.second] = false;
                    places.push_back(current);
                    if (isValid(current.first + 1, current.second))
                    {
                        if (!visArr[current.first + 1][current.second])
                        {
                            fQueue2.push({current.first + 1, current.second});

                            visArr[current.first + 1][current.second] = true;
                            visArr2[current.first + 1][current.second] = false;
                        }
                    }
                    if (isValid(current.first - 1, current.second))
                    {
                        if (!visArr[current.first - 1][current.second])
                        {
                            fQueue2.push({current.first - 1, current.second});
                            visArr[current.first - 1][current.second] = true;
                            visArr2[current.first - 1][current.second] = false;
                        }
                    }
                    if (isValid(current.first, current.second + 1))
                    {
                        if (!visArr[current.first][current.second + 1])
                        {
                            fQueue2.push({current.first, current.second + 1});
                            visArr[current.first][current.second + 1] = true;
                            visArr2[current.first][current.second + 1] = false;
                        }
                    }
                    if (isValid(current.first, current.second - 1))
                    {
                        if (!visArr[current.first][current.second - 1])
                        {
                            fQueue2.push({current.first, current.second - 1});

                            visArr[current.first][current.second - 1] = true;
                            visArr2[current.first][current.second - 1] = false;
                        }
                    }
                }

                vector<vector<double>> huIn(height);
                for (int i=0;i<height;i++){
                    vector<double>tmp(width);
                    for (int j=0;j<width;j++){
                        tmp[j]=255-255*visArr2[i][j];
                    }
                    huIn[i]=tmp;
                }
                vector<vector<double>> huIn2(xMax-xMin+1);
                for (int i=0;i<xMax-xMin+1;i++){
                    vector<double>tmp(yMax-yMin+1);
                    for (int j=0;j<yMax-yMin+1;j++){
                        tmp[j]=huIn[i+xMin][j+yMin];
                    }
                    huIn2[i]=tmp;
                }
                huMoment(huIn2);
                cout << "Hu moment of shape: "<<huRes[0]<<", "<<huRes[1]<<", "<<huRes[2]<<", "<<huRes[3]<<endl;
                double dOv=sqrt(pow((huRes[0])-huOval[0],2.0)+pow((huRes[1])-huOval[1],2.0)+pow((huRes[2])-huOval[2],2.0)+pow((huRes[3])-huOval[3],2.0));
                double dRh=sqrt(pow((huRes[0])-huRhombus[0],2.0)+pow((huRes[1])-huRhombus[1],2.0)+pow((huRes[2])-huRhombus[2],2.0)+pow((huRes[3])-huRhombus[3],2.0));
                double dSq=sqrt(pow((huRes[0])-huSquiggle[0],2.0)+pow((huRes[1])-huSquiggle[1],2.0)+pow((huRes[2])-huSquiggle[2],2.0)+pow((huRes[3])-huSquiggle[3],2.0));
                int shape=3;
                if (dOv<dRh && dOv<dSq) shape=0;
                if (dRh<dOv && dRh<dSq) shape=1;
                if (dSq<dOv && dSq<dRh) shape=2;

                // look for pockets in pockets
                int npock = 0;
                for (int a = 0; a < height; a++)
                {
                    for (int b = 0; b < width; b++)
                    {
                        if (!visArr2[a][b] && (inp[a][b] == 0))
                        {

                            cout << "Sub-thing at (" << a << ',' << b << ")\n";
                            npock++;
                            queue<pair<int, int>> fQueue3; // queue of places to fill
                            fQueue3.push({a, b});
                            iHeight = height, iWidth = width;
                            int ind = 0;
                            cv::Vec3b subU;
                            int rAm = (rand() % 5) + 2;
                            subU[0] = randclr[0] / rAm;
                            subU[1] = randclr[1] / rAm;
                            subU[2] = randclr[2] / rAm;
                            while (!fQueue3.empty())
                            {
                                ind++;
                                if (ind % 1000 == 0)
                                {
                                    // cout << "DEBUG THING: queue length is " << fQueue.size() << ", current element to be popped is {" << fQueue.back().first << ", " << fQueue.back().second << "}\n";
                                }
                                pair<int, int> current = fQueue3.front();
                                fQueue3.pop();
                                visArr2[current.first][current.second] = true;
                                backg.at<cv::Vec3b>(current.first, current.second) = subU;
                                if (isValid(current.first + 1, current.second))
                                {
                                    if (!visArr2[current.first + 1][current.second])
                                    {
                                        if (inp[current.first + 1][current.second] == 0)
                                        {
                                            fQueue3.push({current.first + 1, current.second});

                                            visArr2[current.first + 1][current.second] = true;
                                        }
                                    }
                                }
                                if (isValid(current.first - 1, current.second))
                                {
                                    if (!visArr2[current.first - 1][current.second])
                                    {
                                        if (inp[current.first - 1][current.second] == 0)
                                        {
                                            fQueue3.push({current.first - 1, current.second});
                                            visArr2[current.first - 1][current.second] = true;
                                        }
                                    }
                                }
                                if (isValid(current.first, current.second + 1))
                                {
                                    if (!visArr2[current.first][current.second + 1])
                                    {
                                        if (inp[current.first][current.second + 1] == 0)
                                        {
                                            fQueue3.push({current.first, current.second + 1});
                                            visArr2[current.first][current.second + 1] = true;
                                        }
                                    }
                                }
                                if (isValid(current.first, current.second - 1))
                                {
                                    if (!visArr2[current.first][current.second - 1])
                                    {
                                        if (inp[current.first][current.second - 1] == 0)
                                        {
                                            fQueue3.push({current.first, current.second - 1});

                                            visArr2[current.first][current.second - 1] = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                cout << "pockets: " << npock << endl;
                cout<<"shape: "<<shape<<endl;
                for (pair<int, int> c : places)
                {
                    if (npock == 1)
                    {
                        backg.at<cv::Vec3b>(c.first, c.second) = {0, 0, 255};
                        // cout << "1 pocket\n";
                    }
                    if (npock == 2)
                    {
                        backg.at<cv::Vec3b>(c.first, c.second) = {0, 255, 0};
                        // cout << "2 pockets\n";
                    }
                    if (npock > 2)
                    {
                        backg.at<cv::Vec3b>(c.first, c.second) = {255, 0, 0};
                        // cout << "more pockets\n";
                    }
                    switch (shape){
                        case 0:
                        backg.at<cv::Vec3b>(c.first, c.second)[0]/=2;
                        backg.at<cv::Vec3b>(c.first, c.second)[1]/=2;
                        backg.at<cv::Vec3b>(c.first, c.second)[2]/=2;
                        break;
                        case 1:
                            //cout<<"Case 1"<<endl;
                        break;
                        case 2:
                        backg.at<cv::Vec3b>(c.first, c.second)[0]=min(255,backg.at<cv::Vec3b>(c.first, c.second)[0]+128);
                        backg.at<cv::Vec3b>(c.first, c.second)[1]=min(255,backg.at<cv::Vec3b>(c.first, c.second)[1]+128);
                        backg.at<cv::Vec3b>(c.first, c.second)[2]=min(255,backg.at<cv::Vec3b>(c.first, c.second)[2]+128);
                        break;
                        default:
                        //cout<<"Oh no... (shape error at line 602)"<<endl;
                        break;
                    }
                }
                backg.at<cv::Vec3b>((int)ctX,(int)ctY)={0,0,0};
            }
        }
    }
    imwrite(file, backg);
}

int main()
{
    rand();
    rand();
    rand();
    rand();
    // rand();
    rand();
    rand();
    rand();
    rand();
    rand();
    cout << "Beginning..." << endl;
    ifstream file("file.txt");
    string flnm;
    getline(file, flnm);
    cv::Mat inp = cv::imread(flnm);
    vector<vector<cv::Vec3b>> oImg(inp.rows);
    cout << "Image is " << inp.rows << 'x' << inp.cols << endl;
    cout << "read\n";
    for (int i = 0; i < inp.rows; i++)
    {
        vector<cv::Vec3b> tmp(inp.cols);
        for (int j = 0; j < inp.cols; j++)
        {
            tmp[j] = inp.at<cv::Vec3b>(i, j);
        }
        oImg[i] = tmp;
    }
    // greyscale
    /*cout << "grey\n";
    vector<vector<double>> grey(inp.rows);
    for (int i = 0; i < inp.rows; i++)
    {
        vector<double> tmp(inp.cols);
        for (int j = 0; j < inp.cols; j++)
        {
            tmp[j] = 0.299 * oImg[i][j][0] + 0.587 * oImg[i][j][1] + 0.114 * oImg[i][j][2];
        }
        grey[i] = tmp;
    }*/
    int red[3]={135,51,234};
    int green[3]={108,214,121};
    int purple[3] = {218, 160, 162};
    huFetch();
    vector<vector<double>> greyR = filter(dst(oImg, red), 0.7);
    vector<vector<double>> greyG = filter(dst(oImg, green), 0.7);
    vector<vector<double>> greyP = filter(dst(oImg, purple), 0.7);
    cv::Mat greened(inp);
    for (int i = 0; i < greened.rows; i++)
    {
        for (int j = 0; j < greened.cols; j++)
        {
            greened.at<cv::Vec3b>(i, j)[0] = (uchar)min((int)(greyG[i][j]), 255);
            greened.at<cv::Vec3b>(i, j)[1] = (uchar)min((int)(greyG[i][j]), 255);
            greened.at<cv::Vec3b>(i, j)[2] = (uchar)min((int)(greyG[i][j]), 255);
        }
    }
    cv::imwrite("greenified.bmp", greened);
    cout << "colorfied: " << greyG[0].size() << 'x' << greyG.size() << endl;
    cout << "sobel\n";
    auto fresR = sobel(greyR);
    auto fresG = sobel(greyG);
    auto fresP = sobel(greyP);
    fresR = filterhalf(fresR);
    fresG = filterhalf(fresG);
    fresP = filterhalf(fresP);
    test_fill(fresR,"red.bmp");
    test_fill(fresG,"green.bmp");
    test_fill(fresP,"purple.bmp");
    cv::Mat res(inp);
    cout << "out\n";
    for (int i = 0; i < inp.rows; i++)
    {
        for (int j = 0; j < inp.cols; j++)
        {
            inp.at<cv::Vec3b>(i, j)[0] = (uchar)min((int)(fresG[i][j]), 255);
            inp.at<cv::Vec3b>(i, j)[1] = (uchar)min((int)(fresG[i][j]), 255);
            inp.at<cv::Vec3b>(i, j)[2] = (uchar)min((int)(fresG[i][j]), 255);
        }
    }
    cv::imwrite("out.bmp", res);
    return 0;
}
