/*
 * regcomp and regexec -- regsub and regerror are elsewhere
 *
 *    Copyright (c) 1986 by University of Toronto.
 *    Written by Henry Spencer.  Not derived from licensed software.
 *
 *    Permission is granted to anyone to use this software for any
 *    purpose on any computer system, and to redistribute it freely,
 *    subject to the following restrictions:
 *
 *    1. The author is not responsible for the consequences of use of
 *        this software, no matter how awful, even if they arise
 *        from defects in it.
 *
 *    2. The origin of this software must not be misrepresented, either
 *        by explicit claim or by omission.
 *
 *    3. Altered versions must be plainly marked as such, and must not
 *        be misrepresented as being the original software.
 *** THIS IS AN ALTERED VERSION.  It was altered by John Gilmore,
 *** hoptoad!gnu, on 27 Dec 1986, to add \n as an alternative to |
 *** to assist in implementing egrep.
 *** THIS IS AN ALTERED VERSION.  It was altered by John Gilmore,
 *** hoptoad!gnu, on 27 Dec 1986, to add \< and \> for word-matching
 *** as in BSD grep and ex.
 *** THIS IS AN ALTERED VERSION.  It was altered by John Gilmore,
 *** hoptoad!gnu, on 28 Dec 1986, to optimize characters quoted with \.
 *** THIS IS AN ALTERED VERSION.  It was altered by James A. Woods,
 *** ames!jaw, on 19 June 1987, to quash a regcomp() redundancy.
 *** THIS IS AN ALTERED VERSION.  It was altered by Christopher Seiwald
 *** seiwald@vix.com, on 28 August 1993, for use in jam.  Regmagic.h
 *** was moved into regexp.h, and the include of regexp.h now uses "'s
 *** to avoid conflicting with the system regexp.h.  Const, bless its
 *** soul, was removed so it can compile everywhere.  The declaration
 *** of strchr() was in conflict on AIX, so it was removed (as it is
 *** happily defined in string.h).
 *** THIS IS AN ALTERED VERSION.  It was altered by Christopher Seiwald
 *** seiwald@perforce.com, on 20 January 2000, to use function prototypes.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 */
#include "regexp.h"
#include <stdio.h>
#include <ctype.h>
#include <cstdlib>
#include <string.h>

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart    char that must begin a match; '\0' if none obvious
 * reganch    is the match anchored (at beginning-of-line only)?
 * regmust    string (pointer into program) that match must include, or NULL
 * regmlen    length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that regcomp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in regexec() needs it and regcomp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition    number    opnd?    meaning */
#define    END    0    /* no    End of program. */
#define    BOL    1    /* no    Match "" at beginning of line. */
#define    EOL    2    /* no    Match "" at end of line. */
#define    ANY    3    /* no    Match any one character. */
#define    ANYOF    4    /* str    Match any character in this string. */
#define    ANYBUT    5    /* str    Match any character not in this string. */
#define    BRANCH    6    /* node    Match this alternative, or the next... */
#define    BACK    7    /* no    Match "", "next" ptr points backward. */
#define    EXACTLY    8    /* str    Match this string. */
#define    NOTHING    9    /* no    Match empty string. */
#define    STAR    10    /* node    Match this (simple) thing 0 or more times. */
#define    PLUS    11    /* node    Match this (simple) thing 1 or more times. */
#define    WORDA    12    /* no    Match "" at wordchar, where prev is nonword */
#define    WORDZ    13    /* no    Match "" at nonwordchar, where prev is word */
#define    OPEN    20    /* no    Mark this point in input as start of #n. */
            /*    OPEN+1 is number 1, etc. */
#define    CLOSE    30    /* no    Analogous to OPEN. */

/*
 * Opcode notes:
 *
 * BRANCH    The set of branches constituting a single choice are hooked
 *        together with their "next" pointers, since precedence prevents
 *        anything being concatenated to any individual branch.  The
 *        "next" pointer of the last BRANCH in a choice points to the
 *        thing following the whole choice.  This is also where the
 *        final "next" pointer of each individual branch points; each
 *        branch starts with the operand node of a BRANCH node.
 *
 * BACK        Normal "next" pointers all implicitly point forward; BACK
 *        exists to make loop structures possible.
 *
 * STAR,PLUS    '?', and complex '*' and '+', are implemented as circular
 *        BRANCH structures using BACK.  Simple cases (one character
 *        per match) are implemented with STAR and PLUS for speed
 *        and to minimize recursive plunges.
 *
 * OPEN,CLOSE    ...are numbered at compile time.
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
//#define    OP(p)    (*(p))
//#define    NEXT(p)    (((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define    OP(p)    ( (p)->value() )
#define    NEXT(p)    ( ((((p)+1)->value() & 0377) << 8) + (((p)+2)->value() & 0377) )
#define    OPERAND(p)    ((p) + 3)

/*
 * Utility definitions.
 */
