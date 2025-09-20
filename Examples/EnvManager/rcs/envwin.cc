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
#include <dos/dosextens.h>
#include <dos/var.h>
#include <libraries/gadtools.h>
#include <cxxproto/dos.h>
#include <cclib/debug_protos.h>
#include <stdio.h>
#include <string.h>

#include <screen.h>
#include <gadgets/fbutton.h>
#include <gadgets/cstring.h>
#include <gadgets/cnumber.h>
#include <gadgets/listview.h>
#include <locale.h>

#include "envdef.h"
#include "envwin.h"
#include "enventry.h"
#include "envopt.h"

IMPORT catalog *ecat ;

// Project Menu
lstring __far local(ecat, "L_ocal", 0) ;
lstring __far global(ecat, "G_lobal") ;
lstring __far archive(ecat, "A_rchive") ;
lstring __far locsize(ecat, "Local Size") ;
lstring __far globsize(ecat, "Global Size") ;
lstring __far newentry(ecat, "Create/Search") ;
lstring __far process(ecat, "Process") ;

lstring __far newvar(ecat, "Var Name:", 50) ;
lstring __far unnamed(ecat, "UnNamed") ;
lstring __far nothing(ecat, "Nothing") ;

lstring __far alias(ecat, "Alias", 100) ;
lstring __far local2(ecat, "Local") ;
lstring __far global2(ecat, "Global") ;
lstring __far archived(ecat, "Archived") ;
lstring __far binary(ecat, "Binary") ;
lstring __far nullterm(ecat, "Null Term") ;
lstring __far vsize(ecat, "Size") ;
lstring __far contents(ecat, "_Contents") ;
lstring __far contents2(ecat, "Contents") ;
lstring __far name(ecat, "_Name") ;
lstring __far edit(ecat, "_Edit") ;
lstring __far import(ecat, "_Import") ;
lstring __far export(ecat, "_Export") ;
lstring __far edition(ecat, "Edition") ;

lstring __far importfrom(ecat, "Import from:", 200) ;
lstring __far exportto(ecat, "Export to:") ;

STRPTR StringRequest(STRPTR title, STRPTR def) ;

IMPORT nlist    *envloclist ;
IMPORT nlist    *envlist ;
IMPORT nlist    *envarclist ;
IMPORT TTextAttr    Tiny ;
IMPORT TTextAttr    Normal ;
IMPORT LONG         envsize ;
IMPORT LONG         envlocsize ;
IMPORT Process      *proc ;

enventry *curenv ;

void envwindow::updateenv(enventry *curenv,
        BOOL wasarch, BOOL wasglob,
        BOOL isarch, BOOL isglob,
        BOOL fromString)
{
char    path[256] ;
LONG flags ;
enventry *genv ;
enventry *aenv ;

    if (wasglob) envsize -= oldsize ;

    if (isarch) {
        flags = GVF_GLOBAL_ONLY|GVF_SAVE_VAR ;
        if (curenv->isbinary() && !fromString)
            flags |= GVF_BINARY_VAR ;
        if (!curenv->isnullterm())
            flags |= GVF_DONT_NULL_TERM ;
        if (!wasarch) {
            envarclist->enqueue(new enventry(curenv->name,
                curenv->contents,
                curenv->size,
                ARCHIVED_TYPE)) ;
        }
        SetVar(curenv->name, curenv->contents, curenv->size, flags) ;
    }
    if (isglob) {
        envsize += curenv->size ;
        flags = GVF_GLOBAL_ONLY ;
        if (curenv->isbinary() && !fromString)
            flags |= GVF_BINARY_VAR ;
        if (!curenv->isnullterm())
            flags |= GVF_DONT_NULL_TERM ;
        if (!wasglob) {
            envlist->enqueue(new enventry(curenv->name,
                curenv->contents,
                curenv->size,
                GLOBAL_TYPE)) ;
        }
        SetVar(curenv->name, curenv->contents, curenv->size, flags) ;
    }
    genv = findname(envlist, curenv->name) ;
    aenv = findname(envarclist, curenv->name) ;
    if (wasarch && !isarch) {
        envarclist->remove(aenv) ;
        strcpy(path, "ENVARC:") ;
        AddPart(path, aenv->name, 256) ;
        DeleteFile(path) ;
        delete aenv ;
    }
    if (wasglob && !isglob) {
        envlist->remove(genv) ;
        strcpy(path, "ENV:") ;
        AddPart(path, genv->name, 256) ;
        DeleteFile(path) ;
        delete genv ;
    }
    lglob->reset(envlist, 0, 0) ;
    larc->reset(envarclist, 0, 0) ;
    es->set(envsize) ;
}

