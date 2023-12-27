/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement if block match
*/
#pragma onece
#include<vector>
#include<limits>
#include"bm_types.h"
#include"bm_util.h"

constexpr auto Invalid_float = std::numeric_limits<float32>::infinity();

/**
     * \brief BM parameter struct
    */
   struct BMOption
   {
    sint32 min_disparity;   //minimum disparity
    sint32 max_disparity;   //maximum disparity
    uint8 window_size;      //Block window size

    bool is_check_unique;   //whether to check uniqueness
    float32 uniqueness_thres;   //The uniqueness constraint threshold (minimum cost - second minimum cost) / minimum cost > threshold indicates valid pixels.

    bool is_check_lr;       //whether to chenck uniqueness of right and left
    float32 lrcheck_thres;  //The uniqueness constraint threshold of right and left

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
   bool Initialize(const sint32& width, const sint32& height,const BMOption& option);

   /**
    * \brief running stereo matching
    * \param img_left   input, the pointer of left image
    * \param img_right  input, the pointer of right image
    * \param disp_left  output, left image parallax map pointer
   */
  bool Match(const uint8* img_left,const uint8* img_right,float32* disp_left);

  /*
   * \brief reset parameter
   * \param width      input, the width of rectified stereo image 
   * \param height     input, the height of rectified stereo image
   * \param winsize    input, the window size 
   * \param option     input,  the parameters of SemiGlobalMatching
   */
  bool Reset(const uint32& width, const uint32& height,const uint8 winsize, const BMOption& option);

private:
    /** \brief BM paramaters*/
    BMOption option_;

    uint16 width_;
    uint16 height_;

    /** \brief left image pointer*/
    const uint8* img_left_;

    /** \brief left image mask pointer*/
    std::vector<std::pair<uint16,uint16>> img_left_mask_;

    /** \brief right image pointer*/
    const uint8* img_right_;

    /** \brief right image mask pointer*/
    std::vector<std::pair<uint16,uint16>> img_right_mask_;

    /** \brief init cost */
    uint8* cost_init_;

    /** \brief left image disparity*/
    float32* disp_left_;

    /** \brief right image disparity*/
    float32* disp_right_;

    /** \brief Flag of init*/
    bool is_initialized_;

    bm_util::BM_Unit* block_;

    /** \brief Occluded area pixel vector*/
    std::vector<std::pair<int,int>> occlusions_;

    /** \brief Mismatched area pixel vector*/
    std::vector<std::pair<int,int>> mismatches_;
};