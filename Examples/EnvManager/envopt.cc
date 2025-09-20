// $Id$
#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <libraries/gadtools.h>
#include <proto/dos.h>
#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/wb.h>
#include <clib/macros.h>
#include <mydebug.h>

#include <screen.h>
#include <gadgets/checkbox.h>
#include <gadgets/string.h>
#include <gadgets/cnumber.h>
#include <gadgets/cstring.h>
#include <gadgets/fbutton.h>
#include <locale.h>
#include <compiler.h>

#include "envdef.h"
#include "envopt.h"
#include "enventry.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef open
#undef open
#endif

IMPORT TTextAttr    Tiny ;
IMPORT TTextAttr    Normal ;
IMPORT enventry     *curenv ;
IMPORT nlist        *envlist ;
IMPORT nlist        *envarclist ;

long okid, cancelid ;
static char filename[256] ;
static char path[256] ;

IMPORT lstring FAR alias ;
IMPORT lstring FAR local2 ;
IMPORT lstring FAR global2 ;
IMPORT lstring FAR archived ;
IMPORT lstring FAR binary ;
IMPORT lstring FAR nullterm ;
IMPORT lstring FAR vsize ;
IMPORT lstring FAR contents ;
IMPORT lstring FAR contents2 ;
IMPORT lstring FAR name ;
IMPORT lstring FAR edit ;
IMPORT lstring FAR import ;
IMPORT lstring FAR export ;
IMPORT lstring FAR edition ;
IMPORT lstring FAR importfrom ;
IMPORT lstring FAR exportto ;


BOOL OpenAsl(void)
{
    AslBase = OpenLibrary((UBYTE *)"asl.library", 0) ;
    return (AslBase)?TRUE:FALSE ;
}


void CloseAsl(void)
{
    if (AslBase)
        CloseLibrary(AslBase) ;
}

FileRequester *InitAslFileReq(screen *s, STRPTR drawer, STRPTR title)
{
FileRequester *fr = NULL ;

    if (OpenAsl()) {
        fr = (FileRequester *)AllocAslRequestTags(ASL_FileRequest,
            ASLFR_Screen,           s->scr,
            ASLFR_RejectIcons,      TRUE,
            ASLFR_TitleText,        title,
            ASLFR_InitialDrawer,    drawer,
            TAG_DONE) ;
    }
    return fr ;
}

inline void CloseAslFileReq(FileRequester *fr)
{
    FreeAslRequest(fr) ;
    CloseAsl() ;
}

BOOL getdir(screen *s, STRPTR drawer, STRPTR title)
{
FileRequester *fr ;
BOOL retval = FALSE ;

    if (fr = InitAslFileReq(s, drawer, title)) {
        if (AslRequestTags(fr,
            ASLFR_DrawersOnly, TRUE,
            TAG_DONE)) {
            strcpy(drawer, fr->fr_Drawer) ;
            retval = TRUE ;
        }
        CloseAslFileReq(fr) ;
    }
    return retval ;
}

BOOL getfile(screen *s, STRPTR name, STRPTR drawer, STRPTR pat, STRPTR title, BOOL savemode)
{
FileRequester *fr ;
BOOL retval = FALSE ;

    if (fr = InitAslFileReq(s, drawer, title)) {
        if (AslRequestTags(fr,
            ASLFR_InitialFile,      name,
            ASLFR_InitialPattern,   pat,
            ASLFR_DoSaveMode,       savemode,
            ASLFR_DoPatterns,       TRUE,
            TAG_DONE)) {
            strcpy(name, fr->fr_File) ;
            strcpy(drawer, fr->fr_Drawer) ;
            retval = TRUE ;
        }
        CloseAslFileReq(fr) ;
    }
    return retval ;
}

BOOL getfiles(screen *s, STRPTR name, STRPTR drawer, STRPTR pat, STRPTR title, STRPTR **fliste, LONG& num)
{
FileRequester   *fr ;
BOOL            retval = FALSE ;
WBArg           *wa ;
int             i ;
STRPTR          *t ;


    if (fr = InitAslFileReq(s, drawer, title)) {
        if (AslRequestTags(fr,
            ASLFR_InitialFile,      name,
            ASLFR_InitialPattern,   pat,
            ASLFR_DoPatterns,       TRUE,
            ASLFR_DoMultiSelect,    TRUE,
            TAG_DONE)) {
            strcpy(name, fr->fr_File) ;
            strcpy(drawer, fr->fr_Drawer) ;
            num = fr->fr_NumArgs ;
            wa = fr->fr_ArgList ;
            t = new STRPTR[num] ;
            for (i=0; i<num; i++) {
                t[i] = new char[strlen((char *)wa[i].wa_Name)+1] ;
                strcpy(t[i], (char *)wa[i].wa_Name) ;
            }
            *fliste = t ;
            retval = TRUE ;
        }
        CloseAslFileReq(fr) ;
    }
    return retval ;
}




