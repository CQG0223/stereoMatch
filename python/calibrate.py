#!/bin/python
from lib.calibration import Calibration
import configparser

LOAD_DIR = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/Load'
IMG_DIR = '/media/cqg/CQG/CQGData/3L3DR_Res/C0010/target'
if __name__ == '__main__':
    config = configparser.ConfigParser()
    config.read('settings.conf')
    
    calibrator = Calibration(config, LOAD_DIR,IMG_DIR)
    calibrator.compute_calibration()