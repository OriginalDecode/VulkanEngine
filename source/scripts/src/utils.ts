import fs from 'fs';

export namespace utils {
  // returns the files in given directory - not sub folders?
  export async function getFiles(dirPath: string): Promise<Array<string>> {
    return new Promise((resolve, reject) => {
      return fs.readdir(dirPath, 'utf8', (err, files) => (err ? reject(err) : resolve(files)));
    });
  }
}