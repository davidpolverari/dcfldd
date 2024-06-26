/* $Id: dcfldd.c,v 1.7 2005/06/15 14:33:04 harbourn Exp $
 * dcfldd - The Enhanced Forensic DD
 * By Nicholas Harbour
 */
/* Copyright 85, 90, 91, 1995-2001, 2005 Free Software Foundation, Inc.
   Copyright 2005                        Martin Scharrer
   Copyright 2008                        David Loveall <dave@loveall.org>
   Copyright 2012                        Miah Gregory <mace@debian.org>
   Copyright 2014                        Vangelis Koukis <vkoukis@gmail.com>
   Copyright 2022                        David Polverari <david.polverari@gmail.com>
   Copyright 2022                        Tibor Szolnoki <szolnoki.tibor@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  */

/* GNU dd originally written by Paul Rubin, David MacKenzie, and Stuart Kemp. */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "dcfldd.h"

/*
 *
 * How bored are you? sitting around reading source code, its sad.
 *
 */

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <fcntl.h>
#include "system.h"
#include "human.h"
#include "long-options.h"
#include "safe-read.h"
#include "xstrtol.h"
#include "full-write.h"
#include "copy.h"
#include "hash.h"
#include "verify.h"
#include "translate.h"
#include "sizeprobe.h"
#include "pattern.h"
#include "output.h"
#include "split.h"
#include "hashformat.h"
#include "util.h"
#include "log.h"

/* The name this program was run with. */
char *program_name;

/* The name of the input file, or NULL for the standard input. */
char *input_file = NULL;

/* The name of the output file, or NULL for the standard output. */
char *output_file = NULL;

/* The number of bytes in which atomic reads are done. */
size_t input_blocksize = 0;

/* The number of bytes in which atomic writes are done. */
size_t output_blocksize = 0;

/* Conversion buffer size, in bytes.  0 prevents conversions. */
size_t conversion_blocksize = 0;

/* Skip this many records of `input_blocksize' bytes before input. */
uintmax_t skip_records = 0;

/* Skip this many records of `output_blocksize' bytes before output. */
uintmax_t seek_records = 0;

/* Copy only this many records.  The default is effectively infinity.  */
uintmax_t max_records = (uintmax_t) -1;

/* Copy only this many bytes from the input.  The default is infinity.  */
uintmax_t limit = 0;

/* Copy only this many bytes from the max_record(s). The default is effectively infinity.  */
uintmax_t max_records_extrabytes = (uintmax_t) -1;

/* Bit vector of conversions to apply. */
int conversions_mask = 0;

/* Number of partial blocks written. */
uintmax_t w_partial = 0;

/* Number of full blocks written. */
uintmax_t w_full = 0;

/* Number of partial blocks read. */
uintmax_t r_partial = 0;

/* Number of full blocks read. */
uintmax_t r_full = 0;

/* If nonzero, filter characters through the translation table.  */
int translation_needed = 0;

/* Records truncated by conv=block. */
uintmax_t r_truncate = 0;

/* If nonnzero, the last char from the previous call to `swab_buffer'
   is saved in `saved_char'.  */
int char_is_saved = 0;

/* Odd char from previous call.  */
unsigned char saved_char;

int do_status = 1;
int do_hash = 0;
int do_verify = 0;
int do_split = 0;
int do_diffwr = 0;

hashconv_t hashconv = DEFAULT_HASHCONV;

static char *splitformat = DEFAULT_SPLIT_FORMAT;
static off_t splitsize;

/* How many blocks in between screen writes for status output. */
ssize_t update_thresh = 256;

time_t start_time;

