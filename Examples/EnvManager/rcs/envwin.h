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
@#ifndef ENVWIN_H
#define ENVWIN_H

#include <window.h>
#include <gadgets/listview.h>
#include <gadgets/cnumber.h>
#include "enventry.h"

class envwindow : public window
{
    void updateenv(enventry *curenv,
        BOOL wasarch, BOOL wasglob,
        BOOL isarch, BOOL isglob,
        BOOL fromString) ;
    void floclist(gadget *g, ULONG classe, USHORT code) ;
    void fenvlist(gadget *g, ULONG classe, USHORT code) ;
    void farclist(gadget *g, ULONG classe, USHORT code) ;
    void fnew(gadget *g, ULONG classe, USHORT code) ;
    void fpurge(gadget *g, ULONG classe, USHORT code) ;
    void fhelp(gadget *g, ULONG classe, USHORT code) ;
public:
    envwindow(short l, short t, short w, short h) : window(l,t,w,h) {}
    void open(screen *ns) ;
    void handlevkey(USHORT code) ;
    listview *lloc ;
    listview *lglob ;
    listview *larc ;
    cnumber *es ;
    int     oldsize ;
};
#endif
@
