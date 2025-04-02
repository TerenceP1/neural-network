#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
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
                    if (inc1 >= 0 && inc1 < width)
                    {
                        if (inc2 >= 0 && inc2 < height)
                        {
                            out[a][b] += mtr[i][j] * inp[inc1][inc2];
                        }
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
    for (int i = 0; i < inp.rows; i++)
    {
        vector<cv::Vec3b> tmp(inp.cols);
        for (int j = 0; j < inp.cols; j++)
        {
            tmp = inp.at<cv::Mat3b>(i, j);
        }
        oImg[i] = tmp;
    }
    // greyscale
    vector<vector<double>> grey(inp.rows);
    for (int i = 0; i < inp.rows; i++)
    {
        vector<double> tmp(inp.cols);
        for (int j = 0; j < inp.cols; j++)
        {
            tmp[j] = 0.299 * oImg[i][j][0] + 0.587 * oImg[i][j][1] + 0.114 * oImg[i][j][2];
        }
        grey[i] = tmp;
    }
    auto fres = sobel(grey);
    cv::Mat res(inp);
    for (int i = 0; i < inp.rows; i++)
    {
        for (int j = 0; j < inp.cols; j++)
        {
            inp.at<cv::Mat3b>(i, j)[0] = (uchar)min((int)(fres[i][j]), 255);
            inp.at<cv::Mat3b>(i, j)[1] = (uchar)min((int)(fres[i][j]), 255);
            inp.at<cv::Mat3b>(i, j)[2] = (uchar)min((int)(fres[i][j]), 255);
        }
    }
    cv::imwrite("out.bmp", res);
    return 0;
}