void envwindow::floclist(gadget *g, ULONG classe, USHORT code)
{
BOOL wasglob ;
BOOL wasarch ;
optwindow *ow ;

    curenv = (enventry *)envloclist->get(code) ;
    if (curenv) {
        wasglob = curenv->isglobal() ;
        wasarch = curenv->isarchived() ;
        ow = new optwindow(10, 10, 400, 120) ;
        if (wasglob) oldsize = curenv->size ;
        ow->open(ws) ;
        ow->eventloop() ;
        if (ow->okflag)
            updateenv(curenv, wasarch, wasglob,
                    ow->carch->cursel, ow->cglob->cursel, FALSE) ;
        ow->close() ;
        delete ow ;
    }
}
void envwindow::fenvlist(gadget *g, ULONG classe, USHORT code)
{
optwindow *ow ;
BOOL wasglob ;
BOOL wasarch ;
enventry    *genv ;
BOOL        fromString = FALSE ;

    curenv = (enventry *)envlist->get(code) ;
    if (curenv) {
        wasglob = curenv->isglobal() ;
        wasarch = curenv->isarchived() ;

        ow = new optwindow(10, 10, 400, 120) ;
        ow->open(ws) ;
        ow->eventloop() ;
        if (ow->okflag) {
            if (ow->scontents) {
                fromString = TRUE ;
                delete curenv->contents ;
                if (wasglob) oldsize = curenv->size ;
                curenv->size = strlen(ow->scontents->curstring) ;
                curenv->contents = new char[curenv->size+1] ;
                strcpy(curenv->contents, ow->scontents->curstring) ;
                curenv->setentry() ;
                if (curenv->isarchived()) {
                    genv = findname(envarclist, curenv->name) ;
                    genv->size = curenv->size ;
                    delete genv->contents ;
                    genv->contents = new char[genv->size+1] ;
                    strcpy(genv->contents, curenv->contents);
                    genv->setentry() ;
                }
            }
            updateenv(curenv, wasarch, wasglob, ow->carch->cursel, ow->cglob->cursel, fromString) ;
        }
        ow->close() ;
        delete ow ;
    }
}
void envwindow::farclist(gadget *g, ULONG classe, USHORT code)
{
optwindow *ow ;
BOOL wasglob ;
BOOL wasarch ;
enventry    *genv ;
BOOL        fromString = FALSE ;

    curenv = (enventry *)envarclist->get(code) ;
    if (curenv) {
        wasglob = curenv->isglobal() ;
        wasarch = curenv->isarchived() ;

        ow = new optwindow(10, 10, 400, 120) ;
        ow->open(ws) ;
        ow->eventloop() ;
        if (ow->okflag) {
            if (ow->scontents) {
                fromString = TRUE ;
                if (wasglob) oldsize = curenv->size ;
                curenv->size = strlen(ow->scontents->curstring) ;
                curenv->contents = new char[curenv->size+1] ;
                strcpy(curenv->contents, ow->scontents->curstring) ;
                curenv->setentry() ;
                if (curenv->isglobal()) {
                    genv = findname(envlist, curenv->name) ;
                    genv->size = curenv->size ;
                    delete genv->contents ;
                    genv->contents = new char[genv->size+1] ;
                    strcpy(genv->contents, curenv->contents);
                    genv->setentry() ;
                }
            }
            updateenv(curenv, wasarch, wasglob, ow->carch->cursel, ow->cglob->cursel, fromString) ;
        }
        ow->close() ;
        delete ow ;
    }
}

