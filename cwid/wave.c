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

#include <stdlib.h>
#include <math.h>
#include "wave.h"

/* mkwave
 *
 * Takes waveform parameters and fills the buffer with the appropriate
 * waveform samples.
 * Returns the number of bytes placed in the buffer.
 */
int mkwave(int freq, int rate, int ampl, int dura, int atta, int deca, 
           int16_t **bf, int *nbf)
{
    int     st, sa, sd, ss;             /* to store samples */
    int16_t *tbf;                       /* temporary buffer */
    int     ntbf;                       /* number of bytes in buffer */
    int     i;
    int     a;

    /* Calculate number of samples needed */
    st = (double)rate * dura / 1000;    /* total samples */
    sa = (double)rate * atta / 1000;    /* attack samples */
    sd = (double)rate * deca / 1000;    /* decay samples */
    ss = st - sa - sd;                  /* sustain samples */

    /* Allocate memory for buffer, require 2 bytes per 16 bit value */ 
    ntbf = st * 2;
    tbf = malloc(ntbf);
    if (tbf == NULL) {
        return -1;
    }

    /* Fill buffer with samples for attack phase */
    for (i = 0; i < sa; ++i) {
        a = ((double)ampl / sa) * i;
        tbf[i] = sinval(freq, rate, a, i);
    }

    /* Fill buffer with samples for sustain phase */
    for (i = sa; i < sa + ss; ++i) {
        tbf[i] = sinval(freq, rate, ampl, i);
    }

    /* Fill buffer with samples for decay phase */
    for (i = sa + ss; i < st; ++i) {
        a = ((double)ampl / sd) * (st - i);
        tbf[i] = sinval(freq, rate, a, i);
    }

    /* return buffer and end function */
    *bf = tbf;
    *nbf = ntbf;
    return st;
}

/* sinval
 * Takes waveform parameters and phase to calculates sine value. Scales
 * sine value per amplitude parameter.
 * Returns waveform sample.
 */
int sinval(int freq, int rate, int ampl, int p)
{
    int16_t v;

    /* Calculate sine value */
    v = ((double)ampl * 0x7fff / 100) * sin((double)p / rate * freq * (2 * PI));

    return v;
}

/* mksilence
 * Takes waveform parameters and fills the buffer silence
 * Returns the number of samples placed in the buffer.
 */
int mksilence(int rate, int dura, int16_t **bf, int *nbf)
{
    int16_t *tbf;                       /* temporary buffer */
    int     ntbf;
    int     i, st;

    /* Calculate number of samples needed */
    st = (double)rate * dura / 1000;

    /* Allocate memory for buffer, require 2 bytes per 16 bit value */ 
    ntbf = st * 2;
    tbf = malloc(ntbf);
    if (tbf == NULL) {
        return -1;
    }

    /* Fill buffer with samples */
    for (i = 0; i < st; ++i) {
        tbf[i] = 0;
    }

    /* return buffer and end function */
    *bf = tbf;
    *nbf = ntbf;
    return st;
}

