/* Copyright (c) 2012, Adi Linden <adi@adis.ca>
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
 * Write sound output to alsa
 *
 * Many thanks to Linux Journal and their article on ALSA sound programming
 * as the code there was used here. http://www.linuxjournal.com/article/6735
 *
 * And the good ALSA documentaion. http://www.alsa-project.org/alsa-doc/alsa-lib
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <alsa/asoundlib.h>
#include "cwid.h"
#include "alsa.h"

snd_pcm_t *ph;

void alsa_open(char *ident)
{
    int rc;

    /* Open sound device */
    rc = snd_pcm_open(&ph, ident, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "open of pcm device %s failed\n", snd_strerror(rc));
        exit(1);
    }
}

void alsa_setup(int rate)
{
    unsigned int val; 
    int rc;
    snd_pcm_hw_params_t *params;

    /* Allocate a hardware parameters object */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values */
    snd_pcm_hw_params_any(ph, params);

    /* Interleaved mode */
    rc = snd_pcm_hw_params_set_access(ph, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0) {
        fprintf(stderr, "access type not available: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Signed 16-bit little-endian format */
    rc = snd_pcm_hw_params_set_format(ph, params, SND_PCM_FORMAT_S16_LE);
    if (rc < 0) {
        fprintf(stderr, "sample format not available: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Channels */
    val = CHAN;
    rc = snd_pcm_hw_params_set_channels(ph, params, val);
    if (rc < 0) {
        fprintf(stderr, "channel count %i not available: %s\n", 
                val, snd_strerror(rc));
        exit(1);
    }

    /* Sampling rate */
    val = rate;
    rc = snd_pcm_hw_params_set_rate_near(ph, params, &val, 0);
    if (rc < 0) {
        fprintf(stderr, "requested sampling rate not available: %s\n", 
                snd_strerror(rc));
        exit(1);
    }
    if (val != rate) {
        fprintf(stderr, "rate doen't match (requested %i, got %i): %s\n", 
                rate, val, snd_strerror(rc));
        exit(1);
    }

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(ph, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    /* Free the hatdware parameter object 
     *
     * Only needed when using malloc, we are using alloc which is automatically
     * freed at exit of the function. See snd_pcm_hw_params_alloca().
     */
    // snd_pcm_hw_params_free(params);
}

int alsa_write(int16_t **bf, int *nbf)
{
    snd_pcm_sframes_t rc;
    int n;

    /* Calculate number of samples */
    n = *nbf / 2;

    /* Write to sound device */
    //rc = snd_pcm_writei(ph, *bf, *nbf);
    rc = snd_pcm_writei(ph, *bf, n);
    if (rc < 0) {
        fprintf(stderr, "write error: %s\n", snd_strerror(rc));
    }
    return rc;
}

void alsa_close()
{
    snd_pcm_drain(ph);
    snd_pcm_close(ph);
}

