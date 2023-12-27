/* -*-C++-* stereoMatch - Copyright (C) 2023
* Author    :QG Chen
* Describle: implement if block match util
*/
#pragma onece
#include"bm_types.h"
#include <string>

#ifndef SAFE_DELETE
#define SAFE_DELETE(P) {if(P) delete[](P);P = nullptr;}
#endif 

namespace bm_util
{
	class BM_Unit
	{
	private:
		uint8 win_width_;
		uint8 win_height_;
		sint32 image_width_;
		sint32 image_height_;
	public:
		BM_Unit(const uint8 win_height,const uint8  win_width, 
				const sint32 image_height,const sint32 image_width){
			win_height_ = win_height;
			win_width_ = win_width;
			image_height_ = image_height;
			image_width_ = image_width;
		}
		BM_Unit():win_height_(0),win_width_(0),image_height_(0),image_width_(0){};
		/**
		 * \brief cost calculate(SAD)
		 * \param center_index		input,Center coordinates 
		 * \param src_pointer		input,image target block data
		 * \param tar_pointer		input,image target block data
		 * \param cost      		output, return cost value
		*/
		bool cost_calculate_SAD(std::pair<uint16,uint16> left_center,std::pair<uint16,uint16> right_center,
		const uint8* left_pointer,const uint8* right_pointer,int32_t cost){
			auto const left_raws = left_center.first;
			auto const left_cols = left_center.second;
			auto const left_index = left_raws*image_width_ + left_cols;

			auto const right_raws = right_center.first;
			auto const right_cols = right_center.second;
			auto const right_index = right_raws*image_width_ + right_cols;
			//索引范围判定
			if(left_index - win_width_ < 0 || left_index + win_width_ >= image_width_ * image_height_ ||
			   right_index - win_width_ < 0 || right_index + win_width_ >= image_width_ * image_height_){
				return false;
			}
			//在窗口内逐像素计算差
			auto win_half = (win_height_ - 1)/2;
			int32_t cost_val = 0;
			for (auto i = -win_half; i <= win_half; i++)
			{
				for (auto j = -win_half; j <= win_half; j++)
				{
					auto src_val = left_pointer[(left_raws + i)*image_width_+left_cols + j];
					auto tar_val = right_pointer[(right_raws + i)*image_width_+right_cols + j];
					cost_val += abs(static_cast<int32_t>(src_val) - static_cast<int32_t>(tar_val));
				}
			}
			if(cost_val < 0) return false;
			cost = cost_val;
			return true;
		}
	};
}