// listreq.h
// 01.02.95


#include <libraries/gadtools.h>

#include "screen.h"
#include "window.h"
#include "gadgets/listview.h"

class rlist : public window
{
public:
    STRPTR titre ;
    rlist(short l, short t, short w, short h) : window(l, t, w, h) {}
    void        open(screen *) ;
    void flist(gadget *g, unsigned long classe, unsigned short code) ;
};


IMPORT screen   *ns ;
