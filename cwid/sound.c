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
 * Write sound output to the output device desires
 */

#include <stdlib.h>
#include <stdio.h>
#include "cwid.h"
#include "alsa.h"
#include "dsp.h"
#include "stdout.h"
#include "sound.h"

void sound_open(int outp)
{
    switch (outp) {
        case ALSA:
            alsa_open(DEVALSA);
            break;
        case DSP:
            dsp_open(DEVDSP);
            break;
        case STDOUT:
            stdout_open(DEVDSP);
            break;
        default:
            fprintf(stderr, "Unknown output method\n");
    }
}

void sound_setup(int rate, int outp)
{
    switch (outp) {
        case ALSA:
            alsa_setup(rate);
            break;
        case DSP:
            dsp_setup(rate);
            break;
        case STDOUT:
            stdout_setup(rate);
            break;
        default:
            fprintf(stderr, "Unknown output method\n");
    }
}

int sound_write(int16_t **bf, int *nbf, int outp)
{
    switch (outp) {
        case ALSA:
            return alsa_write(bf, nbf);
            break;
        case DSP:
            return dsp_write(bf, nbf);
            break;
        case STDOUT:
            return stdout_write(bf, nbf);
            break;
        default:
            fprintf(stderr, "Unknown output method\n");
    }
    return -1;
}

void sound_close(int outp)
{
    switch (outp) {
        case ALSA:
            alsa_close();
            break;
        case DSP:
            dsp_close();
            break;
        case STDOUT:
            stdout_close();
            break;
        default:
            fprintf(stderr, "Unknown output method\n");
    }
}

