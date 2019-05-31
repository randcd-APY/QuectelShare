/*****************************************************************************
 * @copyright
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;
int main(int argc, char *argv[])
{
    unsigned char raw_data[384000];
    unsigned char bmp_data[307200];
    const char * inputRawFile = argc > 1 ? argv[1] : "default.raw";
    FILE *fs;
    fs = fopen(inputRawFile, "rb");

    if (!fs)
    {
        cout << "Could not open the raw file: \"" << inputRawFile << "\"" << endl;
        return -1;
    }
    fread(raw_data,384000,1,fs);
    fclose(fs);
    int k=0;
    for(int i=0; i<76800;i++)
    {
        for(int j=0; j<5;j++)
        {
            bmp_data[k] = raw_data[5*i + j];
            k++;
            if(j == 3)
                break;
        }
    }
    Mat temp(480, 640, CV_8UC1);
    temp=Scalar(0);
    for(int ii=0; ii<temp.rows;ii++)
    {
        for(int jj=0; jj<temp.cols;jj++)
        {
            temp.at<unsigned char>(ii,jj)=bmp_data[ii*temp.cols + jj];
        }
    }
    imwrite( "raw.bmp", temp );

    imshow("Image View_temp", temp);

    char key = (char)waitKey(1000);
    return 0;
}
