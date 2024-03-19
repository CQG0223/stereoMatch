#include<gtest/gtest.h>
#include<string>
#include<opencv4/opencv2/opencv.hpp>
#include<iostream>
#include"BlockMatch.h"

struct blockSingleMatchTest : public testing::Test
{
    const int height = 10;
    const int width = 10;
    cv::Mat left = cv::Mat(height,width,CV_8UC1,cv::Scalar(0));
    cv::Mat right = cv::Mat(height,width,CV_8UC1,cv::Scalar(0));;
    void initializeLeftMatrix(){
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                left.at<uchar>(i,j) = i * height + j;
            }
        }
        right = left.clone();
    }

    void shiftMatLeft(cv::Mat& mat, int n) {
    int cols = mat.cols;
    int shift = n % cols;  // 处理超出列数的情况

    if (shift == 0) {
        return;  // 不需要移动
    }

    cv::Mat leftPart = mat.colRange(shift, cols).clone();
    cv::Mat rightPart = mat.colRange(0, shift).clone();

    leftPart.copyTo(mat.colRange(0, cols - shift));
    rightPart.copyTo(mat.colRange(cols - shift, cols));
}
};

TEST_F(blockSingleMatchTest, initAndShowMat){
    initializeLeftMatrix();
    EXPECT_EQ(1,1);
}

TEST_F(blockSingleMatchTest, initBlockMatch){
    initializeLeftMatrix();
    BMOption option{0,10,3,false,1.0f,false,1.0f,false,20};
    BlockMatch bm;
    ASSERT_TRUE(bm.Initialize(left,right,option));
}

TEST_F(blockSingleMatchTest, testComputeCost){
    initializeLeftMatrix();
    BMOption option{0,10,3,false,1.0f,false,1.0f,false,20};
    BlockMatch bm;
    shiftMatLeft(right,3);
    if(bm.Initialize(left,right,option)){
        std::cout<<"Initialize success!"<<std::endl;
    }
    std::cout<<"left \n"<< left <<std::endl;
    std::cout<<"right \n"<< right <<std::endl;
    auto disparity = new float[width * height]();
    bm.Match(left,right,disparity);

    //std::cout<<"disparity \n"<< disparity <<std::endl;
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //ASSERT_FLOAT_EQ(disparity[i * width + j],3);
            std::cout<< disparity[i * width + j]<<"\t";
        }
        std::cout<<std::endl;
    }
}