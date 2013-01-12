/* Copyright (c) 2004-2013, Adi Linden <adi@adis.ca>
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
#include <sys/types.h>      /* waitpid() child handling */
#include <sys/wait.h>       /* waitpid() child handling */
#include <sys/time.h>
#include "portctl_lib.h"
#include "irlpdev.h"
#include "log.h"
#include "repeater.h"

/* Our program name */
#define PROG        "repeater"

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
#define BEEP_SCRIPT "courtesy"
#define IDER_SCRIPT "ider"

static char *usage =
    "Usage: " PROG " [OPTION]\n"
    "The repeater controller.\n"
    "   -l      log to syslog\n"
    "   -v      clutter the screen\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2013, Adi Linden <adi@adis.ca>\n";


/* Execute external script in a non-blocking fashion */
void fork_script(pid_t *pid, const char *script)
{
    *pid = fork();
    if (*pid == 0) {
        system(script);
        exit(0);
    }
    if (*pid < 0) {
        exit(-1);
    }
}

/* Beep using external script */
void do_ct(pid_t *pid)
{
    char m[30];

    if (!*pid) {
        fork_script(pid, BEEP_SCRIPT);
        sprintf(m, "Script: [%d] " BEEP_SCRIPT, *pid);
        do_log(m);
    }
    else {
        do_log("Failed: " BEEP_SCRIPT);
    }
}

/* Beep using external script */
void do_id(pid_t *pid)
{
    char m[30];

    if (!*pid) {
        fork_script(pid, IDER_SCRIPT);
        sprintf(m, "Script: [%d] " IDER_SCRIPT, *pid);
        do_log(m);
    }
    else {
        do_log("Failed: " IDER_SCRIPT);
    }
}

