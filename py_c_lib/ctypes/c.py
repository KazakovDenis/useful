import ctypes
import os


root = os.path.dirname(__file__)
c_file = os.path.join(root, 'clib.so')

lib = ctypes.CDLL(c_file)
lib.hello()
