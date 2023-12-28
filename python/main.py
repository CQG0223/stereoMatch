from core import getSAD,getSADMulti
from visualization import test,pairShow,Posion  
import cv2
from lib.calibration import Calibration
import configparser
from lib.calibrating import ProcessCalibing

LOAD_DIR = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/Load'
IMG_DIR = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/target'

imgLPath = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/absoluteImg/L/11_18L.bmp'
imgRPath = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/absoluteImg/R/11_18R.bmp'

if __name__ == '__main__':
    imgL = cv2.imread(imgLPath,1)
    imgR = cv2.imread(imgRPath,1)
    config = configparser.ConfigParser()
    config.read('settings.conf')

    calibrator = Calibration(config, LOAD_DIR)
    if not calibrator.has_calibration():
        print('Calibration required! Please run  calibrate.py')
    else:
        calibrating = ProcessCalibing(config,calibrator)
        recedL,recedR = calibrating.rectify(imgL,imgR)
        #recedL = cv2.resize(recedL,(320,240))
        #recedR = cv2.resize(recedR,(320,240))
        #cv2.imwrite("/media/cqg/CQG/CQGData/3L3DR_Res/C0010/recded/L/11_18L.bmp",recedL)
        #cv2.imwrite("/media/cqg/CQG/CQGData/3L3DR_Res/C0010/recded/R/11_18R.bmp",recedR)
        getSAD(recedL,recedR)
        #getSADMulti(recedL,recedR,11,500)