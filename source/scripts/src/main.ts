import { compileShaders } from './compile_shaders';
import { build } from './build';
import { BuildOptions } from './builder';
import { configure } from './configure_builds';
import { spawn, ChildProcess, spawnSync } from 'child_process';
import os from 'os';
import * as https from 'https';

const args = process.argv.slice(2);

if (args.indexOf('-test') >= 0) {
    configure.clean(process.cwd());
}

if (args.indexOf('-compile_shaders') !== -1) {
    compileShaders(process.cwd());
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

    if (!project || !generator || !platform) {
        throw 'Invalid params';
    }

    configure.configure(process.cwd(), {
        project: project,
        platform: getPlatform(platform),
        clean: clean,
        generator: generator,
    });
}

function getArgValue(arg: string): string | null {
    if (args.indexOf(arg) !== -1) {
        return args[args.indexOf(arg) + 1];
    }
    return null;
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

if (args.includes('-dxc')) {
    const cwd = process.cwd();
    const src = cwd + '/source/external_libs/dxc';
    const hct = cwd + '/source/external_libs/dxc/utils/hct';
    const proc = spawnSync(`${hct}/hctstart.cmd`, ['-x64', src, `${src}/bin/`]);

    if (!proc.error) {
        const compile = spawnSync(`${hct}/hctbuild.cmd`, ['-buildoutdir', `${src}/bin/`]);
        console.log(compile.stdout.toString());
        if (!compile.error) {
            console.log('Finished without error');
        }
    }
}

if (args.includes('-dependencies')) {
    const cwd = process.cwd();
    // download the dependencies
    try {
        async () => {
            let total: number = 0;
            let location: string | undefined = '';
            https.get(
                'https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.5.2010/dxc_2020_10-22.zip',
                (res) => {
                    location = res.headers['location'];
                }
            );

            const req = https.get(location, (res) => {
                console.log(res.statusCode);
                console.log(res.statusMessage);
                console.log(res.rawHeaders);
            });
            req.on('response', (data) => {
                total = parseInt(data.headers['content-length']!);
                console.log('len ' + data.headers['content-length']);
            });

            req.on('end', () => {
                console.log('closed connection');
            });

            let current = 0;
            req.on('data', (chunk) => {
                current += parseInt(chunk.headers['content-length']);
                console.log('len ' + chunk.headers['content-length']);
            });
        };
    } catch (e) {
        console.log(e);
    }
}
