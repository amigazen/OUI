head	1.2;
access;
symbols;
locks
	dlorre:1.2; strict;
comment	@ * @;


1.2
date	97.07.14.04.24.43;	author dlorre;	state Exp;
branches;
next	1.1;

1.1
date	96.08.22.02.05.49;	author dlorre;	state Exp;
branches;
next	;


desc
@Oui.lib -- Object User Interface
Projet cr�� en 1994
Auteur: Dominique Lorre
@


1.2
log
@now is catcomp compatible
new cast functions
@
text
@// $Id$
#ifndef CLASS_LOCALE_H
#define CLASS_LOCALE_H 1

#include <exec/types.h>
#include <libraries/locale.h>


#ifdef LOCALISE
#define NPARMS(a, b, c, d) (a, b, c, d)
#define LPARMS(a, b, c) (a, b, c)
#define OPARMS(a, b, c) (a, b, "MSG_UNDEF", c)
#define DPARMS(a, b) (a, b, "MSG_UNDEF")
#else
#define NPARMS(a, b, c, d) (a, b, d)
#define LPARMS(a, b, c) (a, b)
#define OPARMS(a, b, c) (a, b, c)
#define DPARMS(a, b) (a, b)
#endif

class catalog
{
friend class lstring ;
    Catalog *cat ;
    int locnum ;
public:
#if defined( LOCALISE )
    char catname[80] ;
    char langname[80] ;
#endif
    catalog(char *name, char *deflang, char *lang) ;
    ~catalog() ;
} ;

class lstring               // Classe de l'application
{
friend class catalog ;
public:
    char *string ;
#ifdef LOCALISE
    lstring(catalog *lc, char *def, char *desc, int id=-1) ;
#else
    lstring(catalog *lc, char *def, int id=-1) ;
#endif
    // Implicit conversion
    operator const char *() const { return (const char *)string ; }
    operator const UBYTE *() const { return (const UBYTE *)string ; }

    // Explicit conversion (used with system functions not declaring const)
    char* charptr() const { return string ; }
    UBYTE* ubyteptr() const { return (UBYTE *)string ; }
} ;
#endif@


1.1
log
@Initial revision
@
text
@d1 1
d8 13
d27 4
d40 12
a51 3
    lstring(catalog *lc, char *def, int id) ;
    lstring(catalog *lc, char *def) ;
    operator char *() { return string ; }
@
