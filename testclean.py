import os
import sys
from glob import glob
import re
#

OBJRE = re.compile("^OBJECTS_DIR\s*=\s*(.*)\n", re.MULTILINE)
MOCRE = re.compile("^MOC_DIR\s*=\s*(.*)\n", re.MULTILINE)

# find all unit tests directories
directories = set()
for item in os.walk("."):
    if os.path.basename(item[0]) == "unit_tests":
        for path in item[1]:
            fullpath = os.path.join(item[0], path)
            if path.startswith("."): continue
            if path.endswith(".app"): continue # skip mac apps
            if os.path.isdir(fullpath):
                directories.add(fullpath)
                
# this one does not contain unit test dirs, but is unit test by itself
directories.add(os.path.join('.','src','compiler_lib','unit_tests')) 

for directory in directories:
    profiles = glob(directory+"/*.pro")
    if not profiles:
        print directory
        print "No pro file found in specified directory. Exiting."
        sys.exit(1)
    profile = open(profiles[0]).read()
    objdirs = OBJRE.findall(profile)
    objdir = directory
    if not objdirs:
        print "There is no OBJECTS_DIR variable set in pro file, using project directory."
    else:
        objdir = objdirs[0]
        if objdir.startswith('.'):
            objdir = os.path.join(directory, objdir)

    mocdirs = MOCRE.findall(profile)
    mocdir = directory
    if not mocdirs:
        print "There is no MOC_DIR variable set in pro file, using project directory."
    else:
        mocdir = mocdirs[0]
        if mocdir.startswith('.'):
            mocdir = os.path.join(directory, mocdir)

    print "Cleaning files in directories:", objdir, mocdir
    files = glob(directory+"/*.cpp")
    for file in files:
        directory = os.path.dirname(file)
        basename = os.path.basename(file)
        noext = '.'.join(basename.split('.')[:-1])
        obj = os.path.join(objdir, noext+".o")
        moc = os.path.join(mocdir, noext+".o")
        if os.path.exists(obj):
            print "deleting", obj
            os.remove(obj)
        if os.path.exists(moc):
            print "deleting", moc
            os.remove(moc)
    print directory, ": done."
print "all done."