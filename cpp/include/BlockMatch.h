/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement if block match
*/
#ifndef BLOCKMATCH_H_
#define BLOCKMATCH_H_

#ifndef SAFE_DELETE
#define SAFE_DELETE(P) {if(P) delete[](P);P = nullptr;}
#endif // !SAFE_DELETE
#include<vector>
#include<limits>
#include"bm_util.h"
#include<opencv4/opencv2/opencv.hpp>
#include<gtest/gtest.h>

constexpr auto Invalid_float = std::numeric_limits<float>::infinity();

/**
     * \brief BM parameter struct
    */
   struct BMOption
   {
    int min_disparity;   //minimum disparity
    int max_disparity;   //maximum disparity
    uint8_t window_size;      //Block window size

    bool is_check_unique;   //whether to check uniqueness
    float uniqueness_thres;   //The uniqueness constraint threshold (minimum cost - second minimum cost) / minimum cost > threshold indicates valid pixels.

    bool is_check_lr;       //whether to chenck uniqueness of right and left
    float lrcheck_thres;  //The uniqueness constraint threshold of right and left

    bool is_remove_speckles;    //whetner to remove little connected region
    int min_speckle_area;       //the minimum size
   };

/**
 * \brief BlockMatch类
*/
class BlockMatch
{
friend ::testing::Test;
private:
    /** \brief cost calculte*/
    void ComputeCost() const;

     /** \brief ComputeDisparity	 */
    void ComputeDisparity() const;

    /** \brief check uniqueness of left and right*/
    void LRCheck();

    /** \brief Memory release*/
    void Release();

    /** \brief Effective values ​​of images(逐像素统计有效点,并将之放在对应的const vector之中)*/
    void source_process_effective();

    // \brief 计算SAD
    uint32_t caculateSAD(const cv::Mat&, const cv::Mat&) const;

    void computingDispPerPixel(const std::vector<std::pair<uint16_t,uint16_t>>&, const uint32_t*, float*) const; 
public:
    BlockMatch():width_{0},height_{0},left_cost_r_{nullptr},disp_left_{nullptr},
                disp_right_{nullptr},is_initialized_{false}{};
    ~BlockMatch(){
        Release();
        is_initialized_ = false;
    };

    //\brief The initialization of the class
    bool Initialize(const cv::Mat& left, const cv::Mat& right,const BMOption& option);

   
    //\brief running stereo matching
    bool Match(cv::Mat&, cv::Mat&, float* disp_left);

    // \brief reset parameter
    bool Reset(const cv::Mat& left, const cv::Mat& right,const BMOption& option);

private:
    /** \brief BM paramaters*/
    BMOption option_;

    uint16_t width_;
    uint16_t height_;

    /** \brief left image*/
    cv::Mat img_left_;

    /** \brief right image*/
    cv::Mat img_right_;

    /** \brief left image mask pointer*/
    std::vector<std::pair<uint16_t,uint16_t>> img_left_mask_;

    /** \brief right image mask pointer*/
    std::vector<std::pair<uint16_t,uint16_t>> img_right_mask_;

    /** \brief left image cost */
    uint32_t* left_cost_r_;
    uint32_t* right_cost_l_;

    /** \brief left image disparity*/
    float* disp_left_;

    /** \brief right image disparity*/
    float* disp_right_;

    /** \brief Flag of init*/
    bool is_initialized_;

    /** \brief Occluded area pixel vector*/
    std::vector<std::pair<int,int>> occlusions_;

    /** \brief Mismatched area pixel vector*/
    std::vector<std::pair<int,int>> mismatches_;
};
#endif // !BLOCKMATCH_H_