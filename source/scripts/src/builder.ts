import { ChildProcess } from 'child_process';

export interface BuildOptions {
    platform: string;
    solution: string | null;
    verbosity: string;
    config: string;
    rebuild: boolean;
    source: string;
}

export abstract class Builder {
    constructor(buildOptions: BuildOptions) {
        this._buildOptions = buildOptions;
    }
    abstract build(): ChildProcess;

    isDebug(config: string): boolean {
        return config.toLowerCase() === 'debug';
    }

    getConfig(options: BuildOptions): string {
        return this.isDebug(options.config) ? `debug_${options.platform}` : `release_${options.platform}`;
    }

    protected _buildOptions: BuildOptions;

    get buildOptions() {
        return this._buildOptions;
    }
}
