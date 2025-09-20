head	1.1;
access;
symbols;
locks
	dlorre:1.1; strict;
comment	@// @;


1.1
date	96.08.23.16.57.28;	author dlorre;	state Exp;
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
@#include <exec/types.h>
#include <cxxproto/exec.h>
#include <string.h>

#include "envdef.h"
#include "enventry.h"

IMPORT nlist *envarclist ;
IMPORT nlist *envlist ;
IMPORT nlist *envloclist ;

enventry *findname(nlist *l, STRPTR name)
{
enventry *c = (enventry *)l->first() ;

    while (c && stricmp(c->name, name))
        c = (enventry *)c->next() ;
    return c ;
}

enventry *findvar(STRPTR name)
{
enventry    *ev = NULL ;

    if (ev = findname(envlist, name))
        return ev ;
    if (ev = findname(envloclist, name))
        return ev ;
    ev = findname(envarclist, name) ;
        return ev ;
}


enventry::enventry(STRPTR n, STRPTR c, long s, char t)
{
    size = s ;
    name = new char[strlen(n)+1] ;
    strcpy(name, n) ;
    label = entry ;
    contents = new char[size+1] ;
    CopyMem(c, contents, size) ;
    contents[size] = '\0' ;
    type = t ;
    setentry() ;
}


void enventry::setentry()
{
int i ;

    binary = FALSE ;
    notnullterm = FALSE ;
    for (i=0; i<size && !binary; i++) {
        binary =  (UBYTE(contents[i]) < ' ' && contents[i] != 0x0A) || ((contents[i] == 0x0A) && (i<size-1)) ;
    }

    if (binary) {
        for (i=0; i<size && !notnullterm ; i++) {
            notnullterm = contents[i] == 0x00 ;
        }
    }
    else
        notnullterm = FALSE ;

    strncpy(entry, name, MAXNAME) ;
    strcat(entry, " ") ;
    while(strlen(entry) < MAXNAME)
        strcat(entry, " ") ;
    if (type == ALIAS_TYPE) {
        strcat(entry, "[ALIAS] ") ;
    }
    strncat(entry, contents, MAXCONTENTS) ;
}

enventry::~enventry()
{
    delete name ;
    delete contents ;
}

BOOL enventry::islocale()
{
    return (type == LOCAL_TYPE) ;
}
BOOL enventry::isalias()
{
    return (type == ALIAS_TYPE) ;
}
BOOL enventry::isglobal()
{
    return findname(envlist, name) != NULL ;
}

BOOL enventry::isarchived()
{
    return findname(envarclist, name) != NULL ;
}

BOOL enventry::isbinary()
{
    return binary ;
}

BOOL enventry:: isnullterm()
{
    return !notnullterm ;
}

@