// #define TO_INTTYPE(c) (c)
#define TO_INTTYPE(c) ( (c).value() )

#ifndef CHARBITS
//#define    UCHARAT(p)    ((int)*(UCHARTYPE*)(p))
#define    UCHARAT(p)    ( (int) ((p)->value()) )
#else
#define    UCHARAT(p)    ((int)*(p)&CHARBITS)
#endif

#define    FAIL(m)    { regerror(m); return(NULL); }
#define    ISMULT(c)    ((c) == '*' || (c) == '+' || (c) == '?')

/*
 * Flags to be passed up and down.
 */
#define    HASWIDTH    01    /* Known never to match null string. */
#define    SIMPLE      02    /* Simple enough to be STAR/PLUS operand. */
#define    SPSTART     04    /* Starts with * or +. */
#define    WORST       0     /* Worst case. */

/*
 * Global work variables for regcomp().
 */

struct parse_state
{
    const CHARTYPE* regparse; /* Input-scan pointer. */
    int regnpar;        /* () count. */
    CHARTYPE* regcode;  /* Code-emit pointer; &regdummy = don't. */
    long regsize;       /* Code size. */
};

//static CHARTYPE *regparse;  /* Input-scan pointer. */
//static int regnpar;         /* () count. */
static CHARTYPE regdummy;
//static CHARTYPE *regcode;   /* Code-emit pointer; &regdummy = don't. */
//static long regsize;        /* Code size. */

/*
 * Forward declarations for regcomp()'s friends.
 */
#ifndef STATIC
#define    STATIC    static
#endif
STATIC CHARTYPE *reg( parse_state* state, int paren, int *flagp );
STATIC CHARTYPE *regbranch( parse_state* state, int *flagp );
STATIC CHARTYPE *regpiece( parse_state* state, int *flagp );
STATIC CHARTYPE *regatom( parse_state* state, int *flagp );
STATIC CHARTYPE *regnode( parse_state* state,int op );
STATIC CHARTYPE *regnext( register CHARTYPE *p );
STATIC void regc( parse_state* state, INTTYPE b );
STATIC void reginsert( parse_state* state, CHARTYPE op, CHARTYPE *opnd );
STATIC void regtail( CHARTYPE *p, CHARTYPE *val );
STATIC void regoptail( CHARTYPE *p, CHARTYPE *val );

void regerror( const char *s )
{
    throw Pt::InvalidRegex(s);
}


size_t strlen ( const Pt::Char* str )
{
    return std::char_traits<Pt::Char>::length(str);
}


int strncmp(const Pt::Char* c1, const Pt::Char* c2, size_t n)
{
    return std::char_traits<Pt::Char>::compare(c1, c2, n);
}


const Pt::Char* strchr(const Pt::Char* str, const Pt::Char& c)
{
    const Pt::Char term(0);
    while( *str != term )
    {
        if( *str == c )
            return str;

        ++str;
    }

    return 0;
}


Pt::Char* strchr(Pt::Char* str, const Pt::Char& c)
{
    const Pt::Char term(0);
    while( *str != term )
    {
        if ( *str == c )
            return str;

        ++str;
    }

    return 0;
}


struct regexp_ptr
{
    regexp_ptr()
    : _r(0)
    {}

    regexp_ptr(pt_regexp* r)
    : _r(r)
    {}

    ~regexp_ptr()
    {
        if(_r) std::free(_r);
    }

    regexp_ptr& operator=(pt_regexp* r)
    {
        _r = r;
        return *this;
    }

    pt_regexp* operator->()
    {
        return _r;
    }

    bool operator!()
    {
        return _r == 0;
    }

    pt_regexp* release()
    {
        pt_regexp* r = _r;
        _r = 0;
        return r;
    }

    pt_regexp* _r;
};

