import cv2
import os

from stereovision.calibration import StereoCalibrator
from stereovision.calibration import StereoCalibration
from stereovision.exceptions import ChessboardNotFoundError

# Handles creation, saving and loading of calibration state for
# stereo cameras.
class Calibration:
    def __init__(self, config, load_directory, capture_directory=None):
        self.config = config
        self.width = int(config['general']['width'])
        self.height = int(config['general']['height'])

        self.chessboard_rows = int(config['calibration']['chessboard_rows'])
        self.chessboard_cols = int(config['calibration']['chessboard_cols'])
        self.chessboard_size = float(config['calibration']['chessboard_size'])
        self.chessboard_number = int(config['calibration']['chessboard_num'])

        self.left_camera_id = int(config['general']['left_camera_id'])
        self.right_camera_id = int(config['general']['right_camera_id'])

        self.capture_directory = capture_directory
        self.load_directory = load_directory

        if os.path.exists(self.load_directory) == False or len(os.listdir(self.load_directory)) == 0:
            self.active_calibration = None
        else:
            self.active_calibration = self.load_calibration()



    def has_calibration(self):
        return self.active_calibration != None

    def compute_calibration(self):
        print('Calibration :: Computing...')

        calibrator = StereoCalibrator( self.chessboard_rows,  self.chessboard_cols,  self.chessboard_size, (self.width, self.height))

        for i in range (1, self.chessboard_number, 1):
            if not os.path.exists(self.capture_directory + '/L/' + str(i) + '.bmp'):
                continue
            if not os.path.exists(self.capture_directory + '/R/' + str(i) + '.bmp'):
                continue

            left = cv2.imread(self.capture_directory + '/L/' + str(i) + '.bmp', 1)
            right = cv2.imread(self.capture_directory + '/R/' + str(i) + '.bmp', 1)

            try:
                calibrator._get_corners(left)
                calibrator._get_corners(right)
            except ChessboardNotFoundError as error:
                print(str(i) + ': ' + str(error))
            else:
                calibrator.add_corners((left, right))

        self.active_calibration = calibrator.calibrate_cameras()

        if not os.path.exists(self.load_directory):
            os.mkdir(self.load_directory)

        self.active_calibration.export(self.load_directory)

        print('Calibration :: Done')
        print('Calibration :: Exported to ' + self.load_directory)


    def load_calibration(self):
       return StereoCalibration(input_folder=self.load_directory)

    def rectify(self, left_frame, right_frame):
        return self.active_calibration.rectify((left_frame, right_frame))