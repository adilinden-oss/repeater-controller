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
#include <time.h>
#include <syslog.h>
#include <sys/time.h>
#include "log.h"

int verbose = 0;
int logging = 0;

/*
 * Print log line
 */
void do_log(char *str)
{
    int sztime = 40;
    char time[sztime];

    if (verbose) {
        timestamp(time, sztime);
        fprintf(stdout, "[%s] %s\n", time, str);
    }

    if (logging) {
        write_syslog(str);
    }
}

/* 
 * Update timestamp 
 */
void timestamp(char *buf, int sz)
{
    struct timeval tv;

    if( gettimeofday(&tv, NULL) < 0 ) buf[0] = 0;
    strftime(buf, sz, "%b %d %H:%M:%S", localtime(&tv.tv_sec));
}

/*
 * Open syslog
 *
 * We put this in here to keep all syslog dependent actions in this one file
 */
void open_syslog(char *nam)
{
    /* Open syslog */
    openlog(nam, LOG_NDELAY | LOG_PID, LOG_DAEMON);
}

/*
 * Write syslog
 */
void write_syslog(char *str)
{
    syslog(LOG_INFO, str);
}