/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
pt_regexp* regcomp( const CHARTYPE *exp )
{
    register regexp_ptr rx;
    register CHARTYPE *scan;
    register CHARTYPE *longest;
    register size_t len;
    ///CHARTYPE *regparse;
    parse_state state;
    int flags;

    if (exp == NULL)
        FAIL("NULL argument");

    /* First pass: determine size, legality. */
#ifdef notdef
    if (exp[0] == '.' && exp[1] == '*') exp += 2;  /* aid grep */
#endif
    ///regparse = exp;
    state.regparse = exp;
    ///regnpar = 1;
    state.regnpar = 1;
    state.regsize = 0L;
    state.regcode = &regdummy;

    regc(&state, MAGIC);
    if( reg(&state, 0, &flags) == NULL )
        return (NULL);

    /* Small enough for pointer-storage convention? */
    if (state.regsize >= 32767L)        /* Probably could be 65535L. */
        FAIL("regexp too big");

    /* Allocate space. */
    rx = (pt_regexp*) std::malloc(sizeof(pt_regexp) + ((unsigned)(state.regsize)*sizeof(CHARTYPE)) );
    if( ! rx )
        FAIL("out of space");
    /*if ( DEBUG_PROFILE )
        profile_memory( sizeof(pt_regexp) + (unsigned)regsize );*/

    rx->regrefs = 0;

    /* Second pass: emit code. */
    ///regparse = exp;
    state.regparse = exp;
    ///regnpar = 1;
    state.regnpar = 1;
    ///regcode = r->program;
    state.regcode = rx->program;

    regc(&state, MAGIC);
    if( reg(&state, 0, &flags) == NULL )
        return(NULL);

    /* Dig out information for optimizations. */
    rx->regstart = '\0';    /* Worst-case defaults. */
    rx->reganch = 0;
    rx->regmust = NULL;
    rx->regmlen = 0;
    scan = rx->program+1;            /* First BRANCH. */
    if (OP(regnext(scan)) == END) {        /* Only one top-level choice. */
        scan = OPERAND(scan);

        /* Starting-point info. */
        if (OP(scan) == EXACTLY)
            rx->regstart = *OPERAND(scan);
        else if (OP(scan) == BOL)
            rx->reganch++;

        /*
         * If there's something expensive in the r.e., find the
         * longest literal string that must appear and make it the
         * regmust.  Resolve ties in favor of later strings, since
         * the regstart check works with the beginning of the r.e.
         * and avoiding duplication strengthens checking.  Not a
         * strong reason, but sufficient in the absence of others.
         */
        if (flags&SPSTART) {
            longest = NULL;
            len = 0;
            for (; scan != NULL; scan = regnext(scan))
                if (OP(scan) == EXACTLY && strlen(OPERAND(scan)) >= len) {
                    longest = OPERAND(scan);
                    len = strlen(OPERAND(scan));
                }
            rx->regmust = longest;
            rx->regmlen = len;
        }
    }

    return rx.release();
}

/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
static CHARTYPE *
reg(
    parse_state* state,
    ///CHARTYPE*& regparse,
    int paren,            /* Parenthesized? */
    int *flagp )
{
    register CHARTYPE *ret;
    register CHARTYPE *br;
    register CHARTYPE *ender;
    register int parno = 0;
    int flags;

    *flagp = HASWIDTH;    /* Tentatively. */

    /* Make an OPEN node, if parenthesized. */
    if (paren) {
        if (state->regnpar >= NSUBEXP)
            FAIL("too many ()");
        parno = state->regnpar;
        state->regnpar++;
        ret = regnode(state, OPEN+parno);
    } else
        ret = NULL;

    /* Pick up the branches, linking them together. */
    br = regbranch(state, &flags);
    if (br == NULL)
        return(NULL);
    if (ret != NULL)
        regtail(ret, br);    /* OPEN -> first. */
    else
        ret = br;
    if (!(flags&HASWIDTH))
        *flagp &= ~HASWIDTH;
    *flagp |= flags&SPSTART;
    while (*state->regparse == '|' || *state->regparse == '\n') {
        state->regparse++;
        br = regbranch(state, &flags);
        if (br == NULL)
            return(NULL);
        regtail(ret, br);    /* BRANCH -> BRANCH. */
        if (!(flags&HASWIDTH))
            *flagp &= ~HASWIDTH;
        *flagp |= flags&SPSTART;
    }

    /* Make a closing node, and hook it on the end. */
    ender = regnode(state, (paren) ? CLOSE+parno : END);
    regtail(ret, ender);

    /* Hook the tails of the branches to the closing node. */
    for (br = ret; br != NULL; br = regnext(br))
        regoptail(br, ender);

    /* Check for proper termination. */
    if (paren && *state->regparse++ != ')') {
        FAIL("unmatched ()");
    } else if (!paren && *state->regparse != '\0') {
        if (*state->regparse == ')') {
            FAIL("unmatched ()");
        } else
            FAIL("junk on end");    /* "Can't happen". */
        /* NOTREACHED */
    }

    return(ret);
}

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
static CHARTYPE *
regbranch( parse_state* state, int *flagp )
{
    register CHARTYPE *ret;
    register CHARTYPE *chain;
    register CHARTYPE *latest;
    int flags;

    *flagp = WORST;        /* Tentatively. */

    ret = regnode(state, BRANCH);
    chain = NULL;
    while (*state->regparse != '\0' && *state->regparse != ')' &&
           *state->regparse != '\n' && *state->regparse != '|') {
        latest = regpiece(state, &flags);
        if (latest == NULL)
            return(NULL);
        *flagp |= flags&HASWIDTH;
        if (chain == NULL)    /* First piece. */
            *flagp |= flags&SPSTART;
        else
            regtail(chain, latest);
        chain = latest;
    }
    if (chain == NULL)    /* Loop ran zero times. */
        (void) regnode(state, NOTHING);

    return(ret);
}

