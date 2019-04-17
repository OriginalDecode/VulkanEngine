import os
import glob
import shutil
import platform
from argparse import ArgumentParser

workingDir = os.getcwd()
system = platform.system()


# args
# ____________________________________________________________

parser = ArgumentParser()

parser.add_argument("-c", "--configure", dest="config", help="configuration : vs2015, vs2017, gmake, specifies type of build toolchain to be used", metavar="")
parser.add_argument("-f", "--file", dest="file", help="optional premake lua", metavar="")

if system=="Linux":
    parser.add_argument("-bc", "--buildConfig", dest="buildConfig", help="build configuration, debug_platform, release_platform, DEFINE_PLATFORM", metavar="")
    parser.add_argument("-b", "--build", action="store_true", help="tell the script to build")
elif system=="Windows":
    parser.add_argument("-bc", "--buildConfig", dest="buildConfig", help="build configuration, Debug|Release|OTHER", metavar="")
    parser.add_argument("-b", "--build", dest="build", help="specify solution file", metavar="")

parser.add_argument("--clean", action='store_true', help="tell the script to clean the tree")
parser.add_argument("-v", dest="verbosity", help="set the verbosity of the compiler. values=[ q[uiet], m[inimal], n[ormal], d[etailed], diag[nostic]")


args = parser.parse_args()


# Utils
# ____________________________________________________________
os.system('cls' if os.name=='nt' else 'clear')


# Utils
# ____________________________________________________________

def red(str):
    return "\033[1;31;40m"+str+"\033[0;37;40m"

def green(str):
    return "\033[1;32;40m"+str+"\033[0;37;40m"

def yellow(str):
    return "\033[1;33;40m"+str+"\033[0;37;40m"

def white(str):
    return "\033[1;37;40m"+str+"\033[0;37;40m"

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


#clean the build. Removes all files. .pdb, .exe, .vcxproj etc...
# ____________________________________________________________
def rmTreeErr(*args):
    func, path, _ = args
    print(red("There was an error removing the folder"))
    print(func)
    print(path)

if args.clean == True:
    print(white("===Starting cleanup==="))
    buildFolder = workingDir + "/build";

    if os.path.isdir(buildFolder):
        shutil.rmtree(buildFolder, onerror=rmTreeErr)
    else:
        print(white("No build folder."))

    files = getListOfFiles(workingDir)

    if len(files) > 0:
        for f in files:
            try:
                os.remove(f)
                print("Removed : " + red(f))
            except:
                print(red("There was an error removing " + f))

    else:
        print(white("No files to remove."))
    
    print(green("===Clean finished==="))


## configure the build
# ____________________________________________________________
def configure(command):
    if args.file == None:
        os.system(command + " " + args.config)
    else:
        os.system(command + " " + args.file + " " + args.config)


if args.config != None:
    print(white("===Starting configure==="))
    if system == "Windows":
        configure("premake5")
    elif system == "Linux":
        configure("./premake5")
    print(green("===Configure done==="))


#build
# ____________________________________________________________
def buildWindows(buildConfig):
    print(system)

    if buildConfig == None:
        buildConfig="Debug"
    
    print(buildConfig)

    verbosity = args.verbosity
    
    if args.verbosity == None:
        verbosity = "n"

    os.system("msbuild " + "build/" + args.build + 
        " /p:GenerateFullPaths=true" +
        " /p:Configuration=" + buildConfig + 
        " /p:Platform=Windows" +
        " /t:build" +
        " -v:" + verbosity
        )

def buildLinux(buildConfig):
    print(system)

    if buildConfig == None:
        buildConfig = "debug_" + system.lower()

    os.chdir("build")
    os.system("make config=" + buildConfig)
    os.chdir("..")

def canBuild():
    return args.build != None and args.build != False

if canBuild():
    print(white("===Starting build==="))
    buildConfig = args.buildConfig 

    if system == "Windows":
        buildWindows(buildConfig)
    elif system == "Linux":
        buildLinux(buildConfig)

    print(green("===Finished build==="))