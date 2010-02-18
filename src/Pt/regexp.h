/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#ifndef REGEXP_DWA20011023_H
#define REGEXP_DWA20011023_H

#include <Pt/Api.h>
#include <stdio.h>

#define NSUBEXP  10

typedef struct regexp {
    char *startp[NSUBEXP];
    char *endp[NSUBEXP];
    char regstart;        /* Internal use only. */
    char reganch;        /* Internal use only. */
    char *regmust;        /* Internal use only. */
    int regmlen;        /* Internal use only. */
    char program[1];    /* Unwarranted chumminess with compiler. */
} regexp;

PT_API regexp *regcomp( char *exp );

PT_API int regexec( regexp *prog, char *string );

void regerror( char *s );

void regerror( char *s )
{
    printf( "re error %s\n", s );
}

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define    MAGIC    0234

#endif

