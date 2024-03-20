#include<gtest/gtest.h>
#include<string>
#include<opencv4/opencv2/opencv.hpp>
#include<iostream>
#include"BlockMatch.h"
//#define DEBUG

TEST(imageTest, maintest){
    auto pathleft = std::string("/home/cqg/githubProject/stereoMatch/data/L/8_16.bmp");
    auto pathright = std::string("/home/cqg/githubProject/stereoMatch/data/R/8_16.bmp");

    auto leftTemp = cv::imread(pathleft,0);
    auto rightTemp = cv::imread(pathright,0);

    cv::Mat left,right;
    cv::resize(leftTemp,left,cv::Size(400,400));
    cv::resize(rightTemp,right,cv::Size(400,400));

    const auto width = left.cols;
    const auto height = left.rows;

    BMOption option{0,200,3,false,1.0f,true,5.0f,true,100,10};
    BlockMatch bm;

    #ifdef DEBUG
        std::cout<<"left \n"<< left <<std::endl;
        std::cout<<"right \n"<< right <<std::endl;
    #endif // DEBUG
    if(bm.Initialize(left,right,option)){
        std::cout<<"Initialize success!"<<std::endl;
    }
    auto disparity = new float[width * height]();
    bm.Match(left,right,disparity);

    #ifdef DEBUG
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //ASSERT_FLOAT_EQ(disparity[i * width + j],3);
            std::cout<< disparity[i * width + j]<<"\t"; 
        }
        std::cout<<std::endl;
    }
    #endif // DEBUG
    
    cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
    float min_disp = width, max_disp = -width;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const float disp = disparity[i * width + j];
            if (disp != Invalid_float) {
                min_disp = std::min(min_disp, disp);
                max_disp = std::max(max_disp, disp);
            }
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const float disp = disparity[i * width + j];
            if (disp == Invalid_float) {
                disp_mat.data[i * width + j] = 0;
            }
            else {
                auto ccc = static_cast<uchar>((disp - min_disp) / (max_disp - min_disp) * 255);
                disp_mat.data[i * width + j] = ccc;
            }
        }
    }
    #ifdef DEBUG
    std::cout<<disp_mat<<std::endl;
    #endif // DEBUG
    //std::cout<<cv::imwrite("disp.png", disp_mat)<<std::endl;
    cv::imshow("视差图", disp_mat);
    cv::Mat disp_color;
    applyColorMap(disp_mat, disp_color, cv::COLORMAP_JET);
    cv::imshow("视差图-伪彩", disp_color);
    cv::waitKey(0);
}