import chalk from 'chalk';
import { ChildProcess } from 'child_process';
import { EventEmitter } from 'events';
import fs from 'fs';
import split2 from 'split2';
import util from 'util';

class Logger {
    constructor() {
        if (!fs.existsSync('./buildlogs')) fs.mkdirSync('./buildlogs');
        this.outStream = fs.createWriteStream('buildlogs/latest.log', { flags: 'w+' });
        // this.warningEvent = new Event('warning');
        this.eventEmitter = new EventEmitter();
        // this.errorEvent = new CustomEvent('error');
        // this.exitEvent = new CustomEvent('exit');
    }

    on(event: string, handler: any) {
        this.eventEmitter.on(event, handler);
    }

    logProcess(proc: ChildProcess) {
        proc.stdout!.pipe(split2()).on('data', (data: any) => {
            const string = data.toString();
            if (string.indexOf('warning') !== -1) {
                logger.warning(string);
                this.eventEmitter.emit('warning');
            } else if (string.indexOf('error') !== -1) {
                logger.error(string);
                this.eventEmitter.emit('error');
            } else if (string.indexOf('message') !== -1) {
                logger.message(string);
            } else {
                logger.info(string);
            }
        });

        proc.stderr!.pipe(split2()).on('data', (data: any) => {
            const string = data.toString();
            if (string.indexOf('warning') !== -1) {
                logger.warning(string);
                this.eventEmitter.emit('warning');
            } else if (string.indexOf('error') !== -1) {
                logger.error(string);
                this.eventEmitter.emit('error');
            } else if (string.indexOf('message') !== -1) {
                logger.message(string);
            } else {
                logger.info(string);
            }
        });

        proc.on('exit', (data: any) => {
            this.eventEmitter.emit('exit', data);
        });
    }

    info(string: any, ...args: any[]) {
        const str = `INFO: ${util.format(string, ...args)}`;
        console.log(chalk.white(str));
        this.outStream.write(str + `\n`);
    }

    error(string: any, ...args: any[]) {
        const str = `ERROR: ${util.format(string, ...args)}`;
        console.log(chalk.red(str));
        this.outStream.write(str + `\n`);
    }

    critical(string: any, ...args: any[]) {
        const str = `ERROR: ${util.format(string, ...args)}`;
        console.log(chalk.red.bold(str));
        this.outStream.write(str + `\n`);
    }

    warning(string: any, ...args: any[]) {
        const str = `WARNING: ${util.format(string, ...args)}`;
        console.log(chalk.yellow(str));
        this.outStream.write(str + `\n`);
    }

    success(string: any, ...args: any[]) {
        const str = `SUCCESS: ${util.format(string, ...args)}`;
        console.log(chalk.green(str));
        this.outStream.write(str + `\n`);
    }

    message(string: any, ...args: any[]) {
        const str = `MESSAGE: ${util.format(string, ...args)}`;
        console.log(chalk.blueBright(str));
        this.outStream.write(str + `\n`);
    }

    private outStream: fs.WriteStream;
    private eventEmitter: EventEmitter;
}

const logger = new Logger();

export { logger };
