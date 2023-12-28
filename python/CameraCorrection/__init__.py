from .CameraMatrix import CameraMatrix
import os

def getCorrect(target_path,chessboard_size=(11,8),imgs=(2448,2048),square_size=24):
    os.path.exists(target_path)
    Matrix = CameraMatrix(chessboard_size,imgs,square_size,target_path)
    Matrix.caculate()