/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
static CHARTYPE *
regpiece( parse_state* state, int *flagp )
{
    register CHARTYPE *ret;
    register CHARTYPE op;
    register CHARTYPE *next;
    int flags;

    ret = regatom(state, &flags);
    if (ret == NULL)
        return(NULL);

    op = *state->regparse;
    if (!ISMULT(op)) {
        *flagp = flags;
        return(ret);
    }

    if (!(flags&HASWIDTH) && op != '?')
        FAIL("*+ operand could be empty");
    *flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

    if (op == '*' && (flags&SIMPLE))
        reginsert(state, STAR, ret);
    else if (op == '*') {
        /* Emit x* as (x&|), where & means "self". */
        reginsert(state, BRANCH, ret);            /* Either x */
        regoptail(ret, regnode(state, BACK));        /* and loop */
        regoptail(ret, ret);            /* back */
        regtail(ret, regnode(state, BRANCH));        /* or */
        regtail(ret, regnode(state, NOTHING));        /* null. */
    } else if (op == '+' && (flags&SIMPLE))
        reginsert(state, PLUS, ret);
    else if (op == '+') {
        /* Emit x+ as x(&|), where & means "self". */
        next = regnode(state, BRANCH);            /* Either */
        regtail(ret, next);
        regtail(regnode(state, BACK), ret);        /* loop back */
        regtail(next, regnode(state, BRANCH));        /* or */
        regtail(ret, regnode(state, NOTHING));        /* null. */
    } else if (op == '?') {
        /* Emit x? as (x|) */
        reginsert(state, BRANCH, ret);            /* Either x */
        regtail(ret, regnode(state, BRANCH));        /* or */
        next = regnode(state, NOTHING);        /* null. */
        regtail(ret, next);
        regoptail(ret, next);
    }
    (state->regparse)++;
    if (ISMULT(*state->regparse))
        FAIL("nested *?+");

    return(ret);
}

