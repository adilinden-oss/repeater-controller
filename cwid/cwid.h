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
 * This header file defines common constants for all programs
 */

/* Define general default values */
#define FREQ        660         /* Frequency in hertz */
#define RATE        22500       /* Sample rate in samples per second */ 
#define AMPL        100         /* Amplitude in percent*/
#define DURA        1000        /* Duration of tone in milliseconds */
#define ATTA        6           /* Attack of tone in milliseconds */
#define DECA        12          /* Decay of tone in milliseconds */
/* Do not change! Stereo has not been implemented, yet! */
#define CHAN        1           /* Channels 1=mono, 2=stereo */

/* Define output options and values */
#define OUTDEFAULT  1           /* Default output method */
#define ALSA        1
#define DSP         2
#define STDOUT      3
#define DEVDSP      "/dev/dsp"  /* Device for dsp output */
#define DEVALSA     "default"   /* Device for alsa output */

/* Define general acceptable value ranges */
#define MINFREQ     10          /* Frequency limits */
#define MAXFREQ     22500
#define MINRATE     8000        /* Sample rate limits */
#define MAXRATE     44100
#define MINAMPL     0           /* Amplitude limits */
#define MAXAMPL     100

/* Define cw.c specific default values */
#define WPM         10          /* Word per minute code speed */

/* Define tones.c specific default values */
#define MAXTONES    10          /* Max number of tones we handle */

/* Define acceptable ranges specific to cw.c */
#define MINWPM      2           /* Code speed limits */
#define MAXWPM      100

/* Define acceptable ranges specific to tones.c */
#define MAXDURA     2000        /* Maximum tone duration */
#define MAXSPAC     2000        /* Maximum space duration */


