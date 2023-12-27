from .Rectification import rectification

def getRectifed(ParPath,imgL,imgR,imgsize):
    A = rectification(ParPath)
    return A.rectified(imgL,imgR,imgsize)