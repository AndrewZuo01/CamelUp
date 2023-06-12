import ctypes
import pathlib
import subprocess
from multiprocessing import Process


def gameStart():
    libname = pathlib.Path().absolute() / "libmain.so"
    c_lib = ctypes.cdll.LoadLibrary(libname)
    func = c_lib.gameboardStart()
def getFile():
    f = open("data.txt","r")
    lines = f.readlines()
    print(lines)
if __name__ == "__main__":
    p = Process(target=gameStart, args=())
    p.start()
    getFile()
    p.join()