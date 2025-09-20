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
@#ifndef ENVOPT_H
#define ENVOPT_H

#include <window.h>
#include <gadgets/string.h>
#include <gadgets/checkbox.h>

class optwindow : public window
{
    void fedit(gadget *g, ULONG classe, USHORT code) ;
    void fimport(gadget *g, ULONG classe, USHORT code) ;
    void fexport(gadget *g, ULONG classe, USHORT code) ;
public:
    string *scontents ;
    checkbox *cglob ;
    checkbox *carch ;
    optwindow(short l, short t, short w, short h) : window(l,t,w,h) {}
    void open(screen *ns) ;
    void handlevkey(USHORT code) ;
};
#endif
@
