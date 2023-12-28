import unittest
import numpy as np
from HtmlTestRunner import HTMLTestRunner
from MultiProcess.multiImg import ImageProcessor

# 外部提供的行处理函数示例
def process_row(chunck):
    processed_chunck = np.zeros_like(chunck)
    return processed_chunck

class MultiProcessGeneratorTest(unittest.TestCase):
    def setUp(self):
        self.image = np.random.rand(1000,1000)

    def test_MultiProcess(self):
        num_processes = 8
        image_processor = ImageProcessor(num_processes)
        proce_image = image_processor.process_image(self.image,process_row)

        image_reference = np.zeros((1000,1000))
        self.assertTrue(np.array_equal(proce_image, image_reference))

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(MultiProcessGeneratorTest)
    print(suite)
    runner = HTMLTestRunner(output='./test_report.html')
    runner.run(suite)
    #unittest.main()