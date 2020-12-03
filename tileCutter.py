from PIL import Image
import sys, os, math
from multiprocessing import Pool
from multiprocessing import cpu_count

maxZoom = 10

def pasteIntoResult(result, filename, x, y):
    try:
        i = Image.open(filename)
        result.paste(i, (x, y))
    except Exception, e:
        print "could not open %s" % (filename)
        # print "-- %s, removing %s" % (e, filename)
        # trash_dst = os.path.expanduser("~/.Trash/%s" % filename)
        # os.rename(filename, trash_dst)

def getFileNameForZoom(x,y,zoom):
    if (zoom == maxZoom):
        return (sys.argv[3]+sys.argv[4]+"%d_%d.png") % (x, y)
    else:
        directory = sys.argv[3]+("%d" % (zoom))
        if not os.path.exists(directory):
            os.makedirs(directory)
        return (directory+"/"+sys.argv[4]+"%d_%d.png") % (x, y)

class ZoomCreator:
    def __init__(self, constX, constZoom):
        self.constX = constX
        self.constZoom = constZoom

    def __call__(self, constY):
        self.createZoomLvl(self.constX, constY, self.constZoom)

    def createZoomLvl(self, x, y, currentZoom):
        result = Image.new("RGBA", (2*1024, 2*1024))
        pasteIntoResult(result, getFileNameForZoom(2*x,   2*y,   currentZoom+1), 0, 0)
        pasteIntoResult(result, getFileNameForZoom(2*x+1, 2*y,   currentZoom+1), 1024, 0)
        pasteIntoResult(result, getFileNameForZoom(2*x,   2*y+1, currentZoom+1), 0, 1024)
        pasteIntoResult(result, getFileNameForZoom(2*x+1, 2*y+1, currentZoom+1), 1024, 1024)
        result = result.resize((1024, 1024), Image.LANCZOS)
        result.save(getFileNameForZoom(x, y, currentZoom))


if __name__ == '__main__':
    print "cpu_count() = %d" % (cpu_count())

    maxX = int(sys.argv[1])
    maxY = int(sys.argv[2])

    print "maxX", maxX
    print "maxY", maxY

    for currentZoom in reversed(xrange(1, maxZoom)):
        pool = Pool(cpu_count())

        xRange = xrange(0, int(math.ceil(float(maxX+1) / pow(2.0, maxZoom - currentZoom))))
        yRange = xrange(0, int(math.ceil(float(maxY+1) / pow(2.0, maxZoom - currentZoom))))

        for x in xRange:
            results = pool.map(ZoomCreator(x, currentZoom), yRange)

        pool.close()
        pool.join()

