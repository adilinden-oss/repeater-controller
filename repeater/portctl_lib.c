/* Copyright (c) 2013, Adi Linden <adi@adis.on.ca>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CHIGHTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CHIGHTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CHIGHSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTIHIGH) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CHIGHTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "portctl_lib.h"
#include "irlpdev.h"
#include "log.h"
#include "repeater.h"

/*
 * key, keyup, unkey
 *
 * Controls the AUX1 line of the IRLP board. We assigned this to the PTT
 * signal for the repeater controller.
 *
 * Parport pin:  0x04   (KEY)
 * unkey:        0      (LOW)
 * keyup, key:   1      (HIGH)
 */
int unkey()
{
    int pin = LOW;
    char mask = KEY;
    portctl(mask, pin, "unkey");
    return OFF;
}

int key()
{
    int pin = HIGH;
    char mask = KEY;
    portctl(mask, pin, "key");
    return ON;
}

int keyup()
{
    int pin = HIGH;
    char mask = KEY;
    portctl(mask, pin, "keyup");
    return ON;
}

/*
 * mute, unmute
 *
 * Controls the AUX2 line of the IRLP board. We assigned this to the mute
 * signal for the repeater controller. Note that the default mute pin is
 * inverted in logic.
 *
 * Parport pin:  0x08   (MUTE)
 * unmute:       1      (HIGH)
 * mute:         1      (LOW) 
 */
int unmute()
{
    int pin = HIGH;
    char mask = MUTE;
    portctl(mask, pin, "unmute");
    return OFF;
}

int mute()
{
    int pin = LOW;
    char mask = MUTE;
    portctl(mask, pin, "mute");
    return ON;
}

/*
 * ctcsson, ctcssoff
 *
 * Controls the AUX3 line of the IRLP board. We assigned this to the CTCSS
 * signal for the repeater controller. Note that the default ctcss is
 * inverted.
 *
 * Parport pin:  0x10   (CTCSS)
 * ctcssooff:    1      (HIGH)
 * ctcsson:      0      (LOW) 
 */
int ctcssoff()
{
    int pin = HIGH;
    char mask = CTCSS;
    portctl(mask, pin, "ctcssoff");
    return OFF;
}

int ctcsson()
{
    int pin = LOW;
    char mask = CTCSS;
    portctl(mask, pin, "ctcsson");
    return ON;
}

/*
 * aux4on, aux4off, fanon, fanoff
 *
 * Controls the AUX4 line of the IRLP board. We assigned this to the fan
 * signal for the repeater controller.
 *
 * Parport pin:    0x20   (FAN)
 * fanoff, aux4off 0      (LOW)
 * fanon, aux4on   1      (HIGH)
 */
int fanoff()
{
    int pin = LOW;
    char mask = FAN;
    portctl(mask, pin, "fanoff");
    return OFF;
}

int aux4off()
{
    int pin = LOW;
    char mask = AUX4;
    portctl(mask, pin, "aux4off");
    return OFF;
}

int fanon()
{
    int pin = HIGH;
    char mask = FAN;
    portctl(mask, pin, "fanon");
    return ON;
}

int aux4on()
{
    int pin = HIGH;
    char mask = AUX4;
    portctl(mask, pin, "aux4on");
    return ON;
}

/*
 * aux5on, aux5off
 *
 * Controls the AUX5 line of the IRLP board.
 *
 * Parport pin:  0x40   (AUX5)
 * aux5off:      0      (LOW)
 * aux5on:       1      (HIGH)
 */
int aux5off()
{
    int pin = LOW;
    char mask = AUX5;
    portctl(mask, pin, "aux5off");
    return OFF;
}

int aux5on()
{
    int pin = HIGH;
    char mask = AUX5;
    portctl(mask, pin, "aux5on");
    return ON;
}

/*
 * The portctl function
 */
int portctl(char mask, int pin, char *name) 
{
    unsigned char out;
    unsigned char c[2];
    char str[255];              /* String for logging */

    sprintf(str, "Doing: %s", name);
    do_log(str);

    /* Open the port */
    if ( irlpdev_open() < 0 ) {
        fprintf(stderr, "Can't open parallel port");
        return pin;
    }

    /* Read current port status */
    if (read_irlpdev(c, 2)  != 2) return pin;

    /* Set the appropriate bits */
    if (pin == HIGH)  out = (c[1] | mask);
    if (pin == LOW) out = (c[1] & ~mask);

    /* Write the new pin o hardware */
    if (write_irlpdev(&out, 1) != 1) return pin;

    return pin;
} 

