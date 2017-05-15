#!/usr/bin/env python

# This script is used for running the emdrv regression test suite.
#
# Run this script with --help to see the command line options.
#
# To add a kit to the regression test suite, create a json file in ../../util/runtest/boards/.
# This file must be named <kitname>.json
# To add a test to the suite, create a json file in the test/ folder of the module
# This file must be named <module>_test.json. Typically a C file with the
# same name will also be used, but it is not required.

import os, sys, argparse, re, json, fnmatch

# Import meta util script
import meta_util

# Import runtest
emdrv_version = meta_util.currentPathToModuleVersion()
util_path = os.path.join(meta_util.REPO_ROOT_DIR, meta_util.moduleSymbolToPath('EMTOOL', emdrv_version))
runtest_path = '{}/runtest'.format(util_path)
sys.path.insert(0, os.path.abspath(runtest_path))
from TestReporter import TestReporter
from TestRunner import TestRunner
from TestJsonParser import TestJsonParser
from Test import Test
from eACommander import eACommander
from SimplicityCommander import SimplicityCommander
from GenprojectBuilder import GenprojectBuilder

# All modules that can be run
available_modules = [
  'AESDRV',
  'CAPSENSE',
  'CRYPTODRV',
  'DMADRV',
  'ECC',
  'EZRADIODRV',
  'GPIOINTERRUPT',
  'NVM',
  'RSADRV',
  'RTCDRV',
  'SHADRV',
  'SLEEP',
  'SPIDRV',
  'TEMPDRV',
  'TRACE',
  'UARTDRV',
  'USTIMER',
  'CPP'
]

# Modules that should automatically run in regression test
auto_run_modules = [
    'AESDRV',
    'DMADRV',
    'GPIOINTERRUPT',
    'RTCDRV',
    'SLEEP',
    'NVM',
    'SPIDRV',      # requires loopback connection on DK
    'TEMPDRV',
    'UARTDRV',     # requires loopback connection on DK
    'USTIMER'
]

def append_test_json(path, files):
  """
  Find JSON test specification files for a peripheral in a directory

  Parameters:
  path       -- The directory we are looking in
  files      -- List of files in this directory

  Returns: List of paths to test JSON files.
  """
  paths = []
  for file in files:
    # File name rule is <peri>_test.json or <peri>_<somefunction>_test.json
    if file.endswith("_test.json"):
      paths.append(os.path.join(path,file))

  return paths

def find_test_json(path):
  """
  Find all JSON test specification files on the given path mathcing the test name

  Parameters:
  path       -- Path to look for test JSON

  Returns: List of paths to test JSON files.
  """
  paths = []
  for root, dirs, files in os.walk(path):
    paths += append_test_json(root, files)
  return paths


def generate_test_specs(module_name, test_name, kit_name, reporter, skip_modules = [], skip_tests = []):
  """
  Generate test specification on the format
  List of: Test {
    name: str
    defines: []
    includes: []
    src: []
    genproject_template: str
  }

  Parameters:
  module_name  -- Name of the module
  test_name    -- Name of the test
  kit_name     -- The kit we are running the test for
  reporter     -- TestReporter instance for reporting success/failure
  skip_modules -- List of modules to skip
  skip_tests   -- List of tests to skip

  Returns: List of Test instances
  """
  # By default, run all auto_run_modules
  modules = auto_run_modules

  if test_name is not None:
    print('Only processing test: {}'.format(test_name))
    # If we have a specific test name given, search among all modules
    modules = available_modules

  if module_name is not None:
    print('Only processing module: {}'.format(module_name))
    # If a specific module is given, only search in this module
    modules = [module_name]


  tests = []

  test_file_parser = TestJsonParser(kit_name, reporter)
  test_file_parser.set_default_genproject_template('./templates/emdrvtest.xml')
  test_file_parser.add_custom_src_path('emlib', '##em-path-emlib##/src/')
  test_file_parser.add_custom_src_path('emdrv', '##em-path-emdrv##/')

  for module in modules:
    files = find_test_json('../{}/test'.format(module.lower()))

    if module in skip_modules:
      reporter.report_skipped(module, "All tests for this module disabled on command line.")
      continue

    tests += test_file_parser.parse_files(files, module, test_name, skip_tests)

  return tests

if __name__ == '__main__':
  """
  Main script entry point
  """
  parser = argparse.ArgumentParser(description='Run emdrv regression tests.', epilog="Example: ./runtest.py --kit EFM32TG_STK3300 --compiler iar")
  parser.add_argument('--kit', dest='kitname', default='EFM32GG_STK3700', help='Which kit to run on. Default is EFM32GG_STK3700.')
  parser.add_argument('--serial', dest='jlink_serial', help='Connect to this jlink serial. If you only have one kit connected, you can ignore this option.')
  parser.add_argument('--ip', dest='jlink_ip', help='Connect to this JLink IP.')
  parser.add_argument('--test', dest='testcase', default=None, help='Only run a specific test.')
  parser.add_argument('--skip-tests', nargs='+', dest='skip_tests', default=[], help='Run tests except the ones listed.')
  parser.add_argument('--module', dest='module', default=None, help='Only run tests for a specific module.')
  parser.add_argument('--skip-modules', nargs='+', dest='skip_modules', default=[], help='Run tests for modules except the ones listed.')
  parser.add_argument('--genonly', dest='genonly', action='store_true', help='Only generate project.')
  parser.add_argument('--buildonly', dest='buildonly', action='store_true', help='Only build project, don\'t run on device.')
  parser.add_argument('--compiler', dest='compiler', choices=['armgcc', 'iar'], default='armgcc', help='Specify the compiler to use.')
  parser.add_argument('--debug', dest='debug', action='store_true', help='Build in debug mode.')
  parser.add_argument('--release', dest='release', action='store_true', default=True, help='Build in release mode (default).')
  parser.add_argument('--serialport', dest='serialport', default=None, help='Serial port to read results from.')
  parser.add_argument('--commander', dest='commander', action='store_true', default=True, help='DEPRECATED: Use Simplicity Commander.')
  parser.add_argument('--eacommander', dest='eacommander', action='store_true', help='Use energyAware Commander.')
  parser.add_argument('--verbose', dest='verbose', action='store_true', help='Verbose output.')
  parser.add_argument('--output-to-file', dest='dump_output', action='store_true', help='Write raw test output to file in the out/ directory. The file will be named <kit>_<compiler>_<test>.txt')
  parser.add_argument('--flashloader', dest='flashloader', default=None, help='Path to a custom flashloader to use.')
  args = parser.parse_args()

  # Create test reporter, using the kit name as name of the report
  reporter = TestReporter(args.kitname)

  if args.buildonly or args.genonly:
    # Only build project, don't flash and run
    commander = None
  elif args.eacommander:
    # Flash using energyAware Commander
    commander = eACommander(reporter, jlink_serial=args.jlink_serial, jlink_ip=args.jlink_ip, flashloader=args.flashloader, kit=args.kitname)
  else:
    # Flash using Simplicity Commander
    commander = SimplicityCommander(reporter, jlink_serial=args.jlink_serial, jlink_ip=args.jlink_ip, flashloader=args.flashloader)

  builder = GenprojectBuilder()
  builder.set_compiler(args.compiler)

  if args.genonly:
    # Only generate project, don't build it
    pass
  elif args.debug:
    # Build in debug mode
    builder.set_build_config('debug')
  else:
    # Build in release mode
    builder.set_build_config('release')

  builder.add_directory('cmsis', os.path.relpath(os.path.join(meta_util.REPO_ROOT_DIR, meta_util.moduleSymbolToPath('CMSIS', emdrv_version))))
  device_path = '{}/SiliconLabs/'.format(
    os.path.join(meta_util.REPO_ROOT_DIR,
                 meta_util.moduleSymbolToPath('DEVICE', emdrv_version))
  )
  builder.add_directory('device', os.path.relpath(device_path))
  builder.add_directory('emlib', os.path.relpath(os.path.join(meta_util.REPO_ROOT_DIR, meta_util.moduleSymbolToPath('EMLIB', emdrv_version))))
  builder.add_directory('unity', os.path.relpath(os.path.join(meta_util.REPO_ROOT_DIR, meta_util.moduleSymbolToPath('UNITY', emdrv_version))))
  builder.add_directory('emdrv', '..')

  masserase = True
  if args.kitname in ["EFM32LG_DK3650", "EFM32GG_DK3750", "EFM32WG_DK3850"]:
    # For some reason these kits sometimes fail to masserase
    masserase = False

  tester = TestRunner(
    kit = args.kitname,
    project_name = 'emdrvtest',
    reporter = reporter,
    builder = builder,
    commander = commander,
    serial_port = args.serialport,
    verbose = args.verbose,
    dump_output = args.dump_output,
    masserase = masserase
  )

  if not args.buildonly and not args.genonly:
    commander.set_device_name(tester.get_device_name())
    # Print kit info to screen for debug purposes
    commander.get_kit_info()

  # Get list of relevant tests for this kit
  tests = generate_test_specs(args.module, args.testcase, args.kitname, reporter, args.skip_modules, args.skip_tests)

  tester.run(tests)

  # Write XML report
  reporter.write_report('./out')

  print("--------------------------------------------------------------------------------")
  print("Human readable report:")
  reporter.print_statistics()
  reporter.print_failures()
