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
@// stringreq.cc
// 01.02.95

#include "stringreq.h"

void rstring::open(screen *nws)
{

    strcpy(sname, def) ;
    ws = nws ;

    g = new gadgetlist(this, 3) ;

    g->box(20, g->fontheight+8, ws->width/2-40, g->fontheight+4) ;
    new string(g, WFUNC(&rstring::fstring), NULL, def) ;

    prepbox(TRUE) ;

    window::_open(NULL, BUTTONIDCMP,
        WA_Title,               titre,
        WA_Activate,            TRUE,
        WA_DragBar,             TRUE,
        WA_SimpleRefresh,       TRUE,
        WA_Gadgets,             g->glist,
        TAG_DONE) ;

    if (initok)
        g->updategadgets() ;
}

void rstring::fstring(gadget *g, unsigned long classe, unsigned short code)
{
    strcpy(sname, ((string *)g)->curstring) ;
}

STRPTR StringRequest(STRPTR title, STRPTR def)
{
STRPTR retstr=NULL ;
rstring rs(120, 20, 400, 70) ;

    rs.def = def ;
    rs.titre = title ;
//    beginWait();
    rs.open(ns) ;
    rs.eventloop() ;
    rs.close() ;
//    endWait() ;

    if (rs.okflag) {
        retstr = new char[strlen(rs.sname)+1] ;
        strcpy(retstr, rs.sname) ;
    }
    return retstr ;
}
@
