import fs from 'fs';
import { spawn, ChildProcess } from 'child_process';
import { logger } from './logger';
import { Builder, BuildOptions } from './builder';

export class LinuxBuilder extends Builder {
    build(): ChildProcess {
        const proc = spawn('make', ['-C', this._buildOptions.source, `config=${this.getConfig(this._buildOptions)}`]);
        logger.logProcess(proc!);
        return proc;
    }
}