/*
 - regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed characters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
static CHARTYPE *
regatom( parse_state* state, int *flagp )
{
    register CHARTYPE *ret;
    int flags;

    *flagp = WORST;        /* Tentatively. */

    const CHARTYPE* cparsed = state->regparse++;
    switch ( cparsed->value() ) 
    {
    /* FIXME: these chars only have meaning at beg/end of pat? */
    case '^':
        ret = regnode(state, BOL);
        break;
    case '$':
        ret = regnode(state, EOL);
        break;
    case '.':
        ret = regnode(state, ANY);
        *flagp |= HASWIDTH|SIMPLE;
        break;
    case '[': {
            register int classr;
            register int classend;

            if (*state->regparse == '^') {    /* Complement of range. */
                ret = regnode(state, ANYBUT);
                state->regparse++;
            } else
                ret = regnode(state, ANYOF);
            if (*state->regparse == ']' || *state->regparse == '-')
                regc(state, *state->regparse++);
            while (*state->regparse != '\0' && *state->regparse != ']') {
                if (*state->regparse == '-') {
                    state->regparse++;
                    if (*state->regparse == ']' || *state->regparse == '\0')
                        regc(state, '-');
                    else {
                        classr = UCHARAT(state->regparse-2)+1;
                        classend = UCHARAT(state->regparse);

                        if (classr > classend+1)
                            FAIL("invalid [] range");
                        for (; classr <= classend; classr++)
                            regc(state, classr);
                        state->regparse++;
                    }
                } else
                    regc(state, *state->regparse++);
            }
            regc(state, '\0');
            if (*state->regparse != ']')
                FAIL("unmatched []");
            state->regparse++;
            *flagp |= HASWIDTH|SIMPLE;
        }
        break;
    case '(':
        ret = reg(state, 1, &flags);
        if (ret == NULL)
            return(NULL);
        *flagp |= flags&(HASWIDTH|SPSTART);
        break;
    case '\0':
    case '|':
    case '\n':
    case ')':
        FAIL("internal urp");    /* Supposed to be caught earlier. */
        break;
    case '?':
    case '+':
    case '*':
        FAIL("?+* follows nothing");
        break;
    case '\\':
        cparsed = state->regparse++;
        switch ( cparsed->value() ) 
        {
        case '\0':
            FAIL("trailing \\");
            break;
        case '<':
            ret = regnode(state, WORDA);
            break;
        case '>':
            ret = regnode(state, WORDZ);
            break;
        /* FIXME: Someday handle \1, \2, ... */
        default:
            /* Handle general quoted chars in exact-match routine */
            goto de_fault;
        }
        break;
    de_fault:
    default:
        /*
         * Encode a string of characters to be matched exactly.
         *
         * This is a bit tricky due to quoted chars and due to
         * '*', '+', and '?' taking the SINGLE char previous
         * as their operand.
         *
         * On entry, the char at regparse[-1] is going to go
         * into the string, no matter what it is.  (It could be
         * following a \ if we are entered from the '\' case.)
         *
         * Basic idea is to pick up a good char in  ch  and
         * examine the next char.  If it's *+? then we twiddle.
         * If it's \ then we frozzle.  If it's other magic char
         * we push  ch  and terminate the string.  If none of the
         * above, we push  ch  on the string and go around again.
         *
         *  regprev  is used to remember where "the current char"
         * starts in the string, if due to a *+? we need to back
         * up and put the current char in a separate, 1-char, string.
         * When  regprev  is NULL,  ch  is the only char in the
         * string; this is used in *+? handling, and in setting
         * flags |= SIMPLE at the end.
         */
        {
            const CHARTYPE *regprev;
            register CHARTYPE ch;

            state->regparse--;            /* Look at cur char */
            ret = regnode(state, EXACTLY);
            for ( regprev = 0 ; ; ) {
                ch = *state->regparse++;    /* Get current char */
                switch ( state->regparse->value() ) {    /* look at next one */

                default:
                    regc(state, ch);    /* Add cur to string */
                    break;

                case '.': case '[': case '(':
                case ')': case '|': case '\n':
                case '$': case '^':
                case '\0':
                /* FIXME, $ and ^ should not always be magic */
                magic:
                    regc(state, ch);    /* dump cur char */
                    goto done;    /* and we are done */

                case '?': case '+': case '*':
                    if (!regprev)     /* If just ch in str, */
                        goto magic;    /* use it */
                    /* End mult-char string one early */
                    state->regparse = regprev; /* Back up parse */
                    goto done;

                case '\\':
                    regc(state, ch);    /* Cur char OK */
                    switch ( state->regparse[1].value() ){ /* Look after \ */
                    case '\0':
                    case '<':
                    case '>':
                    /* FIXME: Someday handle \1, \2, ... */
                        goto done; /* Not quoted */
                    default:
                        /* Backup point is \, scan                             * point is after it. */
                        regprev = state->regparse;
                        state->regparse++; 
                        continue;    /* NOT break; */
                    }
                }
                regprev = state->regparse;    /* Set backup point */
            }
        done:
            regc(state, '\0');
            *flagp |= HASWIDTH;
            if (!regprev)        /* One char? */
                *flagp |= SIMPLE;
        }
        break;
    }

    return(ret);
}

/*
 - regnode - emit a node
 */
static CHARTYPE *            /* Location. */
regnode( parse_state* state, int op )
{
    register CHARTYPE *ret;
    register CHARTYPE *ptr;

    ret = state->regcode;
    if (ret == &regdummy) {
        state->regsize += 3;
        return(ret);
    }

    ptr = ret;
    *ptr++ = op;
    *ptr++ = '\0';        /* Null "next" pointer. */
    *ptr++ = '\0';
    state->regcode = ptr;

    return(ret);
}

/*
 - regc - emit (if appropriate) a byte of code
 */
static void
regc( parse_state* state, INTTYPE b )
{
    if (state->regcode != &regdummy)
        *state->regcode++ = b;
    else
        state->regsize++;
}

/*
 - reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
static void
reginsert(
    parse_state* state,
    CHARTYPE op,
    CHARTYPE *opnd )
{
    register CHARTYPE *src;
    register CHARTYPE *dst;
    register CHARTYPE *place;

    if (state->regcode == &regdummy) {
        state->regsize += 3;
        return;
    }

    src = state->regcode;
    state->regcode += 3;
    dst = state->regcode;
    while (src > opnd)
        *--dst = *--src;

    place = opnd;        /* Op node, where operand used to be. */
    *place++ = op;
    *place++ = '\0';
    *place++ = '\0';
}

/*
 - regtail - set the next-pointer at the end of a node chain
 */
