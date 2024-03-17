/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement if block match
*/
#ifndef BLOCKMATCH_H_
#define BLOCKMATCH_H_

#include<vector>
#include<limits>
#include"bm_util.h"

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

    BMOption():min_disparity(0),max_disparity(64),window_size(0),
                is_check_unique(false),uniqueness_thres(0.95f),
                is_check_lr(false),lrcheck_thres(1.0f),
                is_remove_speckles(false),min_speckle_area(20){}
   };

/**
 * \brief BlockMatch类
*/
class BlockMatch
{
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
public:
    BlockMatch();
    ~BlockMatch();

    /**
     * \brief The initialization of the class
     * \param width     input, the width of rectified stereo image
     * \param height    input, the height of rectified stereo image
     * \param winsize   input, the windows size of block
     * \param option    input, the parameters of BlockMatch
    */
   bool Initialize(const int& width, const int& height,const BMOption& option);

   /**
    * \brief running stereo matching
    * \param img_left   input, the pointer of left image
    * \param img_right  input, the pointer of right image
    * \param disp_left  output, left image parallax map pointer
   */
  bool Match(const uint8_t* img_left,const uint8_t* img_right,float* disp_left);

  /*
   * \brief reset parameter
   * \param width      input, the width of rectified stereo image 
   * \param height     input, the height of rectified stereo image
   * \param winsize    input, the window size 
   * \param option     input,  the parameters of SemiGlobalMatching
   */
  bool Reset(const uint32_t& width, const uint32_t& height,const uint8_t winsize, const BMOption& option);

private:
    /** \brief BM paramaters*/
    BMOption option_;

    uint16_t width_;
    uint16_t height_;

    /** \brief left image pointer*/
    const uint8_t* img_left_;

    /** \brief left image mask pointer*/
    std::vector<std::pair<uint16_t,uint16_t>> img_left_mask_;

    /** \brief right image pointer*/
    const uint8_t* img_right_;

    /** \brief right image mask pointer*/
    std::vector<std::pair<uint16_t,uint16_t>> img_right_mask_;

    /** \brief init cost */
    uint32_t* cost_init_;

    /** \brief left image disparity*/
    float* disp_left_;

    /** \brief right image disparity*/
    float* disp_right_;

    /** \brief Flag of init*/
    bool is_initialized_;

    bm_util::BM_Unit* block_;

    /** \brief Occluded area pixel vector*/
    std::vector<std::pair<int,int>> occlusions_;

    /** \brief Mismatched area pixel vector*/
    std::vector<std::pair<int,int>> mismatches_;
};
#endif // !BLOCKMATCH_H_