#include<gtest/gtest.h>
#include"pyramidCreate.h"

TEST(pyramidCreate, pyramidTest){
    auto pathleft = std::string("/home/cqg/githubProject/stereoMatch/data/L/8_16.bmp");
    auto pathright = std::string("/home/cqg/githubProject/stereoMatch/data/R/8_16.bmp");

    auto leftTemp = cv::imread(pathleft,0);
    auto rightTemp = cv::imread(pathright,0);

    auto pyramid = pyramidCreate(leftTemp, rightTemp, 5);
    pyramid.show();
}