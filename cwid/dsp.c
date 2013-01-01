/* Copyright (c) 2011, Adi Linden <adi@adis.ca>
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

/*
 * Write sound output to the legacy OSS /dev/dsp
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include "cwid.h"
#include "dsp.h"

int fd;

void dsp_open(char *dev)
{
    /* Open sound device */
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open of %s failed\n", dev);
        exit(1);
    }
}

void dsp_setup(int rate)
{
    int a, r;

    /* Set sampling parameters */
    a = AFMT_S16_LE;
    r = ioctl(fd, SNDCTL_DSP_SETFMT, &a);
    if (r < -1)
        fprintf(stderr, "SNDCTL_DSP_SETFMT ioctl failed\n");
    a = CHAN;
    r = ioctl(fd, SNDCTL_DSP_CHANNELS, &a);
    if (r < -1)
        fprintf(stderr, "SNDCTL_DSP_CHANNELS ioctl failed\n");
    if (a != CHAN)
        fprintf(stderr, "unable to set channels\n");
    a = rate;
    r = ioctl(fd, SNDCTL_DSP_SPEED, &a);
    if (r < -1)
        fprintf(stderr, "SNDCTL_DSP_SPEED ioctl failed\n");
    if (a != rate)
        fprintf(stderr, "unable to set sample rate, using %d\n", a);
}

int dsp_write(int16_t **bf, int *nbf)
{
    /* Write to sound device */
    return write(fd, *bf, *nbf);
}

void dsp_close()
{
    close(fd);
}