void envwindow::fnew(gadget *g, ULONG classe, USHORT code)
{
STRPTR name ;
enventry *ev ;

    name = StringRequest(newvar, "") ;
    if (name ) {
        if (!strlen(name))
            ev = NULL ;
        else if (!(ev = findvar(name))) {
             if (ev = new enventry(name, nothing,
                strlen(nothing), GLOBAL_TYPE))
             envlist->enqueue(ev) ;
        }
        delete name ;
        if (ev)
            if (ev->islocale() || ev->isalias())
                floclist(NULL, NULL, ev->index()) ;
            else if (ev->isglobal())
                fenvlist(NULL, NULL, ev->index()) ;
            else
                farclist(NULL, NULL, ev->index()) ;
    }
}

void envwindow::fpurge(gadget *g, ULONG classe, USHORT code)
{
}

void envwindow::fhelp(gadget *g, ULONG classe, USHORT code)
{
}


void envwindow::handlevkey(USHORT code)
{
    g->parsegadgets(code) ;
}

void envwindow::open(screen *ns)
{
long mw ;
long nl ;
static  char titre[80] ;

    ws = ns ;
    sprintf(titre, "EnvManager © 1996, D. Lorre, %s: %s",
        process.string, proc->pr_Task.tc_Node.ln_Name) ;

    mw = ws->screen->Width - 40 ;

    // calcul du nombre de lignes par liste

    nl = (ws->screen->Height -                          // Hauteur maxi
         (ws->screen->WBorTop + ws->screen->WBorBottom +// Bords de la fenêtre
         (Tiny.tta_YSize + 12) * 3 +                    // cstrings
         4 * 3 +                                        // listes
         Normal.tta_YSize + 12 +                        // ligne du bas
         + 8)) /                                        // marge
         (3*Tiny.tta_YSize) ;

    kprintf("nl = %ld\n", nl) ;
    nl = 4 ;


    g->setfont(&Tiny) ;
    g->box(20, ws->winbarheight+8, g->ltext(local)+8, g->fontheight+4) ;
    new cstring(g, NULL, local, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL) ;


    g->box(20, g->maxh+8, mw, g->fontheight*nl+4) ;
    lloc = new listview(g, WFUNC(&envwindow::floclist), NULL, NULL, envloclist, 0, 0) ;


    g->box(20, g->maxh+8, g->ltext(global)+8, g->fontheight+4) ;
    new cstring(g, NULL, global, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL) ;

    g->box(20, g->maxh+8, mw, g->fontheight*nl+4) ;
    lglob = new listview(g, WFUNC(&envwindow::fenvlist), NULL, NULL, envlist, 0, 0) ;


    g->box(20, g->maxh+8, g->ltext(archive)+8, g->fontheight+4) ;
    new cstring(g, NULL, archive, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL) ;

    g->box(20, g->maxh+8, mw, g->fontheight*nl+4) ;
    larc = new listview(g, WFUNC(&envwindow::farclist), NULL, NULL, envarclist, 0, 0) ;
    g->setfont(&Normal) ;


    g->box(20+g->ltext(globsize)+8, g->maxh+8, g->ltext("999999")+8, g->fontheight+4) ;
    es = new cnumber(g, NULL, globsize, envsize, TRUE, PLACETEXT_LEFT|NG_HIGHLABEL) ;

    g->box(g->left+g->width+g->ltext(locsize)+16, g->top, g->ltext("999999")+8, g->fontheight+4) ;
    new cnumber(g, NULL, locsize, envlocsize, TRUE, PLACETEXT_LEFT|NG_HIGHLABEL) ;

    g->box(g->left+g->width+8, g->top, g->ltext(newentry)+8, g->fontheight+4) ;
    new fbutton(g, WFUNC(&envwindow::fnew), newentry, FALSE) ;


    width = short(g->maxw-ws->screen->WBorLeft+20) ;
    height = short(g->maxh-ws->winbarheight+8) ;


    _open(NULL, SCROLLERIDCMP|ARROWIDCMP|STRINGIDCMP|BUTTONIDCMP|
                IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW,
        WA_Title,           titre,
        WA_Gadgets,         g->glist,
        WA_DragBar,         TRUE,
        WA_DepthGadget,     TRUE,
        WA_CloseGadget,     TRUE,
        WA_Activate,        TRUE,
        TAG_DONE) ;

    if (initok) g->updategadgets() ;
}
@