static void
regtail(
    CHARTYPE *p,
    CHARTYPE *val )
{
    register CHARTYPE *scan;
    register CHARTYPE *temp;
    register int offset;

    if (p == &regdummy)
        return;

    /* Find last node. */
    scan = p;
    for (;;) {
        temp = regnext(scan);
        if (temp == NULL)
            break;
        scan = temp;
    }

    if (OP(scan) == BACK)
        offset = static_cast<int>(scan - val);
    else
        offset = static_cast<int>(val - scan);

    *(scan+1) = (offset>>8)&0377;
    *(scan+2) = offset&0377;
}

/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */

static void
regoptail(
    CHARTYPE *p,
    CHARTYPE *val )
{
    /* "Operandless" and "op != BRANCH" are synonymous in practice. */
    if (p == NULL || p == &regdummy || OP(p) != BRANCH)
        return;
    regtail( OPERAND(p), val);
}

/*
 * regexec and friends
 */

/*
 * Global work variables for regexec().
 */
//static CHARTYPE *reginput;        /* String-input pointer. */
//static CHARTYPE *regbol;        /* Beginning of input, for ^ check. */
//static CHARTYPE **regstartp;    /* Pointer to startp array. */
//static CHARTYPE **regendp;        /* Ditto for endp. */

struct match_state
{
    const CHARTYPE *reginput;      /* String-input pointer. */
    const CHARTYPE *regbol;        /* Beginning of input, for ^ check. */
    const CHARTYPE **regstartp;    /* Pointer to startp array. */
    const CHARTYPE **regendp;      /* Ditto for endp. */
};

/*
 * Forwards.
 */
STATIC int regtry( match_state* state, pt_regexp *prog, pt_regmatch_t* match, const CHARTYPE *string );
STATIC int regmatch( match_state* state, CHARTYPE *prog );
STATIC int regrepeat( match_state* state, CHARTYPE *p );

#ifdef DEBUG
const int regnarrate = 0;
void regdump();
STATIC char *regprop(CHARTYPE*);
#endif

/*
 - regexec - match a regexp against a string
 */
int regexec(register pt_regexp *prog,
            pt_regmatch_t *match,
            register const CHARTYPE *string )
{
    register const CHARTYPE *s;

    /* Be paranoid... */
    if (prog == NULL || string == NULL) {
        regerror("NULL parameter");
        return(0);
    }

    /* Check validity of program. */
    if (UCHARAT(prog->program) != MAGIC) {
        regerror("corrupted program");
        return(0);
    }

    /* If there is a "must appear" string, look for it. */
    if (prog->regmust != NULL) {
        s = string;
        while ((s = strchr(s, prog->regmust[0])) != NULL) {
            if (strncmp(s, prog->regmust, prog->regmlen) == 0)
                break;    /* Found it. */
            s++;
        }
        if (s == NULL)    /* Not present. */
            return(0);
    }

    /* Mark beginning of line for ^ . */
    match_state state;
    state.regbol = string;

    /* Mark beginning of line for ^ . */
    //regbol = string;

    /* Simplest case:  anchored match need be tried only once. */
    if (prog->reganch)
        return(regtry(&state, prog, match, string));

    /* Messy cases:  unanchored match. */
    s = string;
    if (prog->regstart != '\0')
        /* We know what char it must start with. */
        while ((s = strchr(s, prog->regstart)) != NULL) {
            if (regtry(&state, prog, match, s))
                return(1);
            s++;
        }
    else
        /* We don't -- general case. */
        do {
            if (regtry(&state, prog, match, s))
                return(1);
        } while (*s++ != '\0');

    /* Failure. */
    return(0);
}

/*
 - regtry - try match at specific point
 */
static int            /* 0 failure, 1 success */
regtry( match_state* state,
        pt_regexp *prog,
        pt_regmatch_t* match,
        const CHARTYPE *string )
{
    register int i;
    register const CHARTYPE **sp;
    register const CHARTYPE **ep;

    state->reginput = string;
    state->regstartp = match->startp;
    state->regendp = match->endp;

    sp = match->startp;
    ep = match->endp;
    for (i = NSUBEXP; i > 0; i--) {
        *sp++ = NULL;
        *ep++ = NULL;
    }
    if (regmatch(state, prog->program + 1)) {
        match->startp[0] = string;
        match->endp[0] = state->reginput;
        return(1);
    } else
        return(0);
}

/*
 - regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.
 */
