import os
import glob
import shutil
import platform
from argparse import ArgumentParser

workingDir = os.getcwd()
system = platform.system()

parser = ArgumentParser()
parser.add_argument("-c", "--configure", dest="config", help="configuration : vs2015, vs2017, gmake, specifies type of build toolchain to be used", metavar="")
parser.add_argument("-f", "--file", dest="file", help="optional premake lua", metavar="")

if system=="Linux":
    parser.add_argument("-bc", "--buildConfig", dest="buildConfig", help="build configuration, debug_platform, release_platform, DEFINE_PLATFORM", metavar="")
elif system=="Windows":
    parser.add_argument("-bc", "--buildConfig", dest="buildConfig", help="build configuration, Debug|Release", metavar="")

parser.add_argument("--clean", action='store_true', help="tell the script to clean the tree")

if system=="Linux":
    parser.add_argument("-b", "--build", action="store_true", help="tell the script to build")
elif system == "Windows":
    parser.add_argument("-b", "--build", dest="build", help="specify solution file", metavar="")

args = parser.parse_args()


os.system('cls' if os.name=='nt' else 'clear')

# ____________________________________________________________

def appendToList(list, dirName, fileType):
    listOfFiles = glob.glob(dirName + "/*/" + fileType)
    
    for entry in listOfFiles:
        fullPath = os.path.join(dirName, entry)
        if os.path.isdir(fullPath):
            list = list + getListOfFiles(fullPath)
        else:
            list.append(fullPath)
    

def getListOfFiles(dirName):
    # listOfFiles = glob.glob(dirName + "/*/*.vcxproj*")
    
    allFiles = list()
    
    appendToList(allFiles, dirName, "*.vcxproj*")
    appendToList(allFiles, dirName, "*Makefile*")
        
    return allFiles

# ____________________________________________________________

def printErr(err):
    print("\033[1;31;40m"+err+"\033[0;37;40m")

def printWarn(warn):
    print("\033[1;33;40m"+warn+"\033[0;37;40m")

def printOk(ok):
    print("\033[1;32;40m"+ok+"\033[0;37;40m")

def printMsg(msg):
    print("\033[1;37;40m"+msg+"\033[0;37;40m")


def rmTreeErr(*args):
    func, path, _ = args
    printErr("There was an error removing the folder")
    print(func)
    print(path)

#clean the build. Removes all files. .pdb, .exe, .vcxproj etc...
if args.clean == True:

    buildFolder = workingDir + "/build";

    if os.path.isdir(buildFolder):
        shutil.rmtree(buildFolder, onerror=rmTreeErr)
    else:
        printMsg("No build folder.")

    files = getListOfFiles(workingDir)

    if len(files) > 0:
        print("Will remove")
        print("\n".join(files))
        for f in files:
            os.remove(f)
    else:
        printMsg("No files to remove.")
    
    printOk("clean finished");

def configure(command):
    if args.file == None:
        os.system(command + " " + args.config)
    else:
        os.system(command + " " + args.file + " " + args.config)

def canBuild():
    return args.build != None and args.build != False


## configure the build
if args.config != None:
    printMsg("Starting configure")
    if system == "Windows":
        configure("premake5")
    elif system == "Linux":
        configure("./premake5")
    printOk("Configure done")

def buildWindows(buildConfig):
        os.system("msbuild " + "build/" + args.build + 
            " /p:GenerateFullPaths=true" +
            " /p:Configuration=" + buildConfig + 
            " /t:build")

#build
if canBuild():
    print("Starting build")
    buildConfig = args.buildConfig 

    if system == "Windows":
        if buildConfig == None:
            buildConfig="Debug"

        

    elif system == "Linux":
        if buildConfig == None:
            buildConfig = "debug_" + system.lower()
        os.chdir("build")
        os.system("make config=" + buildConfig)
        os.chdir("..")
    
