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

/*
 * Some elements of this program were taken from sccw by Steven J. Merrifield
 * and asccw by by Clemens Ladisch.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cwid.h"
#include "wave.h"
#include "sound.h"

void text2code(char *cp);
void code2snd(char *cd);
int mktones(int wpm, int freq, int rate, int ampl, int atta, int deca);

/* Global variables */
static char *morse[] =
    {".-","-...","-.-.","-..",".","..-.","--.",
    "....","..",".---","-.-",".-..","--","-.","---",
    ".--.","--.-",".-.","...","-","..-","...-",
    ".--","-..-","-.--","--..",  /* A..Z */
    "-----",".----","..---","...--","....-",
    ".....","-....","--...","---..","----.", /* 0..9 */
    "..--..","-..-.","S"}; /*  q-mark, slant, space */

static char *usage =
    "Usage: cw [OPTION] [TEXT ...]\n"
    "Play morse code from command line.\n"
    "   -o      output method [alsa|dsp|stdout]\n"
    "   -w      word per minute\n"
    "   -f      frequency in hertz\n"
    "   -r      sample rate in samples per second\n"
    "   -a      amplitude in %\n"
    "   -da     attack in milliseconds\n"
    "   -dd     decay in milliseconds\n"
    "   -v      clutter the screen\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2011, Adi Linden <adi@adis.ca>\n";

int         fd;
int         outp = OUTDEFAULT;
int         verbose = 0;
int16_t     *ditbf, *dahbf, *sgapbf, *lgapbf;
int         nditbf, ndahbf, nsgapbf, nlgapbf;
int         sditbf, sdahbf, ssgapbf, slgapbf;