static int            /* 0 failure, 1 success */
regmatch( match_state* state, CHARTYPE *prog )
{
    register CHARTYPE *scan;    /* Current node. */
    CHARTYPE *next;        /* Next node. */

    scan = prog;
#ifdef DEBUG
    //if (scan != NULL && regnarrate)
    //    fprintf(stderr, "%s(\n", regprop(scan));
#endif
    while (scan != NULL) {
#ifdef DEBUG
   //     if (regnarrate)
   //         fprintf(stderr, "%s...\n", regprop(scan));
#endif
        next = regnext(scan);

        switch (OP(scan)) {
        case BOL:
            if (state->reginput != state->regbol)
                return(0);
            break;
        case EOL:
            if (*state->reginput != '\0')
                return(0);
            break;
        case WORDA:
            /* Must be looking at a letter, digit, or _ */
            if ((!isalnum(*state->reginput)) && *state->reginput != '_')
                return(0);
            /* Prev must be BOL or nonword */
            if (state->reginput > state->regbol &&
                (isalnum(state->reginput[-1]) || state->reginput[-1] == '_'))
                return(0);
            break;
        case WORDZ:
            /* Must be looking at non letter, digit, or _ */
            if (isalnum(*state->reginput) || *state->reginput == '_')
                return(0);
            /* We don't care what the previous char was */
            break;
        case ANY:
            if (*state->reginput == '\0')
                return(0);
            state->reginput++;
            break;
        case EXACTLY: {
                register int len;
                register CHARTYPE *opnd;

                opnd = OPERAND(scan);
                /* Inline the first character, for speed. */
                if (*opnd != *state->reginput)
                    return(0);
                len = static_cast<int>( strlen(opnd) );
                if (len > 1 && strncmp(opnd, state->reginput, len) != 0)
                    return(0);
                state->reginput += len;
            }
            break;
        case ANYOF:
             if (*state->reginput == '\0' || strchr(OPERAND(scan), *state->reginput) == NULL)
                return(0);
            state->reginput++;
            break;
        case ANYBUT:
             if (*state->reginput == '\0' || strchr(OPERAND(scan), *state->reginput) != NULL)
                return(0);
            state->reginput++;
            break;
        case NOTHING:
            break;
        case BACK:
            break;
        case OPEN+1:
        case OPEN+2:
        case OPEN+3:
        case OPEN+4:
        case OPEN+5:
        case OPEN+6:
        case OPEN+7:
        case OPEN+8:
        case OPEN+9: {
                register int no;
                register const CHARTYPE *save;

                no = OP(scan) - OPEN;
                save = state->reginput;

                if (regmatch(state, next)) {
                    /*
                     * Don't set startp if some later
                     * invocation of the same parentheses
                     * already has.
                     */
                    if (state->regstartp[no] == NULL)
                        state->regstartp[no] = save;
                    return(1);
                } else
                    return(0);
            }
            break;
        case CLOSE+1:
        case CLOSE+2:
        case CLOSE+3:
        case CLOSE+4:
        case CLOSE+5:
        case CLOSE+6:
        case CLOSE+7:
        case CLOSE+8:
        case CLOSE+9: {
                register int no;
                register const CHARTYPE *save;

                no = OP(scan) - CLOSE;
                save = state->reginput;

                if (regmatch(state, next)) {
                    /*
                     * Don't set endp if some later
                     * invocation of the same parentheses
                     * already has.
                     */
                    if (state->regendp[no] == NULL)
                        state->regendp[no] = save;
                    return(1);
                } else
                    return(0);
            }
            break;
        case BRANCH: {
                register const CHARTYPE *save;

                if (OP(next) != BRANCH)        /* No choice. */
                    next = OPERAND(scan);    /* Avoid recursion. */
                else {
                    do {
                        save = state->reginput;
                        if (regmatch(state, OPERAND(scan)))
                            return(1);
                        state->reginput = save;
                        scan = regnext(scan);
                    } while (scan != NULL && OP(scan) == BRANCH);
                    return(0);
                    /* NOTREACHED */
                }
            }
            break;
        case STAR:
        case PLUS: {
                register CHARTYPE nextch;
                register int no;
                register const CHARTYPE *save;
                register int min;

                /*
                 * Lookahead to avoid useless match attempts
                 * when we know what character comes next.
                 */
                nextch = '\0';
                if (OP(next) == EXACTLY)
                    nextch = *OPERAND(next);
                min = (OP(scan) == STAR) ? 0 : 1;
                save = state->reginput;
                no = regrepeat(state, OPERAND(scan));
                while (no >= min) {
                    /* If it could work, try it. */
                    if (nextch == '\0' || *state->reginput == nextch)
                        if (regmatch(state, next))
                            return(1);
                    /* Couldn't or didn't -- back up. */
                    no--;
                    state->reginput = save + no;
                }
                return(0);
            }
            break;
        case END:
            return(1);    /* Success! */
            break;
        default:
            regerror("memory corruption");
            return(0);
            break;
        }

        scan = next;
    }

    /*
     * We get here only if there's trouble -- normally "case END" is
     * the terminating point.
     */
    regerror("corrupted pointers");
    return(0);
}

