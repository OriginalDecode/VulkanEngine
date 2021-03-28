import { utils } from './utils';
import { spawn } from 'child_process';
import { logger } from './logger';
import fs from 'fs';

const types = ['vs', 'ps', 'gs', 'ds', 'hs', 'ms', 'cs', 'as'];
const versions = ['6_0', '6_1', '6_2', '6_3', '6_4', '6_5'];
const typeVersions: string[] = [];
types.forEach((type) => {
    versions.forEach((version) => {
        typeVersions.push(`${type}_${version}`);
    });
});

const validate = (arg: string, array: string[]) => {
    return array.indexOf(arg) >= 0;
};

function getShaderArgs(inputFile: string): string[] {
    const args = fs
        .readFileSync(inputFile, 'utf8')
        .split('\n')[0]
        .replace('//', '')
        .split(/(?=\-)/)
        .slice(1);

    args.forEach((arg, index, arr) => {
        if (arg.indexOf('-T') >= 0) {
            // validate types
            if (!validate(arg.replace('-T', '').replace(' ', ''), typeVersions)) {
                throw Error(`Failed to validate shader type. ${arg} is invalid`);
            }
        }
        arr[index] = arg.replace(/\s/g, '');
    });
    return args;
}

async function compileShaders() {
    const cwd = process.cwd();
    const config = {
        compiler: `${cwd}\\dependencies\\dxc\\bin\\x64\\dxc.exe`,
        sourceFolder: `${cwd}\\source\\shaders\\`,
        outputFolder: `${cwd}\\bin\\Data\\Shaders\\`,
        options: ['-spirv', '-Zpr', '-fspv-target-env=vulkan1.1'],
    };

    let shaders = new Array<string>();
    await utils
        .getFiles(config.sourceFolder)
        .then((resolve) => {
            shaders = [...resolve];
            logger.info(shaders);
        })
        .catch((reject) => {
            logger.error(reject);
        });

    if (shaders.length <= 0) {
        throw Error('No shaders');
    }

    shaders.forEach((shader: string) => {
        const inputFile = `${config.sourceFolder}${shader}`;
        const args = getShaderArgs(inputFile);
        const proc = spawn(config.compiler, [
            ...args,
            ...config.options,
            `-Fo${config.outputFolder}${shader}`,
            `${inputFile}`,
        ]);
        logger.logProcess(proc);
    });
}

export { compileShaders };
