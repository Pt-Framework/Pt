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
    CHARTYPE *startp[NSUBEXP];
    CHARTYPE *endp[NSUBEXP];
    CHARTYPE regstart;        /* Internal use only. */
    char reganch;        /* Internal use only. */
    CHARTYPE *regmust;        /* Internal use only. */
    int regmlen;        /* Internal use only. */
    CHARTYPE program[1];    /* Unwarranted chumminess with compiler. */
} regexp;

PT_API regexp *regcomp( CHARTYPE *exp );

PT_API int regexec( regexp *prog, CHARTYPE *string );

void regerror( char *s )
{
    throw std::runtime_error(s);
}

size_t strlen ( const Pt::Char* str )
{
    return std::char_traits<Pt::Char>::length(str);
}

int strncmp(const Pt::Char* c1, const Pt::Char* c2, size_t n)
{
    return std::char_traits<Pt::Char>::compare(c1, c2, n);
}

const Pt::Char* strchr(const Pt::Char* str, int c)
{
    const Pt::Char term(0);
    while( *str != term )
    {
        if (*str == c)
            return str;

        ++str;
    }

    return 0;
}

Pt::Char* strchr(Pt::Char* str, int c)
{
    const Pt::Char term(0);
    while( *str != term )
    {
        if (*str == c)
            return str;

        ++str;
    }

    return 0;
}

#endif