/*
 - regrepeat - repeatedly match something simple, report how many
 */
static int
regrepeat( match_state* state, CHARTYPE *p )
{
    register int count = 0;
    register const CHARTYPE *scan;
    register CHARTYPE *opnd;

    scan = state->reginput;
    opnd = OPERAND(p);
    switch (OP(p)) {
    case ANY:
        count = static_cast<int>( strlen(scan) );
        scan += count;
        break;
    case EXACTLY:
        while (*opnd == *scan) {
            count++;
            scan++;
        }
        break;
    case ANYOF:
        while (*scan != '\0' && strchr(opnd, *scan) != NULL) {
            count++;
            scan++;
        }
        break;
    case ANYBUT:
        while (*scan != '\0' && strchr(opnd, *scan) == NULL) {
            count++;
            scan++;
        }
        break;
    default:        /* Oh dear.  Called inappropriately. */
        regerror("internal foulup");
        count = 0;    /* Best compromise. */
        break;
    }
    state->reginput = scan;

    return(count);
}

/*
 - regnext - dig the "next" pointer out of a node
 */
static CHARTYPE *
regnext( register CHARTYPE *p )
{
    register int offset;

    if (p == &regdummy)
        return(NULL);

    offset = NEXT(p);
    if (offset == 0)
        return(NULL);

    if (OP(p) == BACK)
        return(p-offset);
    else
        return(p+offset);
}

#ifdef DEBUG

/*
 - regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
void
regdump( pt_regexp *r )
{
    register CHARTYPE *s;
    register CHARTYPE op = EXACTLY;    /* Arbitrary non-END op. */
    register CHARTYPE *next;


    s = r->program + 1;
    while (op != END) {    /* While that wasn't END last time... */
        op = OP(s);
        printf("%2d%s", s-r->program, regprop(s));    /* Where, what. */
        next = regnext(s);
        if (next == NULL)        /* Next ptr. */
            printf("(0)");
        else
            printf("(%d)", (s-r->program)+(next-s));
        s += 3;
        if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
            /* Literal string, where present. */
            while (*s != '\0') {
                putchar(*s);
                s++;
            }
            s++;
        }
        putchar('\n');
    }

    /* Header fields of interest. */
    // if (r->regstart != '\0')
    //     printf("start `%c' ", r->regstart);
    // if (r->reganch)
    //     printf("anchored ");
    // if (r->regmust != NULL)
    //     printf("must have \"%s\"", r->regmust);
    printf("\n");
}

/*
 - regprop - printable representation of opcode
 */
static char *
regprop( CHARTYPE *op )
{
    register const char *p = "";
    static char buf[50];

    (void) strcpy(buf, ":");

    switch (OP(op)) {
    case BOL:
        p = "BOL";
        break;
    case EOL:
        p = "EOL";
        break;
    case ANY:
        p = "ANY";
        break;
    case ANYOF:
        p = "ANYOF";
        break;
    case ANYBUT:
        p = "ANYBUT";
        break;
    case BRANCH:
        p = "BRANCH";
        break;
    case EXACTLY:
        p = "EXACTLY";
        break;
    case NOTHING:
        p = "NOTHING";
        break;
    case BACK:
        p = "BACK";
        break;
    case END:
        p = "END";
        break;
    case OPEN+1:
    case OPEN+2:
    case OPEN+3:
    case OPEN+4:
    case OPEN+5:
    case OPEN+6:
    case OPEN+7:
    case OPEN+8:
    case OPEN+9:
        sprintf(buf+strlen(buf), "OPEN");
        p = NULL;
        break;
    case CLOSE+1:
    case CLOSE+2:
    case CLOSE+3:
    case CLOSE+4:
    case CLOSE+5:
    case CLOSE+6:
    case CLOSE+7:
    case CLOSE+8:
    case CLOSE+9:
        sprintf(buf+strlen(buf), "CLOSE");
        p = NULL;
        break;
    case STAR:
        p = "STAR";
        break;
    case PLUS:
        p = "PLUS";
        break;
    case WORDA:
        p = "WORDA";
        break;
    case WORDZ:
        p = "WORDZ";
        break;
    default:
        regerror("corrupted opcode");
        break;
    }
    if (p != NULL)
        (void) strcat(buf, p);
    return(buf);
}
#endif
