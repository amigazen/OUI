#ifndef ENVWIN_H
#define ENVWIN_H

#include <window.h>
#include <gadgetlist.h>
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
    void flist(gadget *g, ULONG classe, USHORT code) ;
    void globlist(USHORT code) ;
    void arclist(USHORT code) ;
    void fswap(gadget *g, ULONG classe, USHORT code) ;
    void fnew(gadget *g, ULONG classe, USHORT code) ;
    void fpurge(gadget *g, ULONG classe, USHORT code) ;
    void fhelp(gadget *g, ULONG classe, USHORT code) ;
public:
    envwindow(short l, short t, short w, short h) : window(l,t,w,h) {}
    void open(screen *ns) ;
    void handlevkey(USHORT code) ;
    listview *lloc ;
    listview *lenv ;
    listview *larc ;
    cnumber *es ;
    int     oldsize ;
};
#endif
