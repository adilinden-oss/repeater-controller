/* Copyright (c) 2004, Adi Linden <adi@adis.on.ca>
 * All rights reserved.
 *
 * This code is derived from software contributed by Dave Cameron.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors may 
 *    be used to endorse or promote products derived from this software 
 *    without specific prior written permission.
 *    
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define VERSION     "20130106"

/* The parallel port input pins
 * 
 * Pin          Hex     Purpose     Active
 * ------------------------------------------
 * Reserved     0x01
 * Reserved     0x02
 * Reserved     0x04
 * Error        0x08    DTMF Q1     high
 * Select       0x10    DTMF Q2     high
 * Paper Out    0x20    DTMF Q3     high
 * Ack          0x40    DTMF Q4     high
 * Busy*        0x80    COS         high (pin low)
 *
 *    * the `Busy' pin is inverted in hardware
 *
 * All of the DTMF pins are held low when idle. This is the reason the DTMF
 * tone for `D' cannot be detected by the IRLP hardware.
 */
#define IRLPDEV_ERR     0x08
#define IRLPDEV_SEL     0x10
#define IRLPDEV_PAP     0x20
#define IRLPDEV_ACK     0x40
#define IRLPDEV_BUS     0x80

/* The parallel port output pins
 *
 * Pin      Hex     Purpose           Active
 * -------------------------------------------
 * D0       0x01    IRLPKEY (unused)  low
 * D1       0x02    IRLPKEY           high
 * D2       0x04    KEY   (AUX1)      high
 * D3       0x08    MUTE  (AUX2)      low
 * D4       0x10    CTCSS (AUX3)      low
 * D5       0x20    FAN   (AUX4)      high
 * D6       0x40    AUX5              high
 * D7       0x80    FORCEKEY (unused) high
 */
#define IRLPDEV_D0      0x01
#define IRLPDEV_D1      0x02
#define IRLPDEV_D2      0x04
#define IRLPDEV_D3      0x08
#define IRLPDEV_D4      0x10
#define IRLPDEV_D5      0x20
#define IRLPDEV_D6      0x40
#define IRLPDEV_D7      0x80

/* Function states */
#define OFF         0
#define ON          1

/* Port pin states */
#define LOW         0
#define HIGH        1

/* Repeater output port pin assignments */
#define IRLPKEY         IRLPDEV_D1
#define KEY             IRLPDEV_D2
#define MUTE            IRLPDEV_D3
#define CTCSS           IRLPDEV_D4
#define FAN             IRLPDEV_D5
#define AUX4            IRLPDEV_D5
#define AUX5            IRLPDEV_D6

