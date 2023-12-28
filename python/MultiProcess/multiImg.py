import multiprocessing
import numpy as np

class ImageProcessor:
    def __init__(self, num_processes = None):
        if num_processes is None:
            num_processes = multiprocessing.cpu_count()
        self.num_processes = num_processes
        self.pool = None

    def process_image(self, image,winSize,maxDisp, row_processing_func):
        Src_Img,Tar_Img = image
        height, width = Src_Img.shape

        # 创建进程池
        self.pool = multiprocessing.Pool(processes=self.num_processes)

        results = []
        partitions = self.__partition_range__(height,self.num_processes)
        for part in partitions:
            # 分割图像为每个进程需要处理的块part
            chunk_Src = Src_Img[part[0]:part[1], :]
            chunk_Tar = Tar_Img[part[0]:part[1], :]
            # 提交任务给进程池
            result = self.pool.apply_async(row_processing_func, (chunk_Src,chunk_Tar,winSize,maxDisp))
            results.append(result)

        # 等待所有进程完成
        self.pool.close()
        self.pool.join()

        # 获取处理后的结果
        processed_chunks = []
        for result in results:
            processed_chunks.append(result.get())

        # 拼接处理后的每一块图像
        processed_image = np.concatenate(processed_chunks, axis=0)

        return processed_image

    def __del__(self):
        # 清理进程池
        if self.pool is not None:
            self.pool.terminate()
    
    def __partition_range__(self,x,n):
        quotient = x // n 
        remainder = x % n

        partitions = []
        start = 0
        for i in range(n):
            end = start + quotient
            if i < remainder:
                end += 1
            partitions.append((start,end))
            start = end
        return partitions