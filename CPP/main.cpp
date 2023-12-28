/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: main
*/
#include<opencv2/opencv.hpp>
#include<iostream>
#include<chrono>
using namespace std::chrono;

#include"BlockMatch.h"

/** 
 * \brief
 * \param argv 3
 * \param argc argc[1]:左影像路径 argc[2]: 右影像路径 argc[3]: 最小视差[可选，默认0] argc[4]: 最大视差[可选，默认64]
 * \param eg. ..\Data\cone\im2.png ..\Data\cone\im6.png 0 64
 * \param eg. ..\Data\Reindeer\view1.png ..\Data\Reindeer\view5.png 0 128
 * \return
 */
int main(int argv, char **argc)
{
    if(argv < 3){
        std::cout<<"参数过少，请指定左右图像的路径!"<<std::endl;
        return -1;
    }
    //=============image read=============
    std::string path_left = argc[1];
    std::string path_right = argc[2];

    cv::Mat src_img_left = cv::imread(path_left,cv::IMREAD_GRAYSCALE);
    cv::Mat src_img_right = cv::imread(path_right,cv::IMREAD_GRAYSCALE);
    cv::Mat img_left,img_right;
    cv::resize(src_img_left,img_left,cv::Size(1024,896));
    cv::resize(src_img_right,img_right,cv::Size(1024,896));

    if(img_left.data == nullptr || img_right.data == nullptr){
        std::cout<<"读入图像失败!"<<std::endl;
        return -1;
    }
    //==================================
    const sint32 width = static_cast<uint32>(img_left.cols);
    const sint32 height = static_cast<uint32>(img_right.rows);

    //左右图像的灰度数据
    auto bytes_left = new uint8[width * height];
    auto bytes_right = new uint8[width * height];
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            bytes_left[i * width + j] = img_left.at<uint8>(i,j);
            bytes_right[i * width + j] = img_right.at<uint8>(i,j);
        }
    }

    printf("Loading Views ... Done!\n");

    //BM匹配参数设计
    BMOption bm_option;
    //候选视差范围
    bm_option.min_disparity = argv < 4 ? 0 :atoi(argc[3]);
    bm_option.max_disparity = argv < 5 ? 64 : atoi(argc[4]);
    //框大小设置
    bm_option.window_size = 3;
    //一致性检查
    bm_option.is_check_lr = false;
    bm_option.lrcheck_thres = 1.0f;

    //去除最小连通区域
    bm_option.is_remove_speckles = false;
    bm_option.min_speckle_area = 20;

    printf("w = %d, h = %d, d = [%d,%d]\n\n", width, height, bm_option.min_disparity, bm_option.max_disparity);

    BlockMatch bm;
    //=====================================================
    //初始化
    printf("BM Initializing... \n");
    auto start = std::chrono::steady_clock::now();
    if (!bm.Initialize(width, height, bm_option)) {
        std::cout << "BM初始化失败！" << std::endl;
        return -2;
    }
    auto end = std::chrono::steady_clock::now();
    auto tt = duration_cast<std::chrono::milliseconds>(end - start);
    printf("BM Initializing Done! Timing : %lf s\n\n", tt.count() / 1000.0);

    //=====================================================
    //匹配
    printf("BM Matching...\n");
    start = std::chrono::steady_clock::now();
    // disparity数组保存子像素的视差结果
    auto disparity = new float32[width * height]();
    if (!bm.Match(bytes_left, bytes_right, disparity)) {
        std::cout << "BM匹配失败！" << std::endl;
        return -2;
    }
    end = std::chrono::steady_clock::now();
    tt = duration_cast<std::chrono::milliseconds>(end - start);
    printf("\nBM Matching...Done! Timing :   %lf s\n", tt.count() / 1000.0);

    //···············································································//
	// 显示视差图
    // 注意，计算点云不能用disp_mat的数据，它是用来显示和保存结果用的。计算点云要用上面的disparity数组里的数据，是子像素浮点数
    cv::Mat disp_mat = cv::Mat(height, width, CV_8UC1);
    float min_disp = width, max_disp = -width;
    for (sint32 i = 0; i < height; i++) {
        for (sint32 j = 0; j < width; j++) {
            const float32 disp = disparity[i * width + j];
            if (disp != Invalid_float) {
                min_disp = std::min(min_disp, disp);
                max_disp = std::max(max_disp, disp);
            }
        }
    }

    for (sint32 i = 0; i < height; i++) {
        for (sint32 j = 0; j < width; j++) {
            const float32 disp = disparity[i * width + j];
            if (disp == Invalid_float) {
                disp_mat.data[i * width + j] = 0;
            }
            else {
                auto ccc = static_cast<uchar>((disp - min_disp) / (max_disp - min_disp) * 255);
                disp_mat.data[i * width + j] = ccc;
            }
        }
    }

    cv::imshow("视差图", disp_mat);
    cv::Mat disp_color;
    applyColorMap(disp_mat, disp_color, cv::COLORMAP_JET);
    cv::imshow("视差图-伪彩", disp_color);
    cv::waitKey(0);

    //···············································································//
    // 释放内存
    delete[] disparity;
    disparity = nullptr;

    system("pause");
    return 0;
}