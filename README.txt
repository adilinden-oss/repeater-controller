
The Repeater Controller
======================

This is a PC based repeater controller which relies on the IRLP board as
hardware interface with the radio.

Credit
------
The repeater-controller is derived from previous work done by:
- David Cameron VE7LTD <http://www.irlp.net>
- Randy Hammock KC6HUR

History
------
The original code was a single source file, ht-ct.c, provided by David Cameron 
VE7LTD on the no longer existing IRLP-controller Yahoo group. The purpose of 
the group was to foster the creation of an IRLP compatible PC based repeater 
controller. The code included a functional repeater controler and functions to
control the ILRP interface board.

I took this code in 2004 and added functionality, such as courtesy beep and
PA compartment fan control. I also added support for directly controlling the
various I/O pins of the IRLP hardware board based on the functions provided in
ht-ct.c. At the time these functions were not provided by the IRLP distribution.

In 2005 Randy Hammock KC6HUR took the work and added the irlpdev patch provided
by David Comaeron. The IRLP system has deprecated the irlp-port  device with the 
release of the 2.6 kernel. The irlpdev driver makes use of the kernel parport 
driver.

As of 2013 I have performed a major cleanup of the code. All of the port control
functions have been moved into a single library file. I've removed the legacy
irlp-port parts. Finally the dedicated port control binaries have been replaced
with a new portctl binary which takes the desired action as a command line
argument.

Contents
--------
The repeater directory contains the sources for the actual repeater controller.
The repeater binary is the executable that reads inputs from the IRLP board and
controls the output according to the timing parameters defined in the source
file. The portctl executable allows for manual manipulation of the port pins to
key or unkey the repeater. Note that the repeater process may not be aware of
manual portctl commands.

The cwid direcotry contains a number of helpers for the creation of
courtesy tones and cw id. These binaries create PCM waveforms and utilize
the ALSA or OSS sound system to output these tones. ALSA is used by
default as it allows for mixing of multiple sound sources.

Features
--------
- Control via IRLP board
- IRLP aware and compatible
- Hangtimer
- Courtesy tone
- CW ID

ToDo
----
Some of the things I'd still like to do

- Man pages and other documentation
- Configurable timing parameters via command line or config file

