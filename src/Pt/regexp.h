/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#ifndef REGEXP_DWA20011023_H
#define REGEXP_DWA20011023_H

#include <Pt/Api.h>
#include <Pt/Char.h>
#include <Pt/Regex.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>

#define NSUBEXP  10

// #define CHARTYPE char
// #define UCHARTYPE unsigned char

#define CHARTYPE Pt::Char
#define UCHARTYPE Pt::Char

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define    MAGIC    0234

typedef struct regexp {
    const CHARTYPE *startp[NSUBEXP];
    const CHARTYPE *endp[NSUBEXP];
    CHARTYPE regstart;        /* Internal use only. */
    char reganch;        /* Internal use only. */
    CHARTYPE *regmust;        /* Internal use only. */
    int regmlen;        /* Internal use only. */
    CHARTYPE program[1];    /* Unwarranted chumminess with compiler. */
} regexp;

regexp* regcomp( const CHARTYPE *exp );

int regexec( regexp *prog, const CHARTYPE *string );

#endif
