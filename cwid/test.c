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
#include <stdio.h>
#include <string.h>
#include "cwid.h"
#include "wave.h"
#include "sound.h"

static char *usage =
    "Usage: test [OPTION]...\n"
    "Tone generating test\n"
    "   -o      output method [alsa|dsp|stdout]\n"
    "   -f      frequency in Hz\n"
    "   -r      sample rate in samples per second\n"
    "   -a      amplitude in %\n"
    "   -d      duration in milliseconds\n"
    "   -da     attack in milliseconds\n"
    "   -dd     decay in milliseconds\n"
    "   -h,-v   this usage information\n"
    "Copyright (c) 2011, Adi Linden <adi@adis.ca>\n";

int main(int argc, char *argv[])
{
    int     outp = OUTDEFAULT;
    int     freq = FREQ;
    int     rate = RATE;
    int     ampl = AMPL;
    int     dura = DURA;
    int     atta = ATTA;
    int     deca = DECA;
    int16_t *bf = NULL;
    int     nbf;
    int     s, w;

    /* Get any optional command line args */
    while (argc > 1) {
        if (!strcmp(argv[1], "-o")) { 
            if (!strcmp(argv[2], "alsa"))
                outp = ALSA;
            else if (!strcmp(argv[2], "dsp"))
                outp = DSP;
            else if (!strcmp(argv[2], "stdout"))
                outp = STDOUT;
            else
                outp = 0;
        }
        if (!strcmp(argv[1], "-f")) { 
            freq = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-r")) { 
            rate = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-a")) { 
            ampl = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-d")) { 
            dura = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-da")) { 
            atta = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-dd")) { 
            deca = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "-h")) { 
            fprintf(stderr, usage);
            return -1;
        }
        argc -= 2;
        argv += 2;
    }

    /* Sanaity check of inout values */
    if (outp < 1) {
        fprintf(stderr, "Output format needs to be alsa, dsp, or stdout\n");
        return -1;
    }
    if (rate < 2 * freq) {
        fprintf(stderr, "Sample rate must be more then twice the frequency\n");
        return -1;
    }
    if (freq < 10 || freq > 22500) {
        fprintf(stderr, "Support 10 to 22,500 Hz frequency range\n");
        return -1;
    }
    if (rate < 8000 || rate > 44100) {
        fprintf(stderr, "Support 8,000 to 44,100 samples per second\n");
        return -1;
    }
    if (dura > 30000) {
        fprintf(stderr, "Support duration of less than 30,000 ms (30 sec)\n");
        return -1;
    }
    if (atta + deca > dura) {
        fprintf(stderr, "Duration needs to be larger then attack plus decay time\n");
        return -1;
    }

    /* Open sound device and setup sampling parameters*/
    sound_open(outp);
    sound_setup(rate, outp);

    /* Tell about what we are doing */
    fprintf(stderr, "Generating sine wave:\n");
    fprintf(stderr, "  Output:              %d  (1 = alsa, 2 = dsp, 3 = stdout)\n", outp);
    fprintf(stderr, "  Frequency (Hz):      %d\n", freq);
    fprintf(stderr, "  Sample Rate (sps):   %d\n", rate);
    fprintf(stderr, "  Amplitude (%%):       %d\n", ampl);
    fprintf(stderr, "  Duration (ms):       %d\n", dura);
    fprintf(stderr, "  Attack (ms):         %d\n", atta);
    fprintf(stderr, "  Decay (ms):          %d\n", deca);

    /* Fill buffer with waveform */
    s = mkwave(freq, rate, ampl, dura, atta, deca, &bf, &nbf);
    if (s < 0) {
        fprintf(stderr, "Waveform generation failed\n");
        return -1;
    }

    /* Write to sound device */
    w = sound_write(&bf, &nbf, outp);

    /* Tell about what we are doing */
    fprintf(stderr, "Buffer of %d bytes for %d samples, wrote %d\n", nbf, s, w);

    /* Exit clean */
    free(bf);
    sound_close(outp);
    return 0;
} 
