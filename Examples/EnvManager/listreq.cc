// listreq.cc
// 01.02.95

#include "listreq.h"

void rlist::open(screen *nws)
{

    strcpy(sname, def) ;
    ws = nws ;

    g = new gadgetlist(this, 3) ;

    g->box(20, g->fontheight+8, ws->width/2-40, g->fontheight+4) ;
    new list(g, WFUNC(&rlist::flist), NULL, def) ;

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

void rlist::flist(gadget *g, unsigned long classe, unsigned short code)
{
    strcpy(sname, ((list *)g)->curlist) ;
}

STRPTR ListRequest(STRPTR title, nlist *l)
{
STRPTR retstr=NULL ;
rlist rs(120, 20, 400, 70) ;

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
