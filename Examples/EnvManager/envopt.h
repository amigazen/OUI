#ifndef ENVOPT_H
#define ENVOPT_H

#include <window.h>
#include <gadgetlist.h>
#include <gadgets/string.h>
#include <gadgets/checkbox.h>

class optwindow : public window
{
    void fedit(gadget *g, ULONG classe, USHORT code) ;
    void fimport(gadget *g, ULONG classe, USHORT code) ;
    void fexport(gadget *g, ULONG classe, USHORT code) ;
    void finfo(gadget *g, ULONG classe, USHORT code) ;
public:
    optwindow(short l, short t, short w, short h) : window(l,t,w,h) {}
    void open(screen *ns) ;
    void handlevkey(USHORT code) ;
    string *scontents ;
    checkbox *cglob ;
    checkbox *carch ;
};
#endif