void optwindow::fedit(gadget *g, ULONG classe, USHORT code)
{
char Cmd[255] ;
char TmpName[255] ;
char EdName[255] ;
BPTR tmp ;
FileInfoBlock ALIGNED fib ;
enventry    *edenv ;


    if (edenv = findvar("EDITOR"))
        strcpy(EdName, edenv->contents) ;
    else
        strcpy(EdName, "Ed") ;

    strcpy(TmpName, "T:EnvManXXXXXXXX") ;
    mktemp(TmpName) ;
    tmp = Open(TmpName, MODE_NEWFILE) ;
    if (tmp) {
        Write(tmp, curenv->contents, curenv->size) ;
        Close(tmp) ;
        strcpy(Cmd, EdName) ;
        strcat(Cmd, " ") ;
        strcat(Cmd, TmpName) ;
        System(Cmd, NULL) ;
        tmp = Open(TmpName, MODE_OLDFILE) ;
        if (tmp) {
            ExamineFH(tmp, &fib) ;
            curenv->size = fib.fib_Size ;
            delete curenv->contents ;
            curenv->contents = new char[curenv->size+1] ;
            Read(tmp, curenv->contents, curenv->size) ;
            curenv->contents[curenv->size] = '\0' ;
            curenv->setentry() ;
            Close(tmp) ;
            DeleteFile(TmpName) ;
        }
    }
}

void optwindow::fimport(gadget *g, ULONG classe, USHORT code)
{
BPTR fic ;
char    file[256] ;
FileInfoBlock ALIGNED fib ;

    if (getfile(ws, filename, path, "#?", importfrom.charptr(), FALSE)) {
        strcpy(file, path) ;
        AddPart(file, filename, 256) ;
        fic = Open(file, MODE_OLDFILE) ;
        if (fic) {
            ExamineFH(fic, &fib) ;
            curenv->size = fib.fib_Size ;
            delete curenv->contents ;
            curenv->contents = new char[curenv->size+1] ;
            Read(fic, curenv->contents, curenv->size) ;
            Close(fic) ;
            curenv->setentry() ;
            if (scontents) {
                scontents->set(curenv->contents) ;
            }
        }
    }
}

void optwindow::fexport(gadget *g, ULONG classe, USHORT code)
{
BPTR    fic ;
char    file[256] ;

    if (getfile(ws, filename, path, "#?", exportto.charptr(), TRUE)) {
        strcpy(file, path) ;
        AddPart(file, filename, 256) ;
        fic = Open(file, MODE_NEWFILE) ; // Tester l'overwrite qd même !
        if (fic) {
            Write(fic, curenv->contents, curenv->size) ;
            Close(fic) ;
        }
    }
}

void optwindow::finfo(gadget *g, ULONG classe, USHORT code)
{
BPTR lock, fic ;
char dirname[80] ;
char fname[80] ;
FileInfoBlock ALIGNED fib ;

    strcpy(fname, FilePart(curenv->name)) ;
    fname[strlen(fname)-5] = '\0' ;
    if (curenv->isarchived()) {
        strcpy(dirname, "ENVARC:") ;
    }
    else {
        strcpy(dirname, "ENV:") ;
    }
    AddPart(dirname, curenv->name, 80) ;

    *(PathPart(dirname)) = '\0' ;
    if(lock = Lock(dirname, ACCESS_READ)) {
        WBInfo(lock, fname, ws->scr) ;
        AddPart(dirname, fname, 80) ;
        strcat(dirname, ".info") ;
        if (fic = Open(dirname, MODE_OLDFILE)) {
            ExamineFH(fic, &fib) ;
            curenv->size = fib.fib_Size ;
            delete curenv->contents ;
            curenv->contents = new char[curenv->size+1] ;
            Read(fic, curenv->contents, curenv->size) ;
            Close(fic) ;
            curenv->setentry() ;
            if (scontents) {
                scontents->set(curenv->contents) ;
            }
        }
        UnLock(lock) ;
    }
}

void optwindow::handlevkey(USHORT code)
{
    if (code == 0x0D) {
        g->selectgadget(okid, FALSE) ;
    }
    else if (code == 0x1B) {
        g->selectgadget(cancelid, FALSE) ;
    }
    else {
        g->parsegadgets(code) ;
    }
}

