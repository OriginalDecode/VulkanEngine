import os
import glob
import shutil

workingDir = os.getcwd()

def appendToList(list, dirName, fileType):
    listOfFiles = glob.glob(dirName + fileType)
    for entry in listOfFiles:
        fullPath = os.path.join(dirName, entry)
        if os.path.isdir(fullPath):
            list = list + getListOfFiles(fullPath)
        else:
            list.append(fullPath)

def main():

    folder = workingDir + "/resources/shaders"
    outputFolder = workingDir + "/bin/Data/shaders"
    print("folder : " + folder)
    print("output : " + outputFolder)
    shaderFiles = list()
    appendToList(shaderFiles, folder, "/*.vert")

    for file in shaderFiles:

        outputFile = file[file.rfind('\\')+1:]
        print(outputFile)
        command = "%VKBIN%/glslangValidator.exe " #validator
        command += "-e main " #entrypoint
        command += "-o " + outputFolder + "/" + outputFile[:-4] + "sprv " #output
        command += "-V -D "
        command += file
        os.system(command) 

if __name__ == '__main__':
    main()