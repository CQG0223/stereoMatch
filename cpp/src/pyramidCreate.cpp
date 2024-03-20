#include"pyramidCreate.h"

void pyramidCreate::run(){
    auto n = 1;
    for(auto i = 0; i < N_; i++){
        cv::Mat left_temp = this->Pyramid(left_, n);
        cv::Mat right_temp = this->Pyramid(right_, n);
        pyramidResult_->emplace_back(std::make_pair(left_temp, right_temp));
        n *= 2;
    }
}

cv::Mat pyramidCreate::Pyramid(cv::Mat& image, int n){
    const auto new_width = image.cols / n;
    const auto new_height = image.rows / n;
    cv::Mat res;
    cv::resize(image,res,cv::Size(new_width, new_height), 0, 0, cv::INTER_NEAREST);
    return res;
}

bool pyramidCreate::CheckInvalid(){
    if(left_.data != nullptr && right_.data != nullptr && left_.cols == right_.cols &&
        left_.rows == right_.rows && pyramidResult_ != nullptr && N_ != 0){
            return true;
    }else{
        return false;
    }
}

void pyramidCreate::show(){
    if(!pyramidResult_->empty()){
        for(int i = 0; i < pyramidResult_->size(); i++){
            auto iter = (*pyramidResult_)[i];
            std::cout<<" index : "<<i<<"\t left : "<<iter.first.size
                                <<"\t right : "<<iter.second.size<<std::endl;
        }
    }
}