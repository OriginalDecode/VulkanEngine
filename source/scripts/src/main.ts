import { compileShaders } from './compile_shaders';

import { configure } from './configure_builds';
import os from 'os';
import { utils } from './utils';

const args = process.argv.slice(2);

if (args.indexOf('-test') >= 0) {
  configure.clean(process.cwd());
}

if (args.indexOf('-compile_shaders') !== -1) {
  compileShaders(process.cwd());
}

if (args.indexOf('-configure') !== -1) {
  console.log(args);

  const project = args[args.indexOf('-p') + 1];
  const generator = args[args.indexOf('-g') + 1];
  const clean = args.indexOf('-clean') != -1;
  const platform = os.platform();

  if (!project || !generator || !platform) {
    throw 'Invalid params';
  }

  // console.log({ project, generator, platform, clean });

  configure.configure(process.cwd(), {
    project: project,
    platform: platform == 'win32' ? 'windows' : platform,
    clean: clean,
    generator: generator,
  });
}
