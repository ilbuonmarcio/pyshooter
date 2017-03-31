import sys, os
from cx_Freeze import setup, Executable

os.environ['TCL_LIBRARY'] = r'C:\Python35\tcl\tcl8.6'
os.environ['TK_LIBRARY'] = r'C:\Python35\tcl\tk8.6'

# Dependencies are automatically detected, but it might need fine tuning.
build_exe_options = {"packages": ["pygame", "random", "math"]}

# GUI applications require a different base on Windows (the default is for a
# console application).
base = None
if sys.platform == "win32":
    base = "Win32GUI"

setup(  name = "pyshooter",
        version = "0.1.6",
        description = "PY SHOOTER!",
        options = {"build_exe": build_exe_options},
        executables = [Executable("acad.py", base=base)])
