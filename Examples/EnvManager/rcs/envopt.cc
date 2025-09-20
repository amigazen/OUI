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
@#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <libraries/gadtools.h>
#include <cxxproto/dos.h>
#include <cxxproto/graphics.h>
#include <cclib/macros.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <screen.h>
#include <gadgets/checkbox.h>
#include <gadgets/string.h>
#include <gadgets/cnumber.h>
#include <gadgets/cstring.h>
#include <gadgets/fbutton.h>
#include <filesel_protos.h>
#include <locale.h>

#include "envdef.h"
#include "envopt.h"
#include "enventry.h"

IMPORT TTextAttr    Tiny ;
IMPORT TTextAttr    Normal ;
IMPORT enventry     *curenv ;
IMPORT nlist        *envlist ;
IMPORT nlist        *envarclist ;

long okid, cancelid ;
static char filename[256] ;
static char path[256] ;
static char comment[256] ;

IMPORT lstring __far alias ;
IMPORT lstring __far local2 ;
IMPORT lstring __far global2 ;
IMPORT lstring __far archived ;
IMPORT lstring __far binary ;
IMPORT lstring __far nullterm ;
IMPORT lstring __far vsize ;
IMPORT lstring __far contents ;
IMPORT lstring __far contents2 ;
IMPORT lstring __far name ;
IMPORT lstring __far edit ;
IMPORT lstring __far import ;
IMPORT lstring __far export ;
IMPORT lstring __far edition ;
IMPORT lstring __far importfrom ;
IMPORT lstring __far exportto ;



void optwindow::fedit(gadget *g, ULONG classe, USHORT code)
{
char Cmd[255] ;
char TmpName[255] ;
char EdName[255] ;
BPTR tmp ;
FileInfoBlock __aligned fib ;
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
FileInfoBlock __aligned fib ;

    if (getfile(ws, filename, importfrom, path, NULL)) {
        strcpy(file, path) ;
        AddPart(file, filename, 256) ;
        fic = Open(file, MODE_OLDFILE) ;
        if (fic) {
            ExamineFH(fic, &fib) ;
            curenv->size = fib.fib_Size ;
            delete curenv->contents ;
            curenv->contents = new char[curenv->size+1] ;
            Read(fic, curenv->contents, curenv->size) ;
            curenv->contents[curenv->size] = '\0' ;
            curenv->setentry() ;
            if (scontents)
                scontents->set(curenv->contents) ;
            Close(fic) ;
        }
    }
}

void optwindow::fexport(gadget *g, ULONG classe, USHORT code)
{
BPTR    fic ;
char    file[256] ;

    if (getfile(ws, filename, exportto, path, comment)) {
        strcpy(file, path) ;
        AddPart(file, filename, 256) ;
        fic = Open(file, MODE_NEWFILE) ; // Tester l'overwrite qd même !
        if (fic) {
            Write(fic, curenv->contents, curenv->size) ;
            Close(fic) ;
            SetComment(file, comment) ;
        }
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
long bh ;
long bw ;
gadget *l ;

    ws = ns ;
    g = new gadgetlist(this, 16) ;

    g->setfont(&Normal) ;
    bh = g->fontheight ;
    bw = MAX((bh*ws->xratio)/ws->yratio, g->fontheight) ;
    // Colonne 1

    mw = g->lmax(alias, local2, global2, archived, NULL) + 20 ;

    g->box(mw, ws->winbarheight+8, bw, bh) ;
    new checkbox(g, NULL, alias, curenv->isalias(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->maxh+8, bw, bh) ;
    new checkbox(g, NULL, local2, curenv->islocale(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->maxh+8, bw, bh) ;
    cglob = new checkbox(g, NULL, global2, curenv->isglobal(), PLACETEXT_LEFT, curenv->isalias()) ;

    g->box(g->left, g->maxh+8, bw, bh) ;
    carch = new checkbox(g, NULL, archived, curenv->isarchived(), PLACETEXT_LEFT, curenv->isalias()) ;

    // Colonne 2

    mw = g->lmax(binary, nullterm, vsize, NULL) + 20 ;
    g->box(g->maxw+mw, ws->winbarheight+8, bw, bh) ;
    new checkbox(g, NULL, binary, curenv->isbinary(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->top+g->height+8, bw, bh) ;
    new checkbox(g, NULL, nullterm, curenv->isnullterm(), PLACETEXT_LEFT, TRUE) ;

    g->box(g->left, g->top+g->height+8, g->ltext("99999")+8, g->fontheight+4) ;
    new cnumber(g, NULL, vsize, curenv->size, TRUE, PLACETEXT_LEFT) ;

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

    mw = g->lmax(edit, import, export, NULL) + 8 ;

    if (curenv->isbinary() && curenv->isnullterm()) {
        g->box(g->left, g->top+g->height+8, mw, g->fontheight+4) ;
        new fbutton(g, WFUNC(&optwindow::fedit), edit, FALSE) ;
    }
    if (!curenv->isalias() && !curenv->islocale()) {
        g->box(g->left, g->top+g->height+8, mw, g->fontheight+4) ;
        new fbutton(g, WFUNC(&optwindow::fimport), import, FALSE) ;
    }

    g->box(g->left, g->top+g->height+8, mw, g->fontheight+4) ;
    l = new fbutton(g, WFUNC(&optwindow::fexport), export, FALSE) ;

    okid = l->id + 1 ;
    cancelid = l->id + 2 ;

    width = short(g->maxw-ws->screen->WBorLeft+20) ;
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
@
