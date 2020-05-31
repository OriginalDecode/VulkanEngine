import { spawn, exec } from 'child_process';
import split2 from 'split2';
import fs from 'fs';
import chalk from 'chalk';

interface BuildOptions {
  platform: string;
  solution?: string; // only valid on windows
  verbosity: string;
  config: string;
}

function buildLinux(rootDir: string) {
  //
}

function buildWindows(rootDir: string, options: BuildOptions) {
  const programFilesx86 = process.env['ProgramFiles(x86)'];
  const version = ['Community', 'Professional', 'Enterprise', 'BuildTools'];
  let path = `${programFilesx86!}\\Microsoft Visual Studio`;
  const visualStudio = fs.readdirSync(path);
  let compiler: string = '';
  // Would be nice to see if I could break this down in anyway
  if (visualStudio.indexOf('2019') !== -1) {
    path = `${path}\\2019`;
    version.forEach((version) => {
      const pathVersion = `${path}\\${version}`;
      if (fs.existsSync(pathVersion)) {
        if (fs.existsSync(`${pathVersion}\\MSBuild\\Current`)) {
          compiler = `${pathVersion}\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe`;
        } else {
          const dir = fs.readdirSync(`${pathVersion}`);
          dir.forEach((subDir) => {
            if (fs.existsSync(`${pathVersion}\\MSBuild\\${subDir}\\Bin\\`)) {
              compiler = `${pathVersion}\\MSBuild\\${subDir}\\Bin\\amd64\\MSBuild.exe`;
            }
          });
        }
      }
    });
  }
  console.log(`Compiler found!\ncompiler: ${chalk.green(compiler)}`);

  const proc = spawn(compiler, [
    options.solution!,
    '/p:GenerateFullPaths=true',
    `/p:Configuration=${options.config}`,
    '/p:Platform=Windows',
    '/t:rebuild',
    `-v:${options.verbosity}`,
  ]);
  proc.stdout.pipe(split2()).on('data', (data: any) => {
    const string = data.toString();
    if (string.indexOf('warning') !== -1) {
      console.log(chalk.yellow(string));
    } else if (string.indexOf('error') !== -1) {
      console.log(chalk.red(string));
    } else if (string.indexOf('message') !== -1) {
      console.log(chalk.blueBright(string));
    }
  });

  proc.stderr.pipe(split2()).on('data', (data: any) => {
    console.error(chalk.red.underline.bold(data.toString()));
  });
}

function build(rootDir: string, options: BuildOptions) {
  if (options.platform === 'win32') {
    buildWindows(rootDir, options);
  } else if (options.platform === 'linux') {
    buildLinux(rootDir);
  }
}

export { build, BuildOptions };
