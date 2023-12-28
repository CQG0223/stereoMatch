import numpy as np
import cv2

ply_header = '''ply
format ascii 1.0
element vertex %(vert_num)d
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
end_header
'''

def write_ply(fn, verts, colors):
    verts = verts.reshape(-1, 3)
    colors = colors.reshape(-1, 3)
    verts = np.hstack([verts, colors])
    with open(fn, 'wb') as f:
        f.write((ply_header % dict(vert_num=len(verts))).encode('utf-8'))
        np.savetxt(f, verts, fmt='%f %f %f %d %d %d ')

def getdisparaty(imgL,imgR):
    #disparity range is tuned for 'aloe' image pair
    window_size = 11
    min_disp = 0
    max_disp = 3200 #min_disp * 9
    num_disp = max_disp - min_disp

    stereo = cv2.StereoSGBM_create(minDisparity = min_disp,
        numDisparities = num_disp,
        blockSize = 5,
        P1 = 8*3*window_size**2,
        P2 = 32*3*window_size**2,
        disp12MaxDiff = 1,
        uniquenessRatio = 10,
        speckleWindowSize = 100,
        speckleRange = 32
    )
    print('computing disparity...')
    disparity = stereo.compute(imgL, imgR)
    min_val = disparity.min()
    max_val = disparity.max()
    disparity = np.uint8(6400 * (disparity - min_val) / (max_val - min_val))

    cv2.namedWindow("left", 0)
    cv2.resizeWindow("left", 1024, 1024)  # 设置窗口大小
    cv2.imshow('left', imgL)
    cv2.namedWindow("right", 0)
    cv2.resizeWindow("right", 1024, 1024)  # 设置窗口大小
    cv2.imshow('right', imgR)
    cv2.namedWindow("disparity", 0)
    cv2.resizeWindow("disparity", 1024, 1024)  # 设置窗口大小
    cv2.imshow('disparity', disparity)
    cv2.waitKey()

    print('Done')