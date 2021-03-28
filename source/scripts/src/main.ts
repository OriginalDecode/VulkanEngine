import { compileShaders } from './compile_shaders';
import { build } from './build';
import { BuildOptions } from './builder';
import { configure } from './configure_builds';
import os from 'os';
import fs from 'fs';
import * as ghd from './githubDownloader';
import { logger } from './logger';
import unzipper from 'unzipper';

const args = process.argv.slice(2);

function getArgValue(arg: string): string | null {
    if (args.indexOf(arg) !== -1) {
        return args[args.indexOf(arg) + 1];
    }
    return null;
}

if (args.indexOf('-test') >= 0) {
    configure.clean(process.cwd());
}

if (args.indexOf('-compile_shaders') !== -1) {
    compileShaders();
}

function getPlatform(platform: string): string {
    if (platform === 'win32') return 'windows';
    if (platform === 'darwin') return 'osx';
    return platform;
}

if (args.indexOf('-configure') !== -1) {
    const project = args[args.indexOf('-p') + 1];
    const generator = args[args.indexOf('-g') + 1];
    const clean = args.indexOf('-clean') != -1;
    const platform = os.platform();

    configure.configure(process.cwd(), {
        project: project,
        platform: getPlatform(platform),
        clean: clean,
        generator: generator,
    });
}

if (getArgValue('-build')) {
    const options: BuildOptions = {
        verbosity: getArgValue('-v')!,
        config: getArgValue('-c')!,
        rebuild: getArgValue('-rebuild') !== null,
        solution: getArgValue('-build'),
        platform: getPlatform(os.platform()),
        source: '/source/',
    };
    build(options);
}

if (args.includes('-dependencies')) {
    if (!fs.existsSync('./dependencies/')) {
        fs.mkdirSync('./dependencies');
    }
    if (!fs.existsSync('./dependencies/downloads')) {
        fs.mkdirSync('./dependencies/downloads');
    }

    const promise = ghd.getLatestRelease('https://api.github.com/repos/microsoft/DirectXShaderCompiler/releases');
    Promise.all([promise])
        .then((results) => {
            results.forEach((result) => {
                const latest: any = result;

                if (!fs.existsSync('./dependencies/dxc')) {
                    fs.mkdirSync('./dependencies/dxc');
                }
                const stream = fs.createWriteStream(`./dependencies/downloads/${latest.tag}.zip`);
                const promise = ghd.download(latest.assets[0].browserDownloadUrl, stream);
                Promise.all([promise])
                    .then((values) => {
                        logger.info(`Downloaded DXC ${latest.tag}`);
                        fs.createReadStream(`./dependencies/downloads/${latest.tag}.zip`).pipe(
                            unzipper.Extract({ path: './dependencies/dxc' })
                        );
                    })
                    .catch((reason) => {
                        logger.error(`Failed: ${reason}`);
                    });
            });
        })
        .catch((reason) => {
            logger.error(`Failed: ${reason}`);
        });
}
