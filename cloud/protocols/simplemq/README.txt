

                        SimpleMQ C Client library

Simple Message Queue (SMQ) is an easy to use "Internet of Things" IoT
/ M2M "Machine-to-Machine" publish subscribe connectivity protocol
designed and optimized for embedded systems.


------- License:
The source code is released under the Eclipse Public License
http://en.wikipedia.org/wiki/Eclipse_Public_License
https://www.eclipse.org/legal/epl-v10.html

You may compile a Program licensed under the EPL without modification
and commercially license the result in accordance with the terms of
the EPL: https://eclipse.org/legal/eplfaq.php


------- Documentation:
https://realtimelogic.com/ba/doc/?url=SMQ.html
https://realtimelogic.com/ba/doc/en/C/reference/html/group__SMQClient.html


------- Compiling:

Build instructions for Windows, Linux, and embedded systems:

The VcMake directory contains Visual Studio build files. You can
download the compiler here:
http://www.visualstudio.com/en-us/downloads#d-2010-express

The included Makefile compiles the library and the example program on
any Linux computer.

You can compile the code for the supported embedded systems. See
src/arch/README.txt for details.

The example code m2m-led.c requires porting to your embedded board's
LED(s).


------- Downloading the broker (SMQ server):

The example program connects to our online broker. After testing with
our online broker, change the URL in the C code, recompile, and test
with your own broker.

Broker: https://makoserver.net/smq-broker/


------- Additional libraries:

SimpleMQ can be used in combination with our JSON library:
https://realtimelogic.com/products/json/