/* Handle forked scripts */
void check_script(pid_t *pid)
{
    int s;
    pid_t p;
    char m[30];

    /* If pid is not 0 we have a child */
    if (*pid) {

        /* 
         * waitpid() returns the pid of the child that terminated 
         * the pid is passed by reference, set it 0 to indicate
         * no more child is present
         */
        p = waitpid(*pid, &s, WNOHANG);
        if (p == *pid) {
            sprintf(m, "Script: [%d] exited", *pid);
            do_log(m);
            *pid = 0;
        }
    }
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

    pid_t ctpid = 0;             /* Keep track of spawned courtesy script */
    pid_t idpid = 0;             /* Kepp track of spawned ider script */

    int idstate = 0;             /* Determines state of ID */
    int ctbusy = 0;              /* Flag while courtesy script executing */
    int idbusy = 0;              /* Flag while id script executing */
    int ctflag = 1;              /* Flag when the courtesy tone has played */
    int idflag = 1;              /* Flag when the ID tone has played */
    int muteflag = 0;            /* Flag when the muter is on */
    int keyflag = 0;             /* Flag when the system (AUX1) is keyed */
    int shortkeyflag = 0;        /* Flag when the shortkey feature is active */
    int forcekeyflag = 0;        /* Flag when the forcekey feature is active */
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
    double idtimer = 0;          /* Definition of the timer to measure time
                                    between keyup and the playing of the ID */
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
        }
        if (!strcmp(argv[1], "-l")) {
            logging = 1;
        }
        --argc;
        ++argv;
    }

    /* Open syslog */
    if (logging)
        open_syslog(PROG);
    do_log("Starting: " PROG ", version " VERSION);

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
    muteflag = mute(irlpdev);

    /* Just loop forever now */
    while (1) {

        /*
         * Get input
         */

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
                if (muteflag) {
                    if (((dnow() - mutetimer) > MUTETIME) && muteflag)
                        muteflag = unmute(irlpdev);
                }
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
            idflag = 0;
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
            idflag = 0;
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
            if (!ctflag && !ctpid && irlpflag && dnow() - cttimer > CTTIMEI) {
                do_ct(&ctpid);
                ctbusy = 1;
                forcekeyflag = 1;
            }
            /* If COS was last to drop cttimer is longer */
            if (!ctflag && !ctpid && !irlpflag && dnow() - cttimer > CTTIME) {
                do_ct(&ctpid);
                ctbusy = 1;
                forcekeyflag = 1;
            }
        }
        /* Handle forked child script */
        check_script(&ctpid);
        if (!ctpid && ctbusy && forcekeyflag) {
            ctbusy = 0;
            ctflag = 1;
            forcekeyflag = 0;
        }

        /*
         * Play the ID
         *
         * The idflag is cleared by keyup and set upon playing of ID. We
         * cannot use the keyfkag in its case because the keyup to play
         * ID would then trigger the ID requirement causing an endless loop.
         *
         * The idbusy flag is required to properly detect end of script
         * execution and setting and clearing of flags associated with that
         * event. It ensure this is a one shot event at the end of script
         * execution.
         *
         * The idstate is more then just true or false here. We track the
         * following states
         *
         *      0   idle
         *      1   immediate ID pending
         *      2   delayed ID pending
         *      3   ID played
         */

        /* ID requirement from idle */
        if (idstate == 0 && !idflag) {
            idstate = 1;
            idtimer = dnow();
            do_log("Trigger: immediate ID");
        }
        /* Repeated ID requirement */
        if (idstate == 3 && !idflag) {
            idstate = 2;
            idtimer = dnow();
            do_log("Trigger: delayed ID");
        }
        /* Immediate ID required */
        if (idstate == 1 && !idpid) {
            /* Tuck behind courtesy tone */
            if (!COS && !irlpkey && keyflag && ctflag) {
                do_id(&idpid);
                idbusy = 1;
                forcekeyflag = 1;
            }
            /* ID if we timeout */
            if (dnow() - idtimer > IDYIELD) {
                keyflag = keyup(irlpdev);
                do_id(&idpid);
                idbusy = 1;
                forcekeyflag = 1;
            }
        }
        /* Delayed ID required */
        if (idstate == 2 && !idpid) {
            /* Tuck behind courtesy tone */
            if (!COS && !irlpkey && keyflag && ctflag && 
                    dnow() - idtimer > IDTIME) {
                do_id(&idpid);
                idbusy = 1;
                forcekeyflag = 1;
            }
            /* ID if we timeout */
            if (dnow() - idtimer > IDTIME + IDYIELD) {
                keyflag = keyup(irlpdev);
                do_id(&idpid);
                idbusy = 1;
                forcekeyflag = 1;
            }
        }
        /* Reset ID */
        if (idstate == 3 && dnow() - idtimer > IDTIME + IDYIELD) {
            idstate = 0;
            do_log("Trigger: reset ID");
        }

        /* Handle forked child script */
        check_script(&idpid);
        if (!idpid && idbusy && forcekeyflag) {
            idbusy = 0;
            idflag = 1;
            idstate = 3;
            forcekeyflag = 0;
        }
        
        /*
         * Events that UNKEY
         */

        /* If the shortkey timer has not been exceeded, and COS is dropped, 
         * we drop the transmitter. It also makes sure there is no courtesy 
         * tones. 
         */
        if (!COS && !irlpkey && !forcekeyflag && keyflag && !shortkeyflag) {
            keyflag = unkey(irlpdev);
            ctflag = 1;
            idflag = 1;
            irlpflag = 0;
            shortkeyflag = 0;
            do_log("Shortkey detected");
        }
        /* When the hangtime is exceeded, the radio is unkeyed, and the 
         * shortkeytimer is reset. 
         */
        if (!COS && !irlpkey && !forcekeyflag && 
                keyflag && (dnow() - hangtimer > HANGTIME)) {
            keyflag = unkey(irlpdev);
            irlpflag = 0;
            shortkeyflag = 0;
        }

        /*
         * Miscellaneous loop tasks
         */

        fflush(stdout);
        fflush(stderr);

        /* This is a delay timer to keep this from sucking 100% processor, 
         * important in any loop. 
         */
        nanosleep(&tim, NULL);
    }
}

