// $Id$
#include <exec/types.h>
#include <dos/dosextens.h>
#include <dos/var.h>
#include <libraries/gadtools.h>
#include <proto/dos.h>
#include <proto/amigaguide.h>
#include <mydebug.h>
#include <stdio.h>
#include <string.h>

#define NO_CLASS_WINDOW_INLINES
#include <screen.h>
#include <gadgets/fbutton.h>
#include <gadgets/cstring.h>
#include <gadgets/cnumber.h>
#include <gadgets/listview.h>
#include <gadgets/checkbox.h>
#include <locale.h>
#include <compiler.h>

#include "envdef.h"
#include "envwin.h"
#include "enventry.h"
#include "envopt.h"

IMPORT catalog *ecat ;

GLOBAL const lstring FAR local NPARMS(ecat, "L_ocal", "MSG_LOCAL_GAD", 0) ;
GLOBAL const lstring FAR global LPARMS(ecat, "G_lobal", "MSG_GLOBAL_GAD") ;
GLOBAL const lstring FAR archive LPARMS(ecat, "A_rchive", "MSG_ARCHIVE_GAD") ;
GLOBAL const lstring FAR locsize LPARMS(ecat, "Local Size", "MSG_LOCAL_SIZE") ;
GLOBAL const lstring FAR globsize LPARMS(ecat, "Global Size", "MSG_GLOBAL_SIZE") ;
GLOBAL const lstring FAR newentry LPARMS(ecat, "Create/Search", "MSG_CREATE_AND_SEARCH") ;
GLOBAL const lstring FAR process LPARMS(ecat, "Process", "MSG_PROCESS") ;
GLOBAL const lstring FAR help LPARMS(ecat, "_Help", "MSG_HELP_GAD") ;

GLOBAL const lstring FAR newvar NPARMS(ecat, "Var Name:", "MSG_VAR_NAME", 50) ;
GLOBAL const lstring FAR unnamed LPARMS(ecat, "UnNamed", "MSG_UNNAMED") ;
GLOBAL const lstring FAR nothing LPARMS(ecat, "Nothing", "MSG_NOTHING") ;

GLOBAL const lstring FAR alias NPARMS(ecat, "Alias", "MSG_ALIAS", 100) ;
GLOBAL const lstring FAR local2 LPARMS(ecat, "Local", "MSG_LOCAL") ;
GLOBAL const lstring FAR global2 LPARMS(ecat, "Global", "MSG_GLOBAL") ;
GLOBAL const lstring FAR archived LPARMS(ecat, "Archived", "MSG_ARCHIVED") ;
GLOBAL const lstring FAR binary LPARMS(ecat, "Binary", "MSG_BINARY") ;
GLOBAL const lstring FAR nullterm LPARMS(ecat, "Null Term", "MSG_NULL_TERM") ;
GLOBAL const lstring FAR vsize LPARMS(ecat, "Size", "MSG_SIZE") ;
GLOBAL const lstring FAR contents LPARMS(ecat, "_Contents", "MSG_CONTENTS_GAD") ;
GLOBAL const lstring FAR contents2 LPARMS(ecat, "Contents", "MSG_CONTENTS") ;
GLOBAL const lstring FAR name LPARMS(ecat, "_Name", "MSG_NAME_GAD") ;
GLOBAL const lstring FAR edit LPARMS(ecat, "_Edit", "MSG_EDIT_GAD") ;
GLOBAL const lstring FAR import LPARMS(ecat, "_Import", "MSG_IMPORT_GAD") ;
GLOBAL const lstring FAR export LPARMS(ecat, "_Export", "MSG_EXPORT_GAD") ;
GLOBAL const lstring FAR edition LPARMS(ecat, "Edition", "MSG_EDITION") ;

GLOBAL const lstring FAR importfrom NPARMS(ecat, "Import from:", "MSG_IMPORT_TITLE", 200) ;
GLOBAL const lstring FAR exportto LPARMS(ecat, "Export to:", "MSG_EXPORT_TITLE") ;

STRPTR StringRequest(STRPTR title, STRPTR def) ;

IMPORT nlist                *envloclist ;
IMPORT nlist                *envlist ;
IMPORT nlist                *envarclist ;
IMPORT TTextAttr            Tiny ;
IMPORT TTextAttr            Normal ;
IMPORT LONG                 envsize ;
IMPORT LONG                 envlocsize ;
IMPORT Process              *proc ;
IMPORT AMIGAGUIDECONTEXT    HelpHandle ;

nlist                       *curlist ;
enventry                    *curenv ;

WORD winZoom[4] = { 10, 10, 200, 11 };

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
    lenv->reset(curlist, lenv->curtop, lenv->cursel) ;
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
        if (ow->okflag) {
            updateenv(curenv, wasarch, wasglob,
                    ow->carch->cursel, ow->cglob->cursel, FALSE) ;
        }
        ow->close() ;
        delete ow ;
    }
}
void envwindow::flist(gadget *g, ULONG classe, USHORT code)
{
    if (curlist == envlist)
        globlist(code) ;
    else
        arclist(code) ;
}

