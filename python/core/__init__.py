from .SAD import SAD,cacualteSAD
import numpy as np
import cv2
from MultiProcess.multiImg import ImageProcessor

def getSAD(Src,Tar):
    A = SAD(101,10000)
    mask = Tar > 0
    A.cacualte(Src,Tar,mask)

def getSADMulti(Src,Tar,winSize,maxDisp):
    
    num_processes = 8
    image_processor = ImageProcessor(num_processes)
    
    proce_image = image_processor.process_image([Src,Tar],winSize,maxDisp,cacualteSAD)

    cv2.imwrite('out.jpg',proce_image)
    cv2.namedWindow("result")
    cv2.imshow("result",proce_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    
def windowTest(file,winSize):
    img = np.arange(0,100)
    Src = img.reshape(10,10)
    with open(file,"a",encoding='utf-8') as f:
        f.writelines("Sourse img:\n")
        for i in Src:
            f.write(str(i))
            f.write("\n")
        f.write("\n")
    
    for i in range(10):
        A = SAD(winSize)
        IterWindows = A.cacualte(Src,i)
        with open(file,"a",encoding='utf-8') as f:
            f.write("=====================")
            f.write("winSize = {};length win = {}\n".format(winSize,len(IterWindows)))
            for index,k in enumerate(IterWindows):
                f.write("Intex:{}\n".format(index))
                for i in k:
                    f.write(str(i))
                    f.write("\n")
                f.write("\n")