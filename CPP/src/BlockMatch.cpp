/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement if block match
*/
#include<chrono>
#include<cstdio>
#include<cstring>
#include"BlockMatch.h"

BlockMatch::BlockMatch():width_(0),height_(0),img_left_(nullptr),img_right_(nullptr),
                        cost_init_(nullptr),disp_left_(nullptr),disp_right_(nullptr),
                        is_initialized_(false),block_(nullptr)
{}

BlockMatch::~BlockMatch(){
    Release();
    is_initialized_ = false;
}

bool BlockMatch::Initialize(const sint32& width, const sint32& height,const BMOption& option){
    //图像尺寸
    width_ = width;
    height_ = height;
    //BM参数
    option_ = option;
    auto winsize = option_.window_size;

    if(width_ == 0 || height == 0) return false;

    //声明块相关类
    block_ = new bm_util::BM_Unit(winsize,winsize,height,width);
    
    //左右图像
    const sint32 image_size = height_ * width_;

    img_left_ = new uint8[image_size]();
    img_right_ = new uint8[image_size]();

    //视差范围
    const sint32 disp_range = option_.max_disparity - option_.min_disparity;
    if(disp_range <= 0) return false;

    //匹配代价初始化
    const sint32 size = width * height * disp_range;
    cost_init_ = new uint8[size]();

    //视差图初始化
    disp_left_ = new float32[image_size]();
    disp_right_ = new float32[image_size]();
    is_initialized_ = cost_init_ && disp_left_ && disp_right_ && img_left_ && img_right_ && block_;
    
    return is_initialized_;
}

void BlockMatch::Release(){
    //释放内存
    SAFE_DELETE(cost_init_);
    SAFE_DELETE(disp_left_);
    SAFE_DELETE(disp_right_);
    SAFE_DELETE(img_left_);
    SAFE_DELETE(img_right_);
    SAFE_DELETE(block_);
}

void BlockMatch::source_process_effective(){
    //逐像素统计有效点,并将之放在对应的const 数组之中
    for (auto i = 0; i < height_; i++)
    {
        for (auto j = 0; j < width_; j++)
        {
            auto gray_left = img_left_[i*width_ + j];
            if(gray_left != 0){
                img_left_mask_.push_back(std::pair<uint16,uint16>(i,j));
            }
            auto gray_right = img_right_[i*width_ + j];
            if(gray_right != 0){
                img_right_mask_.push_back(std::pair<uint16,uint16>(i,j));
            }
        }
    }
}

bool BlockMatch::Match(const uint8* img_left,const uint8* img_right,float32* disp_left){
    if(!is_initialized_ || img_left == nullptr || img_right == nullptr) return false;
    img_left_ = img_left;
    img_right_ = img_right;
    source_process_effective();
    
    auto start = std::chrono::steady_clock::now();

    //代价计算
    ComputeCost();

    auto end = std::chrono::steady_clock::now();
    auto tt = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    printf("computing cost! timing :	%lf s\n", tt.count() / 1000.0);
    start = std::chrono::steady_clock::now();

    //视差计算
    ComputeDisparity();
    end = std::chrono::steady_clock::now();
    tt = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    printf("computing disparities! timing :	%lf s\n", tt.count() / 1000.0);
    start = std::chrono::steady_clock::now();

    //输出视差图
    std::memcpy(disp_left,disp_left_,height_ * width_ * sizeof(float32));
    return true;
}   

void BlockMatch::ComputeCost() const{
    const sint32& min_disparity = option_.min_disparity;
    const sint32& max_disparity = option_.max_disparity;
    const sint32 disp_range = max_disparity - min_disparity;

    //逐像素计算匹配代价
    for(auto left:img_left_mask_){
        auto i = left.first;
        auto j = left.second;
        for (auto d = min_disparity; d < max_disparity; d++)
            {
                auto& cost = cost_init_[(i * width_ +j)*disp_range + d - min_disparity];//获取对应cost位置的引用
                if(j - max_disparity/2 < 0 || j + max_disparity/2 >= width_){
                    cost = UINT8_MAX / 2;
                    continue;
                }
                auto dd = d - (max_disparity / 2);
                auto right = std::pair<uint16,uint16>(i,j+dd);
                block_->cost_calculate_SAD(left,right,img_left_,img_right_,cost);
            }
    }
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
    std::vector<uint16> cost_local(disp_range);

    //逐像素计算最优视差
    for(auto val:img_left_mask_){
        auto i = val.first;
        auto j = val.second;
        uint16 min_cost = UINT16_MAX;
        uint16 sec_min_cost = UINT16_MAX;
        sint32 best_disparity = 0;

        //遍历视差范围内的所有代价值，输出最小代价值及对应的视差值
        for(sint32 d = min_disparity;d < max_disparity; d++){
            const sint32 d_inx = d - min_disparity;
            const auto& cost = cost_local[d_inx] = cost_init_[i * width * disp_range + j * disp_range + d_inx];
            if(min_cost > cost){
                min_cost = cost;
                best_disparity = d;
            }
        }
        if(is_check_unique){
            //再遍历一遍,输出次最优代价值
            for(sint32 d = min_disparity;d < max_disparity;d++){
                if(d == best_disparity) continue;
                const auto& cost = cost_local[d - min_disparity];
                sec_min_cost = std::min(sec_min_cost,cost);
            }

            // 判断唯一性约束
            // 若(min-sec)/min < min*(1-uniquness)，则为无效估计
            if (sec_min_cost - min_cost <= static_cast<uint16>(min_cost * (1 - uniqueness_thres))) {
                    disparity[i * width + j] = Invalid_float;
                    continue;
                }
        }

        //无效像素筛选
        if(best_disparity == min_disparity || best_disparity == max_disparity - 1){
            disparity[i * width + j] = Invalid_float;
            continue;
        }
        // 最优视差前一个视差的代价值cost_1，后一个视差的代价值cost_2
        const sint32 idx_1 = best_disparity - 1 - min_disparity;
        const sint32 idx_2 = best_disparity + 1 - min_disparity;
        const uint16 cost_1 = cost_local[idx_1];
        const uint16 cost_2 = cost_local[idx_2];
        // 解一元二次曲线极值
        const uint16 denom = std::max(1, cost_1 + cost_2 - 2 * min_cost);
        disparity[i * width + j] = static_cast<float32>(best_disparity) + static_cast<float32>(cost_1 - cost_2) / (denom * 2.0f);
    }
}
bool BlockMatch::Reset(const uint32& width, const uint32& height,const uint8 winsize, const BMOption& option){
    //释放内存
    Release();

    //重置初始化标记
    is_initialized_ = false;

    return Initialize(width,height,option);
}