static struct conversion conversions[] =
{
    {"ascii", C_ASCII | C_TWOBUFS},	/* EBCDIC to ASCII. */
    {"ebcdic", C_EBCDIC | C_TWOBUFS},	/* ASCII to EBCDIC. */
    {"ibm", C_IBM | C_TWOBUFS},	/* Slightly different ASCII to EBCDIC. */
    {"block", C_BLOCK | C_TWOBUFS},	/* Variable to fixed length records. */
    {"unblock", C_UNBLOCK | C_TWOBUFS},	/* Fixed to variable length records. */
    {"lcase", C_LCASE | C_TWOBUFS},	/* Translate upper to lower case. */
    {"ucase", C_UCASE | C_TWOBUFS},	/* Translate lower to upper case. */
    {"swab", C_SWAB | C_TWOBUFS},	/* Swap bytes of input. */
    {"noerror", C_NOERROR},	/* Ignore i/o errors. */
    {"notrunc", C_NOTRUNC},	/* Do not truncate output file. */
    {"sync", C_SYNC},		/* Pad input records to ibs with NULs. */
    {NULL, 0}
};

FILE *popened[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void usage(int status)
{
    if (status != 0)
        log_info("Try `%s --help' for more information.\n", program_name);
    else {
        printf("Usage: %s [OPTION]...\n", program_name);
        printf("\
Enhanced version of dd for forensics and security.\n\
\n\
  bs=BYTES            force ibs=BYTES and obs=BYTES (default=32768)\n\
  cbs=BYTES           convert BYTES bytes at a time\n\
  conv=KEYWORDS       convert the file as per the comma separated keyword list\n\
  count=BLOCKS        copy only BLOCKS input blocks\n\
  limit=BYTES         similar to count but using BYTES instead of BLOCKS\n\
  ibs=BYTES           read BYTES bytes at a time\n\
  if=FILE             read from FILE instead of stdin\n\
  obs=BYTES           write BYTES bytes at a time\n\
  of=FILE             write to FILE instead of stdout\n\
  of:=COMMAND         exec and write output to process COMMAND\n\
  seek=BLOCKS         skip BLOCKS obs-sized blocks at start of output\n\
  skip=BLOCKS         skip BLOCKS ibs-sized blocks at start of input\n\
  pattern=HEX         use the specified binary pattern as input\n\
  textpattern=TEXT    use repeating TEXT as input\n\
  errlog=FILE         send error messages to FILE as well as stderr\n\
  hash=NAME           do hash calculation (md5, sha1, sha256, sha384 or sha512)\n\
  hashlog=FILE        send hash output to FILE instead of stderr\n\
  hashwindow=BYTES    perform a hash on every BYTES amount of data\n\
  hashlog:=COMMAND    exec and write hashlog to process COMMAND\n\
  ALGORITHMlog:=COMMAND    also works in the same fashion of hashlog:=COMMAND\n\
  hashconv=[before|after]  perform the hashing before or after the conversions\n\
  hashformat=FORMAT        display each hashwindow according to FORMAT\n\
  totalhashformat=FORMAT   display the total hash value according to FORMAT\n\
  status=[on|off]          display a continual status message on stderr\n\
  statusinterval=N         update the status message every N blocks\n\
  sizeprobe=[if|of|BYTES]  what to use as value to percentage indicator\n\
  split=BYTES              write every BYTES amount of data to a new file\n\
  splitformat=[TEXT|MAC|WIN]  the file extension format for split operation\n\
  vf=FILE                  verify that FILE matches the specified input\n\
  verifylog=FILE           send verify results to FILE instead of stderr\n\
  verifylog:=COMMAND       exec and write verify results to process COMMAND\n\
  diffwr=[on|off]          only write to output if destination block content differs\n\
\n\
  --help              display this help and exit\n\
  --version           output version information and exit\n\
\n\
Read the manpage dcfldd(1) for more details about each option and to see\n\
some examples.\n\
");
        puts("\nReport bugs at\nhttps://github.com/resurrecting-open-source-projects/dcfldd/issues\n");
    }
    exit(status);
}

void print_stats(void)
{
    char buf[2][LONGEST_HUMAN_READABLE + 1];
    log_info("%s+%s records in\n",
            human_readable (r_full, buf[0], 1, 1),
            human_readable (r_partial, buf[1], 1, 1));
    log_info("%s+%s records out\n",
            human_readable (w_full, buf[0], 1, 1),
            human_readable (w_partial, buf[1], 1, 1));
    if (r_truncate > 0) {
        log_info("%s %s\n",
                human_readable (r_truncate, buf[0], 1, 1),
                (r_truncate == 1
                 ? "truncated record"
                 : "truncated records"));
    }
}

void cleanup(void)
{
    if (do_status)
        fprintf(stderr, "\n");
    if (!do_verify)
        print_stats();
    if (close(STDIN_FILENO) < 0)
        ;
    if (close(STDOUT_FILENO) < 0)
        ;

    int i;
    for (i = 0; i<8; i++)
      if (popened[i] != NULL)
	pclose(popened[i]);

    outputlist_t *listptr;
    for (listptr = outputlist; listptr != NULL; listptr = listptr->next)
      if (listptr->stream != NULL)
	pclose(listptr->stream);

}

inline void quit(int code)
{
    cleanup();
    exit(code);
}

static void interrupt_handler(int sig)
{
#ifdef SA_NOCLDSTOP
    struct sigaction sigact;

    sigact.sa_handler = SIG_DFL;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(sig, &sigact, NULL);
#else
    signal(sig, SIG_DFL);
#endif
    cleanup();
    kill(getpid(), sig);
}

static void siginfo_handler(int sig)
{
    print_stats();
}

/* Encapsulate portability mess of establishing signal handlers.  */

static void install_handler(int sig_num, void (*sig_handler) (int sig))
{
#ifdef SA_NOCLDSTOP
    struct sigaction sigact;
    sigaction(sig_num, NULL, &sigact);
    if (sigact.sa_handler != SIG_IGN) {
        sigact.sa_handler = sig_handler;
        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = 0;
        sigaction(sig_num, &sigact, NULL);
    }
#else
    if (signal(sig_num, SIG_IGN) != SIG_IGN)
        signal(sig_num, sig_handler);
#endif
}

/* Open a file to a particular file descriptor.  This is like standard
   `open', except it always returns DESIRED_FD if successful.  */
static int open_fd(int desired_fd, char const *filename,
                   int options, mode_t mode)
{
    int fd;
    close(desired_fd);
    fd = open(filename, options, mode);
    if (fd < 0)
        return -1;

    if (fd != desired_fd) {
        if (dup2(fd, desired_fd) != desired_fd)
            desired_fd = -1;
        if (close(fd) != 0)
            return -1;
    }

    return desired_fd;
}

/* Interpret one "conv=..." option.
 * As a by product, this function replaces each `,' in STR with a NUL byte.
 */
void parse_conversion(char *str)
{
    char *new;
    unsigned int i;

    do {
        new = strchr(str, ',');
        if (new != NULL)
            *new++ = '\0';
        for (i = 0; conversions[i].convname != NULL; i++)
            if (STREQ(conversions[i].convname, str)) {
                conversions_mask |= conversions[i].conversion;
                break;
            }
        if (conversions[i].convname == NULL) {
            log_info("invalid conversion: %s\n", str);
            usage(1);
        }
        str = new;
    } while (new != NULL);
}

void parse_hash(char *str)
{
    char *new;
    unsigned int i;

    do {
        new = strchr(str, ',');
        if (new != NULL)
            *new++ = '\0';
        for (i = 0; hashops[i].name != NULL; i++)
            if (STREQ(hashops[i].name, str)) {
                hashflags |= hashops[i].flag;
                break;
            }
        if (hashops[i].name == NULL) {
            log_info("invalid hash: %s\n", str);
            usage(1);
        }
        str = new;
    } while (new != NULL);
}

/* Return the value of STR, interpreted as a non-negative decimal integer,
 * optionally multiplied by various values.
 * Assign nonzero to *INVALID if STR does not represent a number in
 * this format.
 */

#if HAVE_DECL_STRTOUMAX
#  define __strtol_t uintmax_t
#  define __strtol xstrtoumax
#elif HAVE_DECL_STRTOUL
#  define __strtol_t unsigned long int
#  define __strtol xstrtoul
#else
#  define __strtol_t long int
#  define __strtol xstrtol
#endif

__strtol_t parse_integer(const char *str, int *invalid)
{
    __strtol_t n;
    char *suffix;
    enum strtol_error e = __strtol(str, &suffix, 10, &n, "bcEGkMPTwYZ0");

    if (e == LONGINT_INVALID_SUFFIX_CHAR && *suffix == 'x') {
        __strtol_t multiplier = parse_integer(suffix + 1, invalid);

        if (multiplier != 0 && n * multiplier / multiplier != n) {
            *invalid = 1;
            return 0;
        }

        n *= multiplier;
    }
    else if (e != LONGINT_OK) {
        *invalid = 1;
        return 0;
    }

    return n;
}

int hex2char(char *hstr)
{
    int retval;

    if (strlen(hstr) != 2)
        return -1;
    if (EOF == sscanf(hstr, "%x", &retval))
        return -1;
    return retval;
}

static void scanargs(int argc, char **argv)
{
    int i;

    --argc;
    ++argv;

    for (i = optind; i < argc; i++) {
        char *name, *val;

        name = argv[i];
        val = strchr(name, '=');
        if (val == NULL) {
            log_info("%s: unrecognized option %s\n", program_name, name);
            usage(1);
        }
        *val++ = '\0';

        if (STREQ(name, "if"))
            if (STREQ(val, "/dev/zero")) { /* replace if=/dev/zero with pattern=00 */
                pattern = make_pattern("00");
                pattern_len = 1;
                input_from_pattern = 1;
            } else
                input_file = val;
        else if (STREQ(name, "of"))
            if (do_split)
                outputlist_add(SPLIT_FILE, val, splitformat, splitsize);
            else
                open_output(val);
        else if (STREQ(name, "of:"))
            open_output_pipe(val);
        else if (STREQ(name, "vf")) {
            verify_file = val;
            do_verify++;
        } else if (STREQ(name, "conv"))
            parse_conversion(val);
        else if (STREQ(name, "pattern")) {
            pattern = make_pattern(val);
            if (pattern == NULL) {
                log_info("%s: invalid hex pattern: %s", program_name, val);
                quit(1);
            }
            input_from_pattern = 1;
        } else if (STREQ(name, "textpattern")) {
            pattern = val;
            pattern_len = strlen(pattern);
            input_from_pattern = 1;
        } else if (STREQ(name, "hashlog")) {
            hash_log = fopen(val, "w");
            if (hash_log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "hashlog:")) {
            popened[0] = hash_log = popen(val, "w");
            if (hash_log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "hashformat"))
            hashformat = parse_hashformat(val);
        else if (STREQ(name, "totalhashformat"))
            totalhashformat = parse_hashformat(val);
        else if (STREQ(name, "md5log")) {
            hashops[MD5].log = fopen(val, "w");
            if (hashops[MD5].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "md5log:")) {
            popened[1] = hashops[MD5].log = popen(val, "w");
            if (hashops[MD5].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha1log")) {
            hashops[SHA1].log = fopen(val, "w");
            if (hashops[SHA1].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha1log:")) {
            popened[2] = hashops[SHA1].log = popen(val, "w");
            if (hashops[SHA1].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha256log")) {
            hashops[SHA256].log = fopen(val, "w");
            if (hashops[SHA256].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha256log:")) {
            popened[3] = hashops[SHA256].log = popen(val, "w");
            if (hashops[SHA256].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha384log")) {
            hashops[SHA384].log = fopen(val, "w");
            if (hashops[SHA384].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha384log:")) {
            popened[4] = hashops[SHA384].log = popen(val, "w");
            if (hashops[SHA384].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha512log")) {
            hashops[SHA512].log = fopen(val, "w");
            if (hashops[SHA512].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "sha512log:")) {
            popened[5] = hashops[SHA512].log = popen(val, "w");
            if (hashops[SHA512].log == NULL)
                syscall_error(val);
            do_hash++;
        } else if (STREQ(name, "verifylog")) {
            verify_log = fopen(val, "w");
            if (verify_log == NULL)
                syscall_error(val);
        } else if (STREQ(name, "verifylog:")) {
            popened[6] = verify_log = popen(val, "w");
            if (verify_log == NULL)
                syscall_error(val);
        } else if (STREQ(name, "errlog")) {
            errlog = fopen(val, "w");
            if (errlog == NULL)
                syscall_error(val);
        } else if (STREQ(name, "errlog:")) {
            popened[7] = errlog = popen(val, "w");
            if (errlog == NULL)
                syscall_error(val);
        } else if (STREQ(name, "splitformat"))
            splitformat = val;
        else if (STREQ(name, "status")) {
            if (STREQ(val, "off"))
                do_status = 0;
            else if (STREQ(val, "on"))
                do_status = 1;
        } else if (STREQ(name, "hashalgorithm") || STREQ(name, "hash")) {
            parse_hash(val);
            do_hash++;
        } else if (STREQ(name, "hashconv"))
            if (STREQ(val, "before"))
                hashconv = HASHCONV_BEFORE;
            else if (STREQ(val, "after"))
                hashconv = HASHCONV_AFTER;
            else
                user_error("invalid hashconv value \"%s\"", val);
        else if (STREQ(name, "sizeprobe")) {
            if (STREQ(val, "if"))
                probe = PROBE_INPUT;
            else if (STREQ(val, "of"))
                probe = PROBE_OUTPUT;
	    else {
	      int invalid = 0;
	      uintmax_t n = parse_integer(val, &invalid);
	      probe = PROBE_INPUT_PROVIDED;
	      probed_size = n;
	      if (invalid)
		probe = PROBE_NONE;
	    }
        } else if (STREQ(name, "diffwr")) {
                if (STREQ(val, "off"))
                    do_diffwr = 0;
                else if (STREQ(val, "on"))
                    do_diffwr = 1;
        } else {
            int invalid = 0;
            uintmax_t n = parse_integer(val, &invalid);

            if (STREQ(name, "ibs")) {
                input_blocksize = n;
                invalid |= input_blocksize != n || input_blocksize == 0;
                conversions_mask |= C_TWOBUFS;
            }
            else if (STREQ(name, "obs")) {
                output_blocksize = n;
                invalid |= output_blocksize != n || output_blocksize == 0;
                conversions_mask |= C_TWOBUFS;
            } else if (STREQ(name, "bs")) {
                output_blocksize = input_blocksize = n;
                invalid |= output_blocksize != n || output_blocksize == 0;
            } else if (STREQ(name, "cbs")) {
                conversion_blocksize = n;
                invalid |= (conversion_blocksize != n
                            || conversion_blocksize == 0);
            } else if (STREQ(name, "skip"))
                skip_records = n;
            else if (STREQ(name, "vskip"))
                vskip_records = n;
            else if (STREQ(name, "seek"))
                seek_records = n;
            else if (STREQ(name, "count")) {
                max_records = n;
		limit = 0;
	    }
            else if (STREQ(name, "limit"))
	        limit = n;
            else if (STREQ(name, "split")) {
                splitsize = n;
                do_split++;
            } else if (STREQ(name, "hashwindow")) {
                hash_windowlen = n;
                do_hash++;
            } else if (STREQ(name, "statusinterval")) {
                if (n == 0)
                 { invalid = 1; }
                else
                 { update_thresh = n; }
            } else {
                log_info("%s: unrecognized option %s=%s",
                        program_name, name, val);
                usage(1);
            }

            if (invalid)
                log_info("%s: invalid number %s", program_name, val);
        }
    }

/* If bs= was given, both `input_blocksize' and `output_blocksize' will
   have been set to positive values.  If either has not been set,
   bs= was not given, so make sure two buffers are used. */
    if (input_blocksize == 0 || output_blocksize == 0)
        conversions_mask |= C_TWOBUFS;
    if (input_blocksize == 0)
        input_blocksize = DEFAULT_BLOCKSIZE;
    if (output_blocksize == 0)
        output_blocksize = DEFAULT_BLOCKSIZE;
    if (conversion_blocksize == 0)
        conversions_mask &= ~(C_BLOCK | C_UNBLOCK);

    if (limit != 0) {
      max_records = (limit / input_blocksize) + 1;
      max_records_extrabytes = limit % input_blocksize;
    }

    /* set all unset hashlogs to go to the overall hashlog */
    for (i = 0; hashops[i].name != NULL; i++)
        if (hashops[i].log == NULL)
            hashops[i].log = hash_log;

    if (do_hash && hashflags == 0)
        hashflags = hashops[DEFAULT_HASH].flag;

    if (do_verify) {
        do_hash = 0;
        init_hashlist(&ihashlist, hashops[VERIFY_HASH].flag);
    } else if (do_hash)
        init_hashlist(&ihashlist, hashflags);

    /* make sure selected options make sense */
    if (output_file != NULL && verify_file != NULL)
        user_error("Please select either an output file or a verify file, not both.");
}


int main(int argc, char **argv)
{
    int i;
    int exit_status;
    char *def_hashwin_fmt;
    char *def_totalhash_fmt;

    def_hashwin_fmt = strndup(DEFAULT_HASHWINDOW_FORMAT,
                              strlen(DEFAULT_HASHWINDOW_FORMAT));
    def_totalhash_fmt = strndup(DEFAULT_TOTALHASH_FORMAT,
                                strlen(DEFAULT_TOTALHASH_FORMAT));

    /* disable buffering on stderr */
    setbuf(stderr, NULL);

    hash_log = stderr;
    verify_log = stderr;

    program_name = argv[0];

    hashformat = parse_hashformat(def_hashwin_fmt);
    totalhashformat = parse_hashformat(def_totalhash_fmt);

    /* Arrange to close stdout if parse_long_options exits.  */
    //atexit (close_stdout_wrapper);

    parse_long_options(argc, argv, PROGRAM_NAME, PACKAGE, VERSION,
                        AUTHORS, usage);

    /* Don't close stdout on exit from here on.  */
    //closeout_func = NULL;

    /* Initialize translation table to identity translation. */
    for (i = 0; i < 256; i++)
        trans_table[i] = i;

    /* Decode arguments. */
    scanargs(argc, argv);

    apply_translations();

    if (input_file != NULL) {
        if (open_fd(STDIN_FILENO, input_file, O_RDONLY, 0) < 0)
            syscall_error(input_file);
    } else if (pattern == NULL)
        input_file = "standard input";

    if (verify_file != NULL)
        if ((verify_fd = open(verify_file, O_RDONLY)) < 0)
            syscall_error(verify_file);

    if (outputlist == NULL)
        outputlist_add(SINGLE_FILE, STDOUT_FILENO);

    install_handler(SIGINT, interrupt_handler);
    install_handler(SIGQUIT, interrupt_handler);
    install_handler(SIGPIPE, interrupt_handler);
    install_handler(SIGINFO, siginfo_handler);

    if (probe == PROBE_INPUT)
        if (input_from_pattern)
            probe = PROBE_NONE;
        else
            sizeprobe(STDIN_FILENO);
    else if (probe == PROBE_OUTPUT)
        sizeprobe(outputlist->data.fd);
    start_time = time(NULL);

    if (do_verify)
        exit_status = dd_verify();
    else
        exit_status = dd_copy();

    close(1);
    quit(exit_status);
}