void optwindow::open(screen *ns)
{
long mw ;
long ml ;
long mt ;
long bh ;
long bw ;
gadget *l ;

    ws = ns ;
    g = new gadgetlist(this, 17) ;

    g->setfont(&Normal) ;
    bh = g->fontheight ;
    bw = MAX((bh*ws->xratio)/ws->yratio, g->fontheight) + 4 ;
    bh += 4 ;
    // Colonne 1

    mw = g->lmax(alias, local2, global2, archived, NULL) + 20 ;

    g->box(mw, ws->winbarheight+8, bw, bh) ;
    new checkbox(g, NULL, alias, curenv->isalias(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->maxh+8, bw, bh) ;
    new checkbox(g, NULL, local2, curenv->islocale(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->maxh+8, bw, bh) ;
    cglob = new checkbox(g, NULL, global2, curenv->isglobal(), PLACETEXT_LEFT, curenv->isalias()) ;

    // Colonne 2

    mw = g->lmax(binary, nullterm, vsize, NULL) + 20 ;
    g->box(g->maxw+mw, ws->winbarheight+8, bw, bh) ;
    new checkbox(g, NULL, binary, curenv->isbinary(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->top+g->height+8, bw, bh) ;
    new checkbox(g, NULL, nullterm, curenv->isnullterm(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->top+g->height+8, bw, bh) ;
    carch = new checkbox(g, NULL, archived, curenv->isarchived(), PLACETEXT_LEFT, curenv->isalias()) ;

    // Colonne 3
    mw = g->lmax(name, contents, NULL) ;
    ml = g->maxw+8 ;

    g->box(ml, ws->winbarheight+8, mw+8, g->fontheight+4) ;
    new cstring(g, NULL, name, NULL, FALSE, PLACETEXT_IN) ;

    g->box(g->maxw+8, g->top, 200, g->fontheight+4) ;
    new string(g, NULL, NULL, curenv->name, MAX(strlen(curenv->name)+1, MAXNAME)) ;


    if (curenv->isbinary() || curenv->islocale() || curenv->isalias()) {
        g->box(ml, g->top+g->height+8, mw+8, g->fontheight+4) ;
        new cstring(g, NULL, contents2, NULL, FALSE, PLACETEXT_IN) ;
        g->box(g->left+g->width+8, g->top, 200, g->fontheight+4) ;
        new cstring(g, NULL, NULL, curenv->contents,TRUE, PLACETEXT_IN) ;
        scontents = NULL ;
    }
    else {
        g->box(ml, g->top+g->height+8, mw+8, g->fontheight+4) ;
        new cstring(g, NULL, contents, NULL, FALSE, PLACETEXT_IN) ;
        g->box(g->left+g->width+8, g->top, 200, g->fontheight+4) ;
        scontents = new string(g, NULL, NULL, curenv->contents, MAX(curenv->size+1, MAXNAME)) ;
    }

    g->box(g->left, g->top+g->height+8, g->ltext("99999")+8, g->fontheight+4) ;
    new cnumber(g, NULL, vsize, curenv->size, TRUE, PLACETEXT_LEFT) ;

    mw = g->lmax(edit, import, export, "Info", NULL) + 8 ;
    ml = 20 ;
    mt = g->maxh+8 ;

    if (!curenv->isalias() && !curenv->islocale()) {
        if (curenv->isbinary() && curenv->isnullterm()) {
            g->box(ml, mt, mw, g->fontheight+4) ;
            ml += mw + 8 ;
            new fbutton(g, WFUNC(&optwindow::fedit), edit, FALSE) ;
        }
        g->box(ml, mt, mw, g->fontheight+4) ;
        ml += mw + 8 ;
        new fbutton(g, WFUNC(&optwindow::fimport), import, FALSE) ;
    }

    g->box(ml, mt, mw, g->fontheight+4) ;
    ml += mw + 8 ;
    l = new fbutton(g, WFUNC(&optwindow::fexport), export, FALSE) ;

    if (curenv->isinfo() && !curenv->islocale()) {
        g->box(ml, mt, mw, g->fontheight+4) ;
        l = new fbutton(g, WFUNC(&optwindow::finfo), "_Info", FALSE) ;
    }

    okid = l->id + 1 ;
    cancelid = l->id + 2 ;

    width = short(g->maxw-ws->scr->WBorLeft+20) ;
    height = short(g->maxh-ws->winbarheight+8) ;

    prepbox(TRUE) ;

    _open(NULL, SCROLLERIDCMP|ARROWIDCMP|STRINGIDCMP|BUTTONIDCMP|
                IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS,
        WA_Title,           edition,
        WA_Gadgets,         g->glist,
        WA_DragBar,         TRUE,
        WA_DepthGadget,     TRUE,
        WA_Activate,        TRUE,
        TAG_DONE) ;

    if (initok) g->updategadgets() ;
}
