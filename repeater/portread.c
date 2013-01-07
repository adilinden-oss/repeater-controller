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

#include <string.h>
#include <stdio.h>
#include <time.h>
#include "irlpdev.h"
#include "repeater.h"

static char *usage =
    "Usage: portread CMD\n"
    "Read IRLP port state.\n"
    "   -b      binary representation\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2013, Adi Linden <adi@adis.ca>\n";

void print_irlp(unsigned char *c);
void print_byte(unsigned char *c);
void each_byte(unsigned char b);

int main(int argc, char *argv[])
{
    unsigned char c[2];          /* Returned string from parallel port */
    unsigned char s[2];          /* Saved string from parallel port */
    int binary = 0;              /* Flag for binary representation */
    struct timespec tim;         /* Timespec for the loop timer function */

    /* Get any optional command line args (start with -) */
    while (argc > 1 && *argv[1] == '-') {
        if (!strcmp(argv[1], "-b")) {
            binary = 1;
        }
        if (!strcmp(argv[1], "-h")) {
            fprintf(stderr, usage);
            return -1;
        }
        argc -= 1;
        argv += 1;
    }

    /* Opens the /dev/irlp-port device, read/write. This is the communication
     * Channel to the IRLP hardware from the software 
     */
    if(irlpdev_open() < 0 ) {
        fprintf(stderr, "Can't access parallel port");
        return(-1);
    }

    /* Sets default settings for the main variables */
    tim.tv_sec = 0;
    tim.tv_nsec = 5000000;

    while (1) {
        /* Reads the input and output bit from the port */
        if (read_irlpdev(c, 2) != 2)
            fprintf(stderr, "Can't read parallel port");

        /* Compare with saved state */
        if (memcmp(c, s, 2)) {
            if (binary)
                print_byte(c);
            else
                print_irlp(c);
            memcpy(s, c, 2);
        }

        /* Pace our processing */
        nanosleep(&tim, NULL);
    }

    return 0;
}


void print_irlp(unsigned char *c)
{
    unsigned char dtmf0;
    unsigned char dtmf1;
    unsigned char dtmf2;
    unsigned char dtmf3;
    unsigned char COS;
    unsigned char irlpkeyi;
    unsigned char irlpkey;
    unsigned char key;
    unsigned char mute;
    unsigned char ctcss;
    unsigned char fan;
    unsigned char aux4;
    unsigned char aux5;

    dtmf0 = (c[0] >> 3) & 0x01;
    dtmf1 = (c[0] >> 4) & 0x01;
    dtmf2 = (c[0] >> 5) & 0x01;
    dtmf3 = (c[0] >> 6) & 0x01;
    COS = (c[0] >> 7) & 0x01;
    irlpkeyi = c[1] & 0x01;
    irlpkey = (c[1] >> 1) & 0x01;
    key = (c[1] >> 2) & 0x01;
    mute = (c[1] >> 3) & 0x01;
    ctcss = (c[1] >> 4) & 0x01;
    fan = (c[1] >> 5) & 0x01;
    aux4 = (c[1] >> 6) & 0x01;
    aux5 = (c[1] >> 7) & 0x01;

    fprintf(stderr, "+------ From Radio ------+ ");
    fprintf(stderr, "+---------------------- To Radio --------------------+\n");
    fprintf(stderr, 
        "+---+ DMTF  +---+ COS + ");
    fprintf(stderr,
        "+ IRLP + IRLP +  KEY + MUTE +   PL +  FAN + AUX4 + AUX5 +\n");
    fprintf(stderr, 
        "| %u | %u | %u | %u |  %u  | ",
        dtmf0, dtmf1, dtmf2, dtmf3, COS);
    fprintf(stderr, 
        "|    %u |    %u |    %u |    %u |    %u |    %u |    %u |    %u |\n", 
        irlpkeyi, irlpkey, key, mute, ctcss, fan, aux4, aux5);
    fprintf(stderr, "+---+---+---+---+-----+ ");
    fprintf(stderr, "+------+------+------+------+------+------+------+------+\n\n");
}

void print_byte(unsigned char *c)
{
    fprintf(stderr, "+---+---+---+  Bit  +---+---+---+\n");
    fprintf(stderr, "| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |\n");
    fprintf(stderr, "+---+---+---+ Value +---+---+---+\n");
    each_byte(c[0]);
    each_byte(c[1]);
    fprintf(stderr, "+---+---+---+---+---+---+---+---+\n\n");
}

void each_byte(unsigned char b)
{
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
    unsigned char b4;
    unsigned char b5;
    unsigned char b6;
    unsigned char b7;

    b0 = b & 0x01;
    b1 = (b >> 1) & 0x01;
    b2 = (b >> 2) & 0x01;
    b3 = (b >> 3) & 0x01;
    b4 = (b >> 4) & 0x01;
    b5 = (b >> 5) & 0x01;
    b6 = (b >> 6) & 0x01;
    b7 = (b >> 7) & 0x01;

    fprintf(stderr, "| %u | %u | %u | %u | %u | %u | %u | %u |\n", 
        b0, b1, b2, b3, b4, b5, b6, b7);
}
