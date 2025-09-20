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
@// stringreq.h
// 01.02.95


#include <libraries/gadtools.h>
#include <string.h>

#include "screen.h"
#include "window.h"
#include "gadgets/string.h"

class rstring : public window
{
public:
    STRPTR def ;
    STRPTR titre ;
    rstring(short l, short t, short w, short h) : window(l, t, w, h) {}
    void        open(screen *) ;
    void fstring(gadget *g, unsigned long classe, unsigned short code) ;
};


IMPORT screen   *ns ;
@
