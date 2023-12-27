import cv2
import numpy as np
import os
IMG_EXTENSIONS = ['.jpg', '.png','.bmp']
def is_image(filename):
    return any(filename.endswith(ext) for ext in IMG_EXTENSIONS)

class CameraMatrix:
    def __init__(self,chessboard_size,imgs,square_size,target_path):
        # 定义棋盘格的大小
        self.chessboard_size = chessboard_size
        # 定义图像分辨率，根据自己相机的分辨率修改
        self.img_size = imgs
        # 定义棋盘格中每个格子的物理大小，自己用尺子量，单位为毫米（mm）
        self.square_size = square_size
        self.MatrixSaved = {}

        # 定义棋盘格模板的点的坐标
        #生成每个角点三维坐标，共有chessboard_size[0]*chessboard_size[1]个坐标，z轴置0不影响
        self.objp = np.zeros((chessboard_size[0]*chessboard_size[1], 3), np.float32) 
        self.objp[:, :2] = np.mgrid[0:chessboard_size[0], 0:chessboard_size[1]].T.reshape(-1, 2) * square_size #计算得到每个角点的x,y

        # 读取所有棋盘格图像并提取角点
        self.imgpoints_left, self.imgpoints_right = [], []  # 存储图像中的角点
        self.objpoints = []  # 存储模板中的角点

        filenameL = os.path.join(target_path,'L')
        filenameR = os.path.join(target_path,'R')
        self.target_listL = [os.path.join(dp,f) for dp,dn,fn in os.walk(os.path.expanduser(filenameL))for f in fn if is_image(f)]
        self.target_listR = [os.path.join(dp,f) for dp,dn,fn in os.walk(os.path.expanduser(filenameR))for f in fn if is_image(f)]
        self.target_number = len(self.target_listL)
        assert len(self.target_listR) == len(self.target_listL)
        print("{} fond targets : {}".format(target_path,self.target_number))
    
    def caculate(self):
        for L in self.target_listL:
            img = cv2.imread(L)
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            ret, corners = cv2.findChessboardCorners(gray, self.chessboard_size, None) #计算corner
            ret, corners = cv2.find4QuadCornerSubpix(gray, corners, (7,7)) #提高角点检测的准确性和稳定性
            if ret == True:
                self.imgpoints_left.append(corners)
                self.objpoints.append(self.objp)
        
        for R in self.target_listR:
            img = cv2.imread(R)
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            ret, corners = cv2.findChessboardCorners(gray, self.chessboard_size, None) #计算corner
            ret, corners = cv2.find4QuadCornerSubpix(gray, corners, (7,7)) #提高角点检测的准确性和稳定性
            if ret == True:
                self.imgpoints_right.append(corners)
        
        # 标定相机，获得内参和畸变参数
        ret, mtx_r, dist_r, rvecs_r, tvecs_r = cv2.calibrateCamera(self.objpoints, self.imgpoints_right, gray.shape[::-1], None, None)
        ret, mtx_l, dist_l, rvecs_l, tvecs_l = cv2.calibrateCamera(self.objpoints, self.imgpoints_left, gray.shape[::-1], None, None)

        # 指定迭代次数最大30或者误差小于0.001
        term = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

        # 进行双目相机标定,主要是获得R，T两个矩阵
        rotation_matrix, translation_matrix = cv2.stereoCalibrate(
                    self.objpoints, self.imgpoints_left, self.imgpoints_right,
                    mtx_l, dist_l,
                    mtx_r, dist_r,
                    self.img_size, flags=cv2.CALIB_FIX_INTRINSIC, criteria=term)[5:7]
        
        # 获得矫正矩阵和投影矩阵，用于后续进行图像校正
        rect_left, rect_right,proj_left, proj_right,dispartity, ROI_left, ROI_right = cv2.stereoRectify(
                    mtx_l, dist_l,
                    mtx_r, dist_r,
                    self.img_size, rotation_matrix, translation_matrix,
                    flags=cv2.CALIB_ZERO_DISPARITY, alpha=-1)
        
        self.MatrixSaved = {'mtx_l':mtx_l,'mtx_r':mtx_r,'dist_l':dist_l,'dist_r':dist_r,'rect_left':rect_left,'rect_right':rect_right,'proj_left':proj_left
                            ,'proj_right':proj_right,'dispartity':dispartity,'ROI_left':ROI_left
                            ,'ROI_right':ROI_right,'rotation_matrix':rotation_matrix,'translation_matrix':translation_matrix}
        np.save('./stereoParameter',self.MatrixSaved)

        '''
        打印结果
        '''
        print('mtx_l = np.array({})'.format(np.array2string(mtx_l, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('mtx_r = np.array({})'.format(np.array2string(mtx_r, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('dist_l = np.array({})'.format(np.array2string(dist_l, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('dist_r = np.array({})'.format(np.array2string(dist_r, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('R = np.array({})'.format(np.array2string(rotation_matrix, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('T = np.array({})'.format(np.array2string(translation_matrix, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('rect_left = np.array({})'.format(np.array2string(rect_left, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('rect_right = np.array({})'.format(np.array2string(rect_right, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('proj_left = np.array({})'.format(np.array2string(proj_left, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('proj_right = np.array({})'.format(np.array2string(proj_right, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        print('dispartity = np.array({})'.format(np.array2string(dispartity, separator=', ', formatter={'int': lambda x: f'{x: 3d}'},prefix='[', suffix=']')))
        # print('mtx_l = np.array({})'.format(mtx_l))
        # print('mtx_r = np.array({})'.format(mtx_r))
        # print('dist_l = np.array({})'.format(dist_l))
        # print('dist_r = np.array({})'.format(dist_r))
        # print('R = np.array({})'.format(rotation_matrix))
        # print('T = np.array({})'.format(translation_matrix))
        # print('rect_left = np.array({})'.format(rect_left))
        # print('rect_right = np.array({})'.format(rect_right))
        # print('proj_left = np.array({})'.format(proj_left))
        # print('proj_right = np.array({})'.format(proj_right))
        # print('dispartity = np.array({})'.format(dispartity))
        print('ROI_left = np.array({})'.format(ROI_left))
        print('ROI_right = np.array({})'.format(ROI_right))
        
