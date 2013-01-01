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

#include <string.h>
#include <stdio.h>
#include "portctl_lib.h"
#include "repeater.h"

static char *usage =
    "Usage: portctl [OPTION] CMD [CMD ...]\n"
    "Alter IRLP port state.\n"
    "   -v      clutter the screen\n"
    "   -h      display this help and exit\n"
    "Copyright (c) 2013, Adi Linden <adi@adis.ca>\n";

int verbose = 0;

int main(int argc, char *argv[])
{

    /* Get any optional command line args (start with -) */
    while (argc > 1 && *argv[1] == '-') {
        if (!strcmp(argv[1], "-h")) {
            fprintf(stderr, usage);
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

    fprintf(stderr, "%i \n", argc);
    fprintf(stderr, "%s \n", argv[0]);

    /* Perform command */
    if (argc <  2) {
        fprintf(stderr, "No valid command provided\n");
        return -1;
    }
    while (argc >  1) {
        if (!strcmp(argv[1], "key"))
            key();
        if (!strcmp(argv[1], "keyup"))
            keyup();
        if (!strcmp(argv[1], "unkey"))
            unkey();
        if (!strcmp(argv[1], "mute"))
            mute();
        if (!strcmp(argv[1], "unmute"))
            unmute();
        if (!strcmp(argv[1], "ctcsson"))
            ctcsson();
        if (!strcmp(argv[1], "ctcssff"))
            ctcssoff();
        if (!strcmp(argv[1], "fanon"))
            fanon();
        if (!strcmp(argv[1], "fanoff"))
            fanoff();
        if (!strcmp(argv[1], "aux4on"))
            aux4on();
        if (!strcmp(argv[1], "aux4off"))
            aux4off();
        if (!strcmp(argv[1], "aux5on"))
            aux5on();
        if (!strcmp(argv[1], "aux5off"))
            aux5off();
        --argc;
        ++argv;
    }

    return 0;
}