int main(int argc, char *argv[])
{
    int     wpm  = WPM;
    int     freq = FREQ;
    int     rate = RATE;
    int     ampl = AMPL;
    int     atta = ATTA;
    int     deca = DECA;

    /* Get any optional command line args (start with -) */
    while (argc > 1 && *argv[1] == '-') {
        if (!strcmp(argv[1], "-w")) { 
            wpm = atoi(argv[2]); 
        }
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
        if (!strcmp(argv[1], "-f")) { 
            freq = atoi(argv[2]); 
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

    /* Make sure we have more following */
    if (!(argc > 1)) {
        fprintf(stderr, "No text given\n");
        return -1;
    }

    /* Sanity check of values */
    if (wpm < MINWPM || wpm > MAXWPM) {
        fprintf(stderr, "Support %d to %d word per minute range\n",
                MINWPM, MAXWPM);
        return -1;
    }
    if (outp < 1) {
        fprintf(stderr, "Output format needs to be alsa, dsp, or stdout\n");
        return -1;
    }
    if (rate < 2 * freq) {
        fprintf(stderr, "Sample rate must be more then twice the frequency\n");
        return -1;
    }
    if (freq < MINFREQ || freq > MAXFREQ) {
        fprintf(stderr, "Support %d to %d Hz frequency range\n",
                MINFREQ, MAXFREQ);
        return -1;
    }
    if (rate < MINRATE || rate > MAXRATE) {
        fprintf(stderr, "Support %d to %d samples per second\n",
                MINRATE, MAXRATE);
        return -1;
    }
    if (ampl < MINAMPL || ampl > MAXAMPL) {
        fprintf(stderr, "Support %d to %d amplitude\n", MINAMPL, MAXAMPL);
        return -1;
    }

    /* Generate waveforms */
    if (mktones(wpm, freq, rate, ampl, atta, deca)) {
        return -1;
    }

    /* Setup DSP device */
    sound_open(outp);
    sound_setup(rate, outp);

    if (verbose) fprintf(stderr, "Morse code: "); 

    /* Disect remaining argv into letters */
    while (argc > 1) {
        text2code(argv[1]);
        argc--;
        argv++;
    }

    if (verbose) fprintf(stderr, "\n"); 

    /* Exit clean */
    free(ditbf);
    free(dahbf);
    free(sgapbf);
    free(lgapbf);
    sound_close(outp);
    return 0;
} 

/* mktones
 * Creates tones based on the parameters passed. Buffers are defined global.
 * Called function mkwave Uses malloc to allocate memory.
 *
 * Define international morse code using the PARIS method
 * - "dit" duration is one unit long
 * - "dah" duration is three untis long
 * - inter-element space between dits and dahs is one unti long
 * - space between letters is three units long
 * - space between words is seven units long
 *
 * PARIS methos means 50 dot durations per word.
 *   dot time (min) = 1 / (wpm * 50)
 *   dot time (sec) = 60 * 1 / (wpm * 50)
 *   dot time (sec) = 1.2 / wpm
 *   dot time (ms)  = 1200 / wpm
 */
int mktones(int wpm, int freq, int rate, int ampl, int atta, int deca)
{
    int     ditlen;

    /* Calculate dit length based on PARIS method */
    ditlen = (double) 1200 / wpm;

    /* Generate waveforms */
    sditbf = mkwave(freq, rate, ampl, ditlen, atta, deca, &ditbf, &nditbf);
    if (sditbf < 0) {
        fprintf(stderr, "Dit waveform generation failed\n");
        return -1;
    }
    sdahbf = mkwave(freq, rate, ampl, ditlen * 3, atta, deca, &dahbf, &ndahbf);
    if (sditbf < 0) {
        fprintf(stderr, "dah waveform generation failed\n");
        return -1;
    }
    ssgapbf = mksilence(rate, ditlen, &sgapbf, &nsgapbf);
    if (sditbf < 0) {
        fprintf(stderr, "short gap waveform generation failed\n");
        return -1;
    }
    slgapbf = mksilence(rate, ditlen * 3, &lgapbf, &nlgapbf);
    if (sditbf < 0) {
        fprintf(stderr, "Long gap waveform generation failed\n");
        return -1;
    }

    return 0;
}

/* text2code
 * Takes text and breaks it into individual numbers and letters. Converts
 * letters into stream of . and -. Calls code2snd to turn . and - stream
 * into audible dit and dah.
 */
void text2code(char *tx)
{
    int     ch;
    char    *cd;

    /* Break text into letters, calculate subscribt */
    while ((ch = *tx++) != '\0') {
        if (isalpha(ch))
            ch = toupper(ch);
        if ((ch >= 'A') && (ch <= 'Z')) 
            ch = ch - 65;
        else if ((ch >= '0') && (ch <= '9')) 
            ch = ch - 22;
        else if (ch == '?') 
            ch = 36;
        else if (ch == '/') 
            ch = 37;
        else if (ch == ' ') 
            ch = 38;
        else
            continue;

        /* Convert letter to code */
        cd = morse[ch];
        code2snd(cd);
    }

    /* Insert space between words */
    cd = morse[38];
    code2snd(cd);
}

/* code2snd
 * Takes string of . and - and turns them into audible tones. Typically
 * this will be one letter.
 * Requires prepared global variables:
 *   fd                 - file descriptor for DSP device
 *   ditbf, nditbf      - dit buffer and dit buffer length
 *   dahbf, ndahbf      - dah buffer and dit buffer length
 *   sgapbf, nsgapbf    - short gap buffer and dit buffer length
 *   lgapbf, nlgapbf    - long gap b uffer and dit buffer length
 */
void code2snd(char *cd)
{
    char    ch, w;

    setbuf(stdout, NULL);
    while ((ch = *cd++) != '\0') {
        /* Send dit or dah tone, or space */
        if (ch == '.') {
            w = sound_write(&ditbf, &nditbf, outp);
            if (verbose) fprintf(stderr, ".");
        }
        if (ch == '-') {
            w = sound_write(&dahbf, &ndahbf, outp);
            if (verbose) fprintf(stderr, "-");
        }
        if (ch == 'S') {
            /* Send word gap (7 dit). Note that each element is followed
             * by a short gap (1 dit) and also by the letter gap (3 dit).
             * This means we only need a single long gap here to accomplish
             * the 7 dit gap.
             */
            w = sound_write(&lgapbf, &nlgapbf, outp);
            if (verbose) fprintf(stderr, "  ");
        }
        /* Send inter-element gap (1 dit) */
        w = sound_write(&sgapbf, &nsgapbf, outp);
    }
    /* Send letter gap */
    w = sound_write(&lgapbf, &nlgapbf, outp);
    if (verbose) fprintf(stderr, " ");
}

