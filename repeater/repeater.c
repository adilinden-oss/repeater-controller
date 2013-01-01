/* Copyright (c) 2013, Adi Linden <adi@adis.ca>
 * All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "portctl_lib.h"
#include "irlpdev.h"
#include "repeater.h"

/* Default timing values */
/* NOTE: all times are in millseconds */
#define HANGTIME    3000
#define SHORTKEY    10
#define FANDELAY    300000
#define MUTETIME    1000
#define CTTIME      1000
#define CTTIMEI     300
#define IDTIME      1200000
#define IDYIELD     480000

/* External scripts */
#define BEEP_SCRIPT "courtesy.sh"
#define IDER_SCRIPT "ider.sh"

static char *usage =
    "Usage: repeater [OPTION]\n"
    "The repeater controller.\n"
    "   -v      clutter the screen\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2013, Adi Linden <adi@adis.ca>\n";

int verbose = 0;

/* Beep using external script */
void beep(void)
{
    system(BEEP_SCRIPT);
}

/* Beep using external script */
void ider(void)
{
    system(IDER_SCRIPT);
}

/* Timing source */
double dnow()
{
    struct timeval tv;
    if( gettimeofday(&tv, NULL) < 0 ) return (0);
    else return(1000*((double)tv.tv_sec + 1.e-6 * (double)tv.tv_usec));
}

