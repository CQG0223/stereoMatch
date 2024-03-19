/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement for block match
*/
#include<chrono>
#include<cstdio>
#include<cstring>
#include"BlockMatch.h"
//#define DEBUG

bool BlockMatch::Initialize(const cv::Mat& left, const cv::Mat& right,const BMOption& option){
    if(left.data == nullptr || right.data == nullptr) return false;
    width_ = left.cols;
    height_ = right.rows;
    option_ = option;
    auto winsize = option_.window_size;
    img_left_ = left.clone();
    img_right_ = right.clone();

    source_process_effective();
    const int disp_range = option_.max_disparity - option_.min_disparity;

    //匹配代价初始化(左视图向右代价空间初始化)
    const int size = width_ * height_ * disp_range;
    left_cost_r_ = new uint32_t[size]();
    std::fill(left_cost_r_, left_cost_r_+size, UINT32_MAX);

    if(option_.is_check_lr){
        right_cost_l_ = new uint32_t[size]();
        std::fill(right_cost_l_, right_cost_l_+size, UINT32_MAX);
    }

    //视差图初始化
    const auto image_size = width_ * height_;
    disp_left_ = new float[image_size]();
    disp_right_ = new float[image_size]();
    std::fill(disp_left_,disp_left_+image_size,Invalid_float);
    std::fill(disp_right_,disp_right_+image_size,Invalid_float);

    is_initialized_ = left_cost_r_ && disp_left_ && disp_right_;
    return is_initialized_;
}

void BlockMatch::Release(){
    //释放内存
    SAFE_DELETE(left_cost_r_);
    SAFE_DELETE(disp_left_);
    SAFE_DELETE(disp_right_);
}

void BlockMatch::source_process_effective(){
    //逐像素统计有效点,并将之放在对应的const 数组之中
    for (auto i = 0; i < height_; i++)
    {
        for (auto j = 0; j < width_; j++)
        {
            auto gray_left = img_left_.at<u_char>(i,j);
            if(gray_left != 0){
                img_left_mask_.push_back(std::pair<uint16_t,uint16_t>(i,j));
            }
            auto gray_right = img_right_.at<u_char>(i,j);
            if(gray_right != 0){
                img_right_mask_.push_back(std::pair<uint16_t,uint16_t>(i,j));
            }
        }
    }
}

bool BlockMatch::Match(cv::Mat& left, cv::Mat& right, float* disp_left){
    
    auto substart = std::chrono::steady_clock::now();
    //代价计算
    ComputeCost();

    auto subend = std::chrono::steady_clock::now();
    auto tt = std::chrono::duration_cast<std::chrono::milliseconds>(subend - substart);
    printf("computing cost! timing :	%lf s\n", tt.count() / 1000.0);
    substart = std::chrono::steady_clock::now();

    //视差计算
    ComputeDisparity();
    subend = std::chrono::steady_clock::now();
    tt = std::chrono::duration_cast<std::chrono::milliseconds>(subend - substart);
    printf("computing disparities! timing :	%lf s\n", tt.count() / 1000.0);

    //输出视差图
    std::memcpy(disp_left,disp_left_,height_ * width_ * sizeof(float));
    //std::memcpy(disp_left,disp_right_,height_ * width_ * sizeof(float));
    return true;
}

void BlockMatch::ComputeCost() const{
    const int& min_disparity = option_.min_disparity;
    const int& max_disparity = option_.max_disparity;
    const int disp_range = max_disparity - min_disparity;
    const int& win_size = option_.window_size;

    auto half = (win_size - 1)/2;
    //逐像素计算匹配代价
    for(auto val = 0; val < img_left_mask_.size(); val++){
        auto i = img_left_mask_[val].first;
        auto j = img_left_mask_[val].second;

        if(j - half < 0 || i - half < 0 || j + half >= width_ || i + half >= height_) 
            continue;
        cv::Mat subMatLeft = img_left_(cv::Rect(j - half, i - half, win_size, win_size));

        for (auto d = min_disparity; d < max_disparity; d++)
            {
                auto& cost = left_cost_r_[(i * width_ + j) * disp_range + d - min_disparity];
                if(j - half - d < 0){
                    break;
                }
                cv::Mat subMatRight = img_right_(cv::Rect(j - half - d, i - half, win_size, win_size));
                cost = caculateSAD(subMatLeft, subMatRight);
            }
    }

    //是否需要检查唯一性(逐像素计算右视图视差值,)
    //左影像(i,j)视差为d的代价 = 右影像(i,j+d)视差为d的代价
    if(option_.is_check_lr){
        //逐像素计算
        for(auto val = 0; val < img_right_mask_.size(); val++){
            auto i = img_right_mask_[val].first;
            auto j = img_right_mask_[val].second;

            for(auto d = min_disparity; d < max_disparity; d++){
                if(j + d > width_){
                    break;
                }
                auto index = d - min_disparity;
                auto& cost = right_cost_l_[(i * width_ + j) * disp_range + index];
                cost = left_cost_r_[(i * width_ + j + d) * disp_range + index];
            }
        }
    }
}


