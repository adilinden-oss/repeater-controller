
The Repeater Controller
======================

This is a PC based repeater controller which relies on the IRLP board as
hardware interface with the radio.

History
------
The original code was a single source file, ht-ct.c, provided by David Cameron 
VE7LTD on the no longer existing IRLP-controller Yahoo group. The purpose of 
the group was to foster the creation of an IRLP compatible PC based repeater 
controller.

I took this code in 2004 and added functionality, such as courtesy beep and
PA compartment fan control. I also added support for controlling the various
I/O pins of the IRLP hardware board. At the time these functions were not
provided by the IRLP distribution.

In 2005 Randy Hammock KC6HUR took my work on this and added the irlpdev patch
provided by David Comaeron. The IRLP system has deprecated the irlp-port 
device with the release of the 2.6 kernel. The irlpdev driver makes use of 
the kernel parport driver.

As of 2013 I have performed a major cleanup of the code. All of the port
control bits have been placed into a library that is now shared between the
repeater and port control binaries. I've removed all legacy irlp-port parts
as they should be no longer relevant for a modern Linux system. Finally the
dedicated port control binaries have disappeared and been replaced by the
portctl function which now takes a single or multiple commands as command
line arguments.

Contents
--------
The repeater directory contains the sources for the actual repeater
controller. The repeater binary is the executable that reads inputs from
the IRLP board and controls the output according to the timing parameters
and what not defined in the source file. The portctl executable allows
for manual manipulation of the port pins to key or unkey the repeater.
Note that the repeater process may not be aware of manual portctl commands.

The cwid direcotry contains a number of helpers for the creation of
courtesy tones and cw id. These binaries create PCM waveforms and utilize
the ALSA or OSS sound system to output these tones. ALSA is used by
default as it allows for mixing of multiple sound sources.

