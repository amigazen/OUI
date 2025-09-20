#ifndef ENVENTRY_H
#define ENVENTRY_H

#include <slist.h>

class enventry : public nlink
{
public:
    char    entry[MAXSIZE] ;
    char    *name ;
    char    *contents ;
    long    size ;
    char    type ;
    BOOL    binary ;
    BOOL    notnullterm ;
    BOOL    info ;

    enventry(STRPTR n, STRPTR c, long s, char t) ;
    ~enventry();
    void setentry() ;
    BOOL islocale() ;
    BOOL isalias() ;
    BOOL isglobal() ;
    BOOL isarchived() ;
    BOOL isbinary() ;
    BOOL isnullterm() ;
    BOOL isinfo() ;
};
enventry *findname(nlist *l, STRPTR name) ;
enventry *findvar(STRPTR name) ;

#endif

