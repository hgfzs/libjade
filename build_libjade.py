# build_libjade.py

import os, subprocess

# Settings
clean = True

# Run qmake to generate makefiles
subprocess.call("qmake")

# Build source
subprocess.call("nmake release")

# Clean all intermediate files
if (clean):
	subprocess.call("nmake clean")
	os.rmdir("debug")
	os.rmdir("release")
	os.remove("Makefile.Debug")
	os.remove("Makefile.Release")
	os.remove("Makefile")
