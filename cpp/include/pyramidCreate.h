#ifndef PYRAMID_CREATE_H_
#define PYRAMID_CREATE_H_
#include<opencv4/opencv2/opencv.hpp>
#include<string>
#include<vector>
#include<exception>
#include<iostream>

class pyramidCreate
{
private:
    cv::Mat left_;
    cv::Mat right_;
    int N_;
    std::vector<std::pair<cv::Mat, cv::Mat>>* pyramidResult_;
    int width_;
    int height_;
public:
    pyramidCreate(cv::Mat& left, cv::Mat& right, int N): left_{left}, 
                                        right_{right},N_{N}{
        width_ = left_.cols;
        height_ = left_.rows;
        pyramidResult_ = new std::vector<std::pair<cv::Mat, cv::Mat>>();
        run();
    };
    ~pyramidCreate() = default;
    
    std::vector<std::pair<cv::Mat, cv::Mat>> getResult(){
        return *pyramidResult_;
    }
    void reset(cv::Mat& left, cv::Mat& right, int N){
        left_ = left;             
        right = right_;
        N_ = N;
        if(CheckInvalid()){
            pyramidResult_->clear();
            run();
        }else{
            throw std::runtime_error("Do not pass Check Function!");
        }
    };
    void show();
private:
    void run();
    bool CheckInvalid();
    cv::Mat Pyramid(cv::Mat&, int);
};

#endif // !PYRAMIDCREATE