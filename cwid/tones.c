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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND COMAXTONESRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRAMAXTONESIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRAMAXTONESIES OF MERCHAMAXTONESABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVEMAXTONES SHALL THE COPYRIGHT OWNER OR COMAXTONESRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDEMAXTONESAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUEMAXTONESIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMEMAXTONES OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * IMAXTONESERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * COMAXTONESRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
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
    "Usage: tones [OPTION] [FREQUENCY DURATION SPACE]...\n"
    "Generate a sequence of tones, up to %d tones may be specified.\n"
    "   -o      output method [alsa|dsp|stdout]\n"
    "   -r      sample rate in samples per second\n"
    "   -a      amplitude in %\n"
    "   -da     attack in milliseconds\n"
    "   -dd     decay in milliseconds\n"
    "   -v      clutter the screen\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2011, Adi Linden <adi@adis.ca>\n";

int main(int argc, char *argv[])
{
    int     i, w;
    int     outp = OUTDEFAULT;
    int     rate = RATE;
    int     ampl = AMPL;
    int     atta = ATTA;
    int     deca = DECA;
    int     verbose = 0;
    int     nt = 0;
    int     freq[MAXTONES];
    int     dura[MAXTONES];
    int     spac[MAXTONES];
    int16_t *bigbf, *tonbf, *spcbf; /* Buffers for waveform */
    int     nbigbf, ntonbf, nspcbf; /* Number of bytes in buffer */
    int     sbigbf, stonbf, sspcbf; /* Number of samples in buffer */

    /* Get any optional command line args (start with -) */
    while (argc > 1 && *argv[1] == '-') {
        if (!strcmp(argv[1], "-o")) {
            if (!strcmp(argv[2], "alsa"))
                outp = ALSA;
            else if (!strcmp(argv[2], "dsp"))
                outp = DSP;
            else if (!strcmp(argv[2], "alsa"))
                outp = STDOUT;
            else
                outp = 0;
        }
        if (!strcmp(argv[1], "-r")) { 
            rate = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-a")) { 
            ampl = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-da")) { 
            atta = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-dd")) { 
            deca = atoi(argv[2]); 
        }
        if (!strcmp(argv[1], "-h")) { 
            fprintf(stderr, usage, MAXTONES);
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

    /* Get mandatory tone(s) */
    while (argc > 3) {
        freq[nt] = atoi(argv[1]);
        dura[nt] = atoi(argv[2]);
        spac[nt] = atoi(argv[3]);
        argc -= 3;
        argv += 3;
        if (!(++nt <= MAXTONES)) {
            fprintf(stderr, "Too many tones specified\n");
            fprintf(stderr, usage, MAXTONES);
            return -1;
        }
    }

    /* Sanaity check of inout values */
    if (nt < 1) {
        fprintf(stderr, "No tones specified\n");
        fprintf(stderr, usage, MAXTONES);
        return -1;
    }
    if (outp < 1) {
        fprintf(stderr, "Output format needs to be alsa, dsp, or stdout\n");
        return -1;
    }
    if (rate < MINRATE || rate > MAXRATE) {
        fprintf(stderr, "Support %d to %d samples per second\n",
                MINRATE, MAXRATE);
        return -1;
    }
    if (ampl < MINAMPL || ampl > MAXAMPL) {
        fprintf(stderr, "Support %d to %d amplitude\n",
                MINAMPL, MAXAMPL);
        return -1;
    }
    for (i=0; i<nt; ++i) {
        if (rate < 2 * freq[i]) {
            fprintf(stderr, "Sample rate must be more then twice the frequency\n");
            return -1;
        }
        if (freq[i] < MINFREQ || freq[i] > MAXFREQ) {
            fprintf(stderr, "Support %d to %d Hz frequency range\n",
                    MINFREQ, MAXFREQ);
            return -1;
        }
        if (dura[i] > MAXDURA) {
            fprintf(stderr, "Support duration of %d ms or less\n", MAXDURA);
            return -1;
        }
        if (spac[i] > 2000) {
            fprintf(stderr, "Support space of %d ms or less\n", MAXSPAC);
            return -1;
        }
        if (atta + deca > dura[i]) {
            fprintf(stderr, "Duration needs to be larger then attack plus decay time\n");
            return -1;
        }
    }

    /* Open sound device and setup sampling parameters*/
    sound_open(outp);
    sound_setup(rate, outp);

    /* Tell about what we are doing */
    if (verbose) {
        fprintf(stderr, "Generating tones:\n");
        fprintf(stderr, "  Output:              %d  (1 = alsa, 2 = dsp, 3 = stdout)\n", outp);
        fprintf(stderr, "  Sample Rate (sps):   %d\n", rate);
        fprintf(stderr, "  Amplitude (%%):       %d\n", ampl);
        fprintf(stderr, "  Attack (ms):         %d\n", atta);
        fprintf(stderr, "  Decay (ms):          %d\n", deca);
        for (i=0; i<nt; ++i) {
            fprintf(stderr, "  Tone %d:\n", i+1);
            fprintf(stderr, "    Frequency (Hz):    %d:\n", freq[i]);
            fprintf(stderr, "    Duration (ms):     %d:\n", dura[i]);
            fprintf(stderr, "    Space (ms):        %d:\n", spac[i]);
        }
    }

    /* Prepare our waveform buffer */
    bigbf = NULL;
    nbigbf = 0;
    sbigbf = 0;

    /* Generate each tone */
    for (i=0; i<nt; ++i) {

        /* Generate tone */
        stonbf = mkwave(freq[i], rate, ampl, dura[i], atta, deca, &tonbf, &ntonbf);
        if (stonbf < 0) {
            fprintf(stderr, "Waveform generation failed\n");
            return -1;
        }
        /* Generate space */
        sspcbf = mksilence(rate, spac[i], &spcbf, &nspcbf);
        if (sspcbf < 0) {
            fprintf(stderr, "Waveform generation failed\n");
            return -1;
        }

        /* Place samples into our buffer */
        bigbf = realloc(bigbf, nbigbf + ntonbf + nspcbf);
        if (bigbf == NULL) {
            fprintf(stderr, "Failed to reallocate memory\n");
            return -1;
        }
        memcpy(bigbf + sbigbf, tonbf, ntonbf);
        nbigbf += ntonbf;
        sbigbf += stonbf;
        memcpy(bigbf + sbigbf, spcbf, nspcbf);
        nbigbf += nspcbf;
        sbigbf += sspcbf;
        if (verbose) fprintf(stderr, "Add: tone %dS (%dB), space %dS (%dB), total %dS (%dB)\n",
                            stonbf, ntonbf, sspcbf, nspcbf, sbigbf, nbigbf);
        
        free(tonbf);
        free(spcbf);
    }

    /* Write buffer to device */
    w = sound_write(&bigbf, &nbigbf, outp);
    if (verbose) fprintf(stderr, "Wrote %d bytes for buffer of %d samples (%d bytes)\n", 
                        w, sbigbf, nbigbf);

    /* Exit clean */
    free(bigbf);
    sound_close(outp);
    return 0;
} 
