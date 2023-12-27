import unittest
import numpy as np
from HtmlTestRunner import HTMLTestRunner
from core.windowSplit import SlidingWindowGenerator

class SlidingWindowGeneratorTest(unittest.TestCase):
    def setUp(self):
        # 创建一个示例灰度图像
        self.image = np.array([[1, 2, 3, 4, 5 , 6 , 7 ,8],
                               [6, 7, 8, 9, 10 , 11 , 12, 13],
                               [11, 12, 13, 14, 15, 16, 17, 18],
                               [16, 17, 18, 19, 20, 21, 22, 23],
                               [24, 25, 26, 27, 28, 29, 30, 31],
                               [32, 33, 34, 35, 36, 37, 38, 39]])

        # 创建 SlidingWindowGenerator 实例
        self.window_generator = SlidingWindowGenerator(self.image)

    def test_generate_windows3X3(self):
        center_col = 2
        center_raw = 2
        window_size = 3
        num_windows_left = 1
        num_windows_right = 1

        # 生成滑动窗口
        windows = list(self.window_generator.generate_windows(center_col,center_raw, window_size, num_windows_left, num_windows_right))

        expected_windows = [
            np.array([[6, 7, 8],
                      [11, 12, 13],
                      [16, 17, 18]]),
            np.array([[7, 8, 9],
                      [12, 13, 14],
                      [17, 18, 19]]),
            np.array([[8, 9, 10],  
                      [13, 14, 15],
                      [18, 19, 20]])
        ]
        expected_box = [[(0,1),(2,3)],
                        [(1,1),(3,3)],
                        [(2,1),(4,3)]]
        center_index = [(1,2),
                        (2,2),
                        (3,2)]

        self.assertEqual(len(windows), len(expected_windows))
        for i in range(len(windows)):
            self.assertTrue(np.array_equal(windows[i].image, expected_windows[i]))
            self.assertTrue(np.array_equal(windows[i].box, expected_box[i]))
            self.assertTrue(np.array_equal(windows[i].centerX, center_index[i]))

    def test_generate_windows5X5(self):
        center_col = 2
        center_raw = 2
        window_size = 5
        num_windows_left = 3
        num_windows_right = 5

        # 生成滑动窗口
        windows = list(self.window_generator.generate_windows(center_col,center_raw, window_size, num_windows_left, num_windows_right))

        expected_windows = [
            np.array([[1, 2, 3, 4, 5],
                      [6, 7, 8, 9, 10],
                      [11, 12, 13, 14, 15],
                      [16, 17, 18, 19, 20],
                      [24, 25, 26, 27, 28]]),
            np.array([[ 2, 3, 4, 5, 6],
                      [ 7, 8, 9, 10, 11],
                      [ 12, 13, 14, 15, 16],
                      [ 17, 18, 19, 20, 21],
                      [ 25, 26, 27, 28, 29]]),
            np.array([[3, 4, 5, 6, 7],
                      [8, 9, 10, 11, 12],
                      [ 13, 14, 15, 16, 17],
                      [ 18, 19, 20, 21, 22],
                      [ 26, 27, 28, 29, 30]]),
            np.array([[4, 5, 6, 7, 8],
                      [9, 10, 11, 12, 13],
                      [14, 15, 16, 17, 18],
                      [19, 20, 21, 22, 23],
                      [27, 28, 29, 30, 31]]),
        ]
        expected_box = [[(0,0),(4,4)],
                        [(1,0),(5,4)],
                        [(2,0),(6,4)],
                        [(3,0),(7,4)]]
        center_index = [(2,2),
                        (3,2),
                        (4,2),
                        (5,2)]

        self.assertEqual(len(windows), len(expected_windows))
        for i in range(len(windows)):
            self.assertTrue(np.array_equal(windows[i].image, expected_windows[i]))
            self.assertTrue(np.array_equal(windows[i].box, expected_box[i]))
            self.assertTrue(np.array_equal(windows[i].centerX, center_index[i]))

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(SlidingWindowGeneratorTest)
    print(suite)
    runner = HTMLTestRunner(output='./test_report.html')
    runner.run(suite)
    #unittest.main()