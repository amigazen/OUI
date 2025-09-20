head	1.1;
access;
symbols;
locks
	dlorre:1.1; strict;
comment	@ * @;


1.1
date	96.08.23.16.57.35;	author dlorre;	state Exp;
branches;
next	;


desc
@EnvManager : Gestionnaire d'Environnement
Auteur : Dominique Lorre
@


1.1
log
@Initial revision
@
text
@#ifndef ENVENTRY_H
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

    enventry(STRPTR n, STRPTR c, long s, char t) ;
    ~enventry();
    void setentry() ;
    BOOL islocale() ;
    BOOL isalias() ;
    BOOL isglobal() ;
    BOOL isarchived() ;
    BOOL isbinary() ;
    BOOL isnullterm() ;
};
enventry *findname(nlist *l, STRPTR name) ;
enventry *findvar(STRPTR name) ;

#endif

@
