import { ChildProcess } from 'child_process';
import { logger } from './logger';
import { Builder, BuildOptions } from './builder';
import { WindowsBuilder } from './windowsBuilder';
import { LinuxBuilder } from './linuxBuilder';
import { MacBuilder } from './macBuilder';

function build(options: BuildOptions) {
    let builder: Builder;
    options.source = process.cwd() + '/source/';
    if (options.platform === 'windows') builder = new WindowsBuilder(options);
    else if (options.platform === 'osx') builder = new MacBuilder(options);
    else if (options.platform === 'linux') builder = new LinuxBuilder(options);
    else throw Error('No builder found');
    builder!.build();

    let warnings = 0;
    let errors = 0;
    logger.on('warning', () => {
        warnings++;
    });
    logger.on('error', () => {
        errors++;
    });
    logger.on('exit', (data: any) => {
        logger.info({ warnings, errors });
        if (data === 0) logger.success('Compile finished successfully!');
        else logger.critical('Failed to compile!');
    });
}

export { build };
