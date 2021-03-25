import fs from 'fs';
import { spawn, ChildProcess } from 'child_process';
import { logger } from './logger';
import { Builder } from './builder';

export class WindowsBuilder extends Builder {
    build(): ChildProcess {
        const compiler = this.getCompiler();
        if (!compiler) throw Error('Failed to find a compiler!');
        logger.info(`Compiler found! ${compiler}`);
        const proc = spawn(compiler, [
            this._buildOptions.source + this._buildOptions.solution!,
            '/p:GenerateFullPaths=true',
            `/p:Configuration=${this._buildOptions.config}`,
            `/p:Platform=${this._buildOptions.platform}`,
            '/t:build',
            `-v:${this._buildOptions.verbosity}`,
        ]);
        logger.logProcess(proc!);
        return proc;
    }

    private getCompiler(): string | undefined {
        const programFilesx86 = process.env['ProgramFiles(x86)'];
        const version = ['Community', 'Professional', 'Enterprise', 'BuildTools'];
        const path = `${programFilesx86!}\\Microsoft Visual Studio`;
        const visualStudio = fs.readdirSync(path);

        // This is the version of VS we support, we might build with GCC or Clang in the future
        if (!visualStudio.includes('2019')) return;
        logger.info('Visual Studio version found');
        let msbuild = '';
        version.forEach((version) => {
            const pathVersion = `${path}\\2019\\${version}`;

            if (!fs.existsSync(pathVersion)) return;
            logger.info('Visual Studio 2019 %s Selected', version);

            if (fs.existsSync(`${pathVersion}\\MSBuild\\Current`)) {
                msbuild = `${pathVersion}\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe`;
            }
            const dir = fs.readdirSync(`${pathVersion}`);
            dir.forEach((subDir) => {
                if (fs.existsSync(`${pathVersion}\\MSBuild\\${subDir}\\Bin\\`)) {
                    msbuild = `${pathVersion}\\MSBuild\\${subDir}\\Bin\\amd64\\MSBuild.exe`;
                }
            });
        });

        return msbuild;
    }
}
