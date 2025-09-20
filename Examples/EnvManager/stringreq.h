// stringreq.h
// 01.02.95


#include <libraries/gadtools.h>
#include <string.h>

#include <screen.h>
#include <window.h>
#include <gadgetlist.h>
#include <gadgets/string.h>

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
