#ifndef PTV_PPR_SQLITE_DBH_
#define PTV_PPR_SQLITE_DBH_

//#define PTV_GEOMETRY_OUTPUT

/*
** The type used to represent a page number.  The first page in a file
** is called page 1.  0 is used to represent "not a page".
*/
typedef unsigned int Pgno;

/*
** Each open file is managed by a separate instance of the "Pager" structure.
*/
typedef struct Pager Pager;

typedef struct MemPage MemPage;
typedef struct Btree Btree;
typedef struct BtShared BtShared;
typedef struct BtCursor BtCursor;
typedef struct BtLock BtLock;
typedef struct OsFile OsFile;
typedef struct PgHdr PgHdr;

struct fragmentDesc
{
    u32 aPageNo;
    u32 aPayloadOffset;
    u32 aCellOffset;
    u32 aSize;
};

#endif
