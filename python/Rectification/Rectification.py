import cv2
import os
import numpy as np

class rectification:
    def __init__(self,ParPath):
        assert os.path.exists(ParPath)
        self.Path = ParPath
        self.steroPar = np.load(self.Path,allow_pickle=True).item()
        try:
            self.mtx_l = self.steroPar['mtx_l']
            self.dist_l = self.steroPar['dist_l']
            self.mtx_r = self.steroPar['mtx_r']
            self.dist_r = self.steroPar['dist_r']
            self.R = self.steroPar['rotation_matrix']
            self.T = self.steroPar['translation_matrix']
        except:
            raise ValueError
    
    def rectified(self,imgL,imgR,imgsize):
        if(os.path.exists(imgL)):
            imgL = cv2.cvtColor(cv2.imread(imgL),cv2.COLOR_BGR2GRAY)
            imgR = cv2.cvtColor(cv2.imread(imgR),cv2.COLOR_BGR2GRAY)
        else:
            raise ValueError
        #去畸变
        h,w = imgL.shape[:2]
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(self.mtx_l,self.dist_l,(w,h),0,(w,h))
        imgL = cv2.undistort(imgL, self.mtx_l, self.dist_l, None, newcameramtx)
        newcameramtx, roi = cv2.getOptimalNewCameraMatrix(self.mtx_r,self.dist_r,(w,h),0,(w,h))
        imgR = cv2.undistort(imgR, self.mtx_r, self.dist_r, None, newcameramtx)
        # 计算双目校正的矩阵
        R1, R2, P1, P2, Q, validPixROI1, validPixROI2 = cv2.stereoRectify(self.mtx_l, self.dist_l, self.mtx_r, self.dist_r, imgsize, self.R, self.T)
        # 计算校正后的映射关系
        maplx , maply = cv2.initUndistortRectifyMap(self.mtx_l, self.dist_l, R1, P1, imgsize, cv2.CV_16SC2)
        maprx , mapry = cv2.initUndistortRectifyMap(self.mtx_r, self.dist_r, R2, P2, imgsize, cv2.CV_16SC2)
        # 映射新图像
        lr = cv2.remap(imgL, maplx, maply, cv2.INTER_LINEAR)
        rr = cv2.remap(imgR, maprx, mapry, cv2.INTER_LINEAR)

        #all = np.hstack((lr,rr))
        """
        cv2.namedWindow("left", 0)
        cv2.resizeWindow("left", 1024, 1024)  # 设置窗口大小
        cv2.imshow('left', lr)
        cv2.namedWindow("right", 0)
        cv2.resizeWindow("right", 1024, 1024)  # 设置窗口大小
        cv2.imshow('right', rr)
        cv2.waitKey()
        cv2.destroyAllWindows()
        """
        return lr,rr    