uint32_t BlockMatch::caculateSAD(const cv::Mat& left, const cv::Mat& right) const{
    cv::Mat temp1,temp2;
    left.convertTo(temp1, CV_32FC1);
    right.convertTo(temp2, CV_32FC1);
    auto res = static_cast<uint32_t>(cv::sum(cv::abs(temp1 - temp2))[0]);
    return res;
}

void BlockMatch::ComputeDisparity() const{
    const auto min_disparity = option_.min_disparity;
    const auto max_disparity = option_.max_disparity;
    const auto disp_range = max_disparity - min_disparity;
    if(disp_range <= 0) return;

    //左图像视差图
    const auto disparity = disp_left_;
    const auto width = width_;
    const auto height = height_;
    const bool is_check_unique = option_.is_check_unique;
    const auto uniqueness_thres = option_.uniqueness_thres;

    //为了加快读取效率,把单个像素的所有代价值存储在局部数组之中
    std::vector<uint32_t> cost_local(disp_range);

    //逐像素计算最优视差
    computingDispPerPixel(img_left_mask_, left_cost_r_, disp_left_);
    if(option_.is_check_lr){
        computingDispPerPixel(img_right_mask_, right_cost_l_, disp_right_);
    }

}
bool BlockMatch::Reset(const cv::Mat& left, const cv::Mat& right,const BMOption& option){
    //释放内存
    Release();

    //重置初始化标记
    is_initialized_ = false;
    return Initialize(left,right,option);
}

void BlockMatch::computingDispPerPixel(const std::vector<std::pair<uint16_t,uint16_t>>& mask, 
                                       const uint32_t* costValue, float* outputDisp) const{
    const auto min_disparity = option_.min_disparity;
    const auto max_disparity = option_.max_disparity;
    const auto disp_range = max_disparity - min_disparity;
    const auto width = width_;
    const auto height = height_;

    for(auto val = 0; val < mask.size();val++){
        auto i = mask[val].first;
        auto j = mask[val].second;
        uint32_t min_cost = UINT32_MAX;
        uint32_t sec_min_cost = UINT32_MAX;
        int best_disparity = 0;

        //遍历视差范围内的所有代价值，输出最小代价值及对应的视差值
        for(int d = min_disparity;d < max_disparity; d++){
            const int d_inx = d - min_disparity;
            uint32_t cost = left_cost_r_[(i * width_ + j) * disp_range + d - min_disparity];
            if(min_cost > cost){
                min_cost = cost;
                best_disparity = d;
            }
        }
        if(option_.is_check_unique){
            //再遍历一遍,输出次最优代价值
            for(int d = min_disparity;d < max_disparity;d++){
                if(d == best_disparity) continue;
                const auto& cost = costValue[d - min_disparity];
                sec_min_cost = std::min(sec_min_cost,cost);
            }

            // 判断唯一性约束
            // 若(min-sec)/min < min*(1-uniquness)，则为无效估计
            if (sec_min_cost - min_cost <= static_cast<uint16_t>(min_cost * (1 - option_.uniqueness_thres))) {
                    outputDisp[i * width + j] = Invalid_float;
                    continue;
                }
        }

        //无效像素筛选
        /*
        if(best_disparity == min_disparity || best_disparity == max_disparity){
            disparity[i * width + j] = Invalid_float;
            continue;
        }*/
        outputDisp[i * width + j] = static_cast<float>(best_disparity);
        /*
        // 最优视差前一个视差的代价值cost_1，后一个视差的代价值cost_2
        const sint32 idx_1 = best_disparity - 1 - min_disparity;
        const sint32 idx_2 = best_disparity + 1 - min_disparity;
        const uint16 cost_1 = cost_local[idx_1];
        const uint16 cost_2 = cost_local[idx_2];
        // 解一元二次曲线极值
        const uint16 denom = std::max(1, cost_1 + cost_2 - 2 * min_cost);
        disparity[i * width + j] = static_cast<float32>(best_disparity) + static_cast<float32>(cost_1 - cost_2) / (denom * 2.0f);
        */
    }
}