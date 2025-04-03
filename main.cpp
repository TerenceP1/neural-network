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
            tmp[j] = inp[i][j] < 0.5 * mVal ? 0 : inp[i][j];
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

int iWidth, iHeight;
bool isValid(x,y){
    return (x>=0) && (x<iHeight) && (y>=0) && (y<iWidth);
}

void test_fill(vector<vector<double>> inp)
{
    // Uses floodfill to find shading
    // inp is sobel edge and filtered
    int width = inp[0].size(), height = inp.size();
    vector<vector<bool>>visArr(height);
    for (int i=0;i<height;i++)
    {
        vector<bool>tmp(width,false);
        visArr[i]=tmp;
    }
    // fill from corner to detect and remove background
    // then, for each unvisited "pocket" in visArr,
    // use floodfill to find the number of connected reigons 
    // in the pocket. Then, 1=full, 2=empty, more=partial

    // Floodfill from corner:
    queue<pair<int,int>> fQueue; // queue of places to fill
    fQueue.push({0,0});
    
}

int main()
{
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
    int green[3] = {75, 166, 90};
    vector<vector<double>> grey = filter(dst(oImg, green), 0.7);
    cv::Mat greened(inp);
    for (int i = 0; i < greened.rows; i++)
    {
        for (int j = 0; j < greened.cols; j++)
        {
            greened.at<cv::Vec3b>(i, j)[0] = (uchar)min((int)(grey[i][j]), 255);
            greened.at<cv::Vec3b>(i, j)[1] = (uchar)min((int)(grey[i][j]), 255);
            greened.at<cv::Vec3b>(i, j)[2] = (uchar)min((int)(grey[i][j]), 255);
        }
    }
    cv::imwrite("greenified.bmp", greened);
    cout << "colorfied: " << grey[0].size() << 'x' << grey.size() << endl;
    cout << "sobel\n";
    auto fres = sobel(grey);
    fres = filterhalf(fres);
    cv::Mat res(inp);
    cout << "out\n";
    for (int i = 0; i < inp.rows; i++)
    {
        for (int j = 0; j < inp.cols; j++)
        {
            inp.at<cv::Vec3b>(i, j)[0] = (uchar)min((int)(fres[i][j]), 255);
            inp.at<cv::Vec3b>(i, j)[1] = (uchar)min((int)(fres[i][j]), 255);
            inp.at<cv::Vec3b>(i, j)[2] = (uchar)min((int)(fres[i][j]), 255);
        }
    }
    cv::imwrite("out.bmp", res);
    return 0;
}