int main(int argc, char *argv[])
/* Main function */
  {  
    int irlpdev = 0;
    unsigned char c[2];          /* Returned string from parallel port */

    int ctflag = 0;              /* Flag when the Courtesy tone has played */
    //int idflag =0;               /* Flag when the ID has played */
    int muteflag = 0;            /* Flag when the muter is on */
    int keyflag = 0;             /* Flag when the system (AUX1) is keyed */
    int shortkeyflag = 0;        /* Flag when the shortkey feature is active */
    int fanflag = 0;             /* Flag when the fan is active */
    int irlpflag = 0;            /* Flag when IRLP keyed and is active */

    unsigned char COS = '1';     /* Character which determines the state of the 
                                    COS. Capitals used to avoid confusion with 
                                    the cosine function */
    unsigned char dtmf = '0';    /* Character which determines when a valid 
                                    DTMF tone is recieved */
    unsigned char irlpkey;       /* Character which determines when IRLP 
                                    software has the key triggered */
    struct timespec tim;         /* Timespec for the loop timer function */
    double mutetimer = 0;        /* Definition of the timer to measure time 
                                    bewteen mute on and mute off */
    double hangtimer = 0;        /* Definition of the timer to measure time 
                                    bewteen start of key and unkey */
    double cttimer = 0;          /* Definition of the timer to measure time 
                                    bewteen unkey and the playing of the 
                                    courtesy tone */
    //double idtimer = 0;          /* Definition of the timer to measure time
    //                                between keyup and the playing of the ID */
    double shortkeytimer = 0;    /* Definition of the timer to measure time 
                                    bewteen mute on and mute off */
    double fantimer = 0;         /* Definition of the time to measure time
                                    from transmit drop to fan off */

    /* Look for the command line arg we know of */
    while (argc > 1) {
        if (!strcmp(argv[1], "-h")) {
            fprintf(stderr, usage);
            return -1;
        }
        if (!strcmp(argv[1], "-v")) {
            verbose = 1;
            ++argc;     /* Offset for lack of value */
            --argv;     /* Needs to be last test!   */
        }
        argc -= 2;
        argv += 2;
    }
        
    /* Opens the /dev/irlp-port device, read/write. This is the communication
     * Channel to the IRLP hardware from the software 
     */
    if(irlpdev_open() < 0 ) { 
        fprintf(stderr, "Can't access parallel port"); 
        exit(-1); 
    } 

    /* Sets default settings for the main variables */
    tim.tv_sec = 0;
    tim.tv_nsec = 5000000;

    keyflag = unkey(irlpdev);

    while (1) {
        /* Reads the input and output bit from the port */
        if (read_irlpdev(c, 2) != 2)
            fprintf(stderr, "Can't read parallel port");

        /* Determines the status of various inputs and outputs from the port */
        COS = (c[0] >> 7) & 0x01;
        dtmf = (c[0] >> 3) & 0x0f;
        irlpkey = c[1] & 0x02;

        /*
         * FAN control
         */
        
        /* This controles the fan. If the radio has been keyed we turn on
         * the fan.
         */
        if (keyflag && !fanflag) {
            fanflag = fanon(irlpdev);
        }
        /* Unkey 5 minutes (300,000 ms) after transmitter dropped */
        if (!keyflag && fanflag && dnow() - fantimer > FANDELAY) {
            fanflag = fanoff(irlpdev);
        }
        /* fantimer is being set as long as we are keyed */
        if (keyflag) {
            fantimer = dnow();
        }
        
        /*
         * MUTE control
         */
        
        /* This sets the muter, muteflag, and mutetimer when DTMF is detected
         * It also resets the mute if no DTMF is present, and the timer has 
         * elapsed.
         */
        if (COS) {
            if (dtmf >= 1 && dtmf <= 17) {
              if (!muteflag)
                    muteflag = mute(irlpdev);
                mutetimer = dnow();
            } else {
                if (((dnow() - mutetimer) > MUTETIME) && muteflag)
                    muteflag = unmute(irlpdev);
            }
        }

        /* This mutes repeated audio if there is no COS */
        if (!COS && !muteflag) {
            muteflag = mute(irlpdev);
        }
       
        /*
         * Events that KEY
         */
        
        /* This is the start of the hangtimer. It detects COS and determines 
         * whether to key up or reset the hangtimer, depending on keyflag. 
         * Embedded is also the timer for the shortkey system 
         */
        if (COS) {
            if (!keyflag) {
                keyflag = keyup(irlpdev);
                shortkeytimer = dnow();
            }
            hangtimer = dnow();
            cttimer = dnow();
            ctflag = 0;
        }

        /* Determines if the IRLP software has keyed the radio, and sets the 
         * hangtimer, and keys up the radio. 
         */
        if (irlpkey) {
            if (!keyflag) {
                keyflag = keyup(irlpdev);
                shortkeytimer = dnow();
            }
            hangtimer = dnow();
            cttimer = dnow();
            irlpflag = 1;
            ctflag = 0;
        }

        /*
         * Shortkey feature
         */

        /* Once the shortkey timer is exceeded, it stops the shortkey 
         * features from unkeying the radio.
         */
        if (COS || irlpkey) { 
            if (!shortkeyflag && dnow() - shortkeytimer > SHORTKEY) {
                shortkeyflag = 1;
            }
        }

        /*
         * Play the courtesy tone
         */

        /* Once COS is dropped, and the cttimer is exceeded, we play the 
         * courtesy tone 
         */
        if (!COS && !irlpkey) {
            /* If IRLP was last to drop cttimer is shorter because IRLP
             * has a longer delay before unkey
             */
            if (!ctflag && irlpflag && dnow() - cttimer > CTTIMEI) {
                /* Plays the courtesy tone */
                beep();
                ctflag = 1;
                irlpflag = 0;
            }
            /* If COS was last to drop cttimer is longer */
            if (!ctflag && dnow() - cttimer > CTTIME) {
                /* Plays the courtesy tone */
                beep();
                ctflag = 1;
            }
        }

        /*
         * Events that UNKEY
         */

        /* If the shortkey timer has not been exceeded, and COS is dropped, 
         * we drop the transmitter. It also makes sure there is no courtesy 
         * tones. 
         */
        if (!COS && !irlpkey && keyflag && !shortkeyflag) {
            keyflag = unkey(irlpdev);
            ctflag = 1;
            irlpflag = 0;
            shortkeyflag = 0;
        }

        /* When the hangtime is exceeded, the radio is unkeyed, and the 
         * shortkeytimer is reset. 
         */
        if (!COS && !irlpkey && keyflag && (dnow() - hangtimer > HANGTIME)) {
            keyflag = unkey(irlpdev);
            irlpflag = 0;
            shortkeyflag = 0;
        }

        fflush(stdout);

        /* This is a delay timer to keep this from sucking 100% processor, 
         * important in any loop. 
         */
        nanosleep(&tim, NULL);
    }
}