void envwindow::globlist(USHORT code)
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


void envwindow::arclist(USHORT code)
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

void envwindow::fswap(gadget *g, ULONG classe, USHORT code)
{
    curlist = (code) ? envarclist : envlist ;
    lenv->reset(curlist, lenv->curtop, lenv->cursel) ;
}

void envwindow::fnew(gadget *g, ULONG classe, USHORT code)
{
STRPTR name ;
enventry *ev ;

    name = StringRequest(newvar.charptr(), "") ;
    if (name ) {
        if (!strlen(name))
            ev = NULL ;
        else if (!(ev = findvar(name))) {
             if (ev = new enventry(name, nothing.charptr(),
                strlen(nothing), GLOBAL_TYPE))
             envlist->enqueue(ev) ;
        }
        delete name ;
        if (ev)
            if (ev->islocale() || ev->isalias())
                floclist(NULL, NULL, ev->indexof()) ;
            else if (ev->isglobal())
                globlist(ev->indexof()) ;
            else
                arclist(ev->indexof()) ;
    }
}

void envwindow::fpurge(gadget *g, ULONG classe, USHORT code)
{
}

void envwindow::fhelp(gadget *g, ULONG classe, USHORT code)
{
    if (AmigaGuideBase && HelpHandle)
        SendAmigaGuideCmd(HelpHandle, "LINK MAIN", NULL) ;
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
    sprintf(titre, "EnvManager © 1997, D. Lorre, %s: %s",
        process.string, proc->pr_Task.tc_Node.ln_Name) ;

    mw = ws->scr->Width - 40 ;

    g = new gadgetlist(this, 10) ;

    // calcul du nombre de lignes par liste

    nl = (ws->scr->Height -                             // Hauteur maxi
         (ws->winbarheight + ws->scr->WBorBottom +      // Bords de la fenêtre
         (Tiny.tta_YSize*3/2 + 8) * 3 +                 // cstrings
         12 * 3 +                                       // listes
         Normal.tta_YSize*3/2 +                         // ligne du bas
         + 8)) /                                        // marge
         (3*Tiny.tta_YSize) ;

    g->setfont(&Tiny) ;
    g->box(20, ws->winbarheight+8, g->ltext(local)+8, g->fontheight*3/2) ;
    new cstring(g, NULL, local, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL) ;

    g->box(20, g->maxh+4, mw, g->fontheight*nl+4) ;
    lloc = new listview(g, WFUNC(&envwindow::floclist), NULL, NULL, envloclist, 0, 0) ;

    curlist = envlist ;
    g->box(20, g->maxh+8, (g->fontheight*3*ws->xratio)/(ws->yratio*2), g->fontheight*3/2) ;
    new checkbox(g, WFUNC(&envwindow::fswap), archive, FALSE, PLACETEXT_RIGHT|NG_HIGHLABEL) ;

    g->box(20, g->maxh+4, mw, g->fontheight*nl*2+4) ;
    lenv = new listview(g, WFUNC(&envwindow::flist), NULL, NULL, envlist, 0, 0) ;
    g->setfont(&Normal) ;

    g->box(20+g->ltext(globsize)+8, g->maxh+8, g->ltext("999999")+8, g->fontheight*3/2) ;
    es = new cnumber(g, NULL, globsize, envsize, TRUE, PLACETEXT_LEFT|NG_HIGHLABEL) ;

    g->box(g->left+g->width+g->ltext(locsize)+16, g->top, g->ltext("999999")+8, g->fontheight*3/2) ;
    new cnumber(g, NULL, locsize, envlocsize, TRUE, PLACETEXT_LEFT|NG_HIGHLABEL) ;

    g->box(g->left+g->width+8, g->top, g->ltext(newentry)+8, g->fontheight*3/2) ;
    new fbutton(g, WFUNC(&envwindow::fnew), newentry, FALSE) ;

    if (AmigaGuideBase && HelpHandle) {
        g->box(g->left+g->width+8, g->top, g->ltext(help)+8, g->fontheight*3/2) ;
        new fbutton(g, WFUNC(&envwindow::fhelp), help, FALSE) ;
    }

    width = short(g->maxw-ws->scr->WBorLeft+20) ;
    height = short(g->maxh-ws->winbarheight+8) ;

    winZoom[2] = g->ltext(titre) + 120 ; // about 40 pixels per (3) sysim
    winZoom[3] = ns->winbarheight ;

    _open(NULL, SCROLLERIDCMP|ARROWIDCMP|STRINGIDCMP|BUTTONIDCMP|
                IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW,
        WA_Title,           titre,
        WA_Gadgets,         g->glist,
        WA_DragBar,         TRUE,
        WA_DepthGadget,     TRUE,
        WA_CloseGadget,     TRUE,
        WA_Activate,        TRUE,
        WA_Zoom,            winZoom,
        TAG_DONE) ;

    if (initok) g->updategadgets() ;
}
