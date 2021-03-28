import { spawn, exec } from 'child_process';
import split2 from 'split2';
import fs from 'fs';
import chalk from 'chalk';
import { logger } from './logger';

export namespace configure {
    export enum Platform {
        NONE,
        LINUX,
        WINDOWS,
        MACOS,
    }
    export interface Options {
        project: string;
        platform: string;
        generator: string;
        clean: boolean;
    }

    function isDir(path: string): boolean {
        return fs.existsSync(path) && fs.statSync(path).isDirectory();
    }

    export function clean(rootDir: string) {
        logger.info('Starting cleanup');

        // Get all the binaries from the bin folder & remove them
        const files = fs.readdirSync(`${rootDir}\\bin`).filter((file) => file.search(/Engine_*.*/) >= 0);
        files.forEach((file) => {
            fs.unlink(`${rootDir}\\bin\\${file}`, (err) => {
                if (err) throw Error('Failed to remove file');
                logger.info(`Removed ${file}`);
            });
        });

        // remove object files, pdb, ilk etc..
        if (isDir(`${rootDir}\\source\\obj`)) {
            exec(`rd -r ${rootDir}\\source\\obj /s /q`, (err) => {
                if (err) logger.error(err);
            });
        }

        // remove static libraries
        if (isDir(`${rootDir}\\source\\lib`)) {
            exec(`rd -r ${rootDir}\\source\\lib /s /q`, (err) => {
                if (err) logger.error(err);
            });
        }

        const ignore = ['external_libs', 'node_modules', 'unit_test', 'shaders', 'scripts', 'obj', 'premake5'];
        // remove all vcxproj related files
        let source = fs.readdirSync(`${rootDir}\\source`).filter((value) => value.search(/\..+/) < 0);
        source = source.filter((el) => {
            return ignore.indexOf(el) < 0;
        });

        source.forEach((dir) => {
            const files = fs
                .readdirSync(`${rootDir}\\source\\${dir}`)
                .filter((value) => value.search(/[A-Za-z].+\.vcxproj/) >= 0);

            files.forEach((file) => {
                fs.unlink(`${rootDir}\\source\\${dir}\\${file}`, (err) => {
                    if (err) throw Error('Failed to remove file');
                    logger.info(`Removed ${file}`);
                });
            });
        });

        logger.info(source);
    }

    export async function configure(rootDir: string, options: Options) {
        let premake = `${rootDir}\\premake5`;
        if (!fs.existsSync(premake)) {
            premake = `${rootDir}/source/premake5`;
            if (!fs.existsSync(`${premake}.exe`)) {
                throw 'Failed to find premake5';
            }
        }
        logger.info(premake);

        if (options.clean) {
            await clean(rootDir);
        }

        logger.info(options);

        const proc = spawn(premake, [
            `--file=${premake}.lua`,
            `--project=${options.project}`,
            `--platform=${options.platform}`,
            `${options.generator}`,
            options.clean ? 'clean' : '',
        ]);
        logger.logProcess(proc);
    }
}
