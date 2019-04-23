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

# shader suffixes or commands
# vert, tesc(hull), tese(domain), geom, frag, comp

def main():

    folder = workingDir + "/shaders"
    outputFolder = workingDir + "/../bin/Data/shaders"
    if not os.path.isdir(outputFolder):
        os.makedirs(outputFolder)

    print("folder : " + folder)
    print("output : " + outputFolder)
    shaderFiles = list()

    fileTypes = { 
        ".vert" : "v", 
        ".frag" : "f", 
        ".comp" : "c", 
        ".geom" : "g", 
        ".tese" : "te", 
        ".tesc" : "tc" 
        }

    appendToList(shaderFiles, folder, "/*.vert")
    appendToList(shaderFiles, folder, "/*.frag")
    appendToList(shaderFiles, folder, "/*.comp")
    appendToList(shaderFiles, folder, "/*.geom")
    appendToList(shaderFiles, folder, "/*.tese")
    appendToList(shaderFiles, folder, "/*.tesc")
    
    for file in shaderFiles:
        suffix = "unknown"
        for key, value in fileTypes.iteritems():
            if file.find(key) != -1:
                suffix = value

        outputFile = file[file.rfind('\\')+1:]
        print(outputFile)
        command = "%VKBIN%/glslangValidator.exe " #validator path
        command += "-e main " #entrypoint, this should probably be changed into something else ?
        command += "-o " + outputFolder + "/" + outputFile[:-4] + "spv" + suffix + " " #output
        command += "-V -D " #-V needed to generate binary, -D to tell the validator that it's HLSL code.
        command += file 
        os.system(command) 

if __name__ == '__main__':
    main()