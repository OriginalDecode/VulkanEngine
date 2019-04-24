import os
import glob
import shutil

def appendToList(list, dirName, fileType):
    listOfFiles = glob.glob(dirName + fileType)
    for entry in listOfFiles:
        fullPath = os.path.join(dirName, entry)
        if os.path.isdir(fullPath):
            list = list + getListOfFiles(fullPath)
        else:
            list.append(fullPath)

def main():
    workingDir = os.getcwd()
    folder = workingDir + "/shaders"
    outputFolder = workingDir + "/../bin/Data/shaders"
    if not os.path.isdir(outputFolder):
        os.makedirs(outputFolder)

    print("input  folder : " + folder)
    print("output folder : " + outputFolder )
    shaderFiles = list()

    appendToList(shaderFiles, folder, "/*.*")

    for file in shaderFiles:
        outputFile = file[file.rfind('\\')+1:]
        print("\ninput : " + file)
        print("output : " + outputFolder + "/" + outputFile)
        command = "%VKBIN%/glslangValidator.exe " #validator path
        command += "-e main " #entrypoint, this should probably be changed into something else ?
        command += "-o " + outputFolder + "/" + outputFile + " " #output

        command += "-V " #-V needed to generate binary

        with open(file) as f:
            first_line = f.readline()
            if first_line.find("#version") == -1:
                print("is HLSL")
                command += "-D " #-D to tell the validator that it's HLSL code

        command += file 
        os.system(command) 

if __name__ == '__main__':
    main()