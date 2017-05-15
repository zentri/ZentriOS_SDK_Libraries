# emdrv test runner

This folder contains the test runner for emdrv. It depends on the test framework in `s026_utils/runtest`.

## Running tests

In order to run tests, change working directory to this folder (`emdrv/test/`).

### Prerequisites

In order to use the test runner, the following environment variables need to be defined:

Environment var      | Value
-------------------- | -----
`SL_COMMANDER_PATH`  | Path to the `eACommander` or `commander` binary used for flashing and reading SWO output.

Example .bashrc: `export SL_COMMANDER_PATH=/opt/commander-0.11/commander`  
Or define it inline on the command line: `SL_COMMANDER_PATH="C:/app/energyAware Commander/2.96/eACommander.exe" python runtest.py ...`

### Usage

```
#!text
usage: runtest.py [-h] [--kit KITNAME] [--serial JLINK_SERIAL]
                  [--test TESTCASE] [--module MODULE] [--genonly]
                  [--buildonly] [--compiler {armgcc,iar}] [--debug]
                  [--release] [--serialport SERIALPORT] [--commander]

Run emdrv regression tests.

optional arguments:
  -h, --help            show this help message and exit
  --kit KITNAME         Which kit to run on. Default is EFM32GG_STK3700.
  --serial JLINK_SERIAL
                        Connect to this jlink serial. If you only have one kit
                        connected, you can ignore this option.
  --test TESTCASE       Only run a specific test.
  --module MODULE       Only run tests for a specific module.
  --genonly             Only generate project.
  --buildonly           Only build project, don't run on device.
  --compiler {armgcc,iar}
                        Specify the compiler to use.
  --debug               Build in debug mode.
  --release             Build in release mode (default).
  --serialport SERIALPORT
                        Serial port to read results from.
  --commander           Use Simplicity Commander instead of eACommander.
```

### Examples

```
./runtest.py --kit EFM32TG_STK3300 --compiler iar
./runtest.py --kit EFM32TG_STK3300 --module SPIDRV
./runtest.py --kit EFM32TG_STK3300 --buildonly --module DMADRV --test DMADRV_NATIVE
./runtest.py --kit EFM32ZG_STK3200 --serialport COM3 --debug
```

### Disambiguating multiple kits

If more than a single kit is connected to the computer, and `--serial` is not supplied, runtest.py will use `commander -v` to list all connected emulators and JLink serial numbers. It will then connect to each device until it finds one that matches the part name of the selected kit. This serial number is then used for subsequent calls to the Commander.

## Defining a test

Tests are discovered by the test runner by looking for files named `*_test.json` in the `<module>/test/` folders. Each file contains the test description for one binary that will be compiled and flashed with the selected toolchain. Depending on the selected kit, SWO or serial output will be used. See the [runtest documentation in s026_utils](../../../s026_utils/browse/runtest/) for information on how to add new kits, as well as the specification of the test JSON format.