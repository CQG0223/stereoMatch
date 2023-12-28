import cv2
import numpy as np

class ProcessCalibing:
    def __init__(self,config,calibrator):
        self.config = config
        self.calibrator = calibrator

        self.width = int(config['general']['width'])
        self.height = int(config['general']['height'])
    
    def rectify(self,imgL,imgR):
        rectified_pair = self.__rect__(imgL, imgR)
        return rectified_pair[0],rectified_pair[1]
    
    def __rect__(self, left_frame, right_frame):
        # Convert to greyscale
        left_grey = cv2.cvtColor(left_frame, cv2.COLOR_BGR2GRAY)
        right_grey = cv2.cvtColor(right_frame, cv2.COLOR_BGR2GRAY)

        # Apply rectification
        return self.calibrator.rectify(left_grey, right_grey)