// $Id$
#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <dos/var.h>
#include <graphics/gfxbase.h>
#include <diskfont/diskfont.h>
#include <string.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/amigaguide.h>
#include <mydebug.h>
#include <compiler.h>

#define NO_CLASS_WINDOW_INLINES
#include <screen.h>
#include <locale.h>

#include "envdef.h"
#include "enventry.h"
#include "envwin.h"
#include "envman_rev.h"

nlist *envlist ;
nlist *envarclist ;
nlist *envloclist ;

LONG envsize = 0 ;
LONG envlocsize = 0 ;
Library *ConsoleDevice ;
Process *proc ;
screen *ns ;
char NormalName[MAXFONTNAME] = "Times.font" ;
char TinyName[MAXFONTNAME] = "Helvetica.font" ;
TTextAttr Normal ;
TTextAttr Tiny ;
char *version = VSTRING ;

catalog *ecat ;


envwindow *ew ;

ULONG               asig ;
AMIGAGUIDECONTEXT   HelpHandle ;
NewAmigaGuide       nag = {NULL};
char                basename[80] ;
AmigaGuideMsg       *amsg;
BOOL                amgok=FALSE;


Library __near *LocaleBase ;
Library *AmigaGuideBase ;

void _STI_200_initlibs()
{
    LocaleBase = OpenLibrary((UBYTE *)"locale.library", 0) ;
    AmigaGuideBase = OpenLibrary((UBYTE *)"amigaguide.library", 39) ;
}

void _STD_200_closelibs()
{
    if (LocaleBase) CloseLibrary(LocaleBase) ;
    if (AmigaGuideBase) CloseLibrary(AmigaGuideBase) ;
}


extern "C" {
void _STI_4500_initcat()
{
        ecat = new catalog("envman.catalog", "english", NULL) ;
}


void _STD_4500_closecat()
{
    delete ecat ;
}
}


void initlist(STRPTR name, UWORD flags, nlist *l, char type, char *prevpath)
{
ExAllData       *EAData ;
ExAllData       *ead ;
ExAllControl    *eac ;
BOOL            more ;
char            locname[MAXSIZE] ;
char            fname[MAXSIZE] ;
char            *contents ;
char            path[MAXSIZE] ;
BPTR            clef ;
BPTR            file ;

    strcpy(path, name) ;
    if (prevpath) {
        AddPart(path, prevpath, MAXSIZE-1) ;
    }
    clef = Lock(path, ACCESS_READ);

    EAData = new ExAllData[MAXENTRIES] ;

    eac = (ExAllControl *)AllocDosObject(DOS_EXALLCONTROL,NULL);
    eac->eac_Entries = MAXENTRIES ;
    eac->eac_MatchString = NULL ;
    eac->eac_MatchFunc = NULL ;
    eac->eac_LastKey = 0;
    do {
        more = (BOOL)ExAll(clef, EAData, sizeof(ExAllData)*MAXENTRIES, ED_SIZE, eac);
        if ((!more) && (IoErr() != ERROR_NO_MORE_ENTRIES)) {
            // ExAll failed abnormally
            break;
        }
        if (eac->eac_Entries == 0) {
            // ExAll failed normally with no entries
            continue;                   // ("more" is *usually* zero)
        }
        ead = EAData;
        do {
            if (prevpath) {
                strcpy(locname, prevpath) ;
                AddPart(locname, (STRPTR)ead->ed_Name, MAXSIZE-1) ;
            }
            else  {
                strncpy(locname, (char *)ead->ed_Name, MAXSIZE-1) ;
                locname[MAXSIZE-1] = '\0' ;
            }
            if (ead->ed_Type>0) {
                initlist(name, flags, l, type, locname) ;
            }
            else {
                contents = new char[ead->ed_Size+1] ;
                strcpy(fname, name) ;
                AddPart(fname, locname, MAXSIZE-1) ;
                if (file = Open(fname, MODE_OLDFILE)) {
                    Read(file, contents, ead->ed_Size) ;
                    Close(file) ;
                    if (!(flags & GVF_SAVE_VAR))
                        envsize += ead->ed_Size ;
                    l->enqueue(new enventry(locname, contents, ead->ed_Size, type)) ;
                }
                delete contents ;
            }
            ead = ead->ed_Next;
        } while (ead);

    } while (more);

    FreeDosObject(DOS_EXALLCONTROL,eac);
    delete EAData ;
    UnLock(clef);
}

void initloclist(Process *p)
{
LocalVar    *lv ;
char        locname[MAXSIZE] ;

    lv = (LocalVar *)p->pr_LocalVars.mlh_Head ;
    while (lv->lv_Node.ln_Succ) {
        strcpy(locname, lv->lv_Node.ln_Name) ;
        envloclist->enqueue(new enventry(locname, (char *)lv->lv_Value,
            lv->lv_Len, (lv->lv_Node.ln_Type & LV_ALIAS)?ALIAS_TYPE:LOCAL_TYPE)) ;
        envlocsize += sizeof(LocalVar) + lv->lv_Len ;

        lv = (LocalVar *)lv->lv_Node.ln_Succ ;
    }
}


void main(void)
{
    ns = new screen(NULL, "Workbench") ;
    if (ns && ns->status) {
        proc = (Process *)FindTask(NULL) ;

        envlist = new nlist ;
        initlist("ENV:", GVF_GLOBAL_ONLY|GVF_BINARY_VAR|GVF_DONT_NULL_TERM, envlist, GLOBAL_TYPE, NULL) ;
        envarclist = new nlist ;
        initlist("ENVARC:", GVF_GLOBAL_ONLY|GVF_SAVE_VAR|GVF_BINARY_VAR|GVF_DONT_NULL_TERM, envarclist, ARCHIVED_TYPE, NULL) ;

        envloclist = new nlist ;
        initloclist(proc) ;

        strcpy((char *)NormalName, (char *)ns->scr->Font->ta_Name) ;
        Normal.tta_Name = NormalName ;
        Normal.tta_YSize = ns->scr->Font->ta_YSize ;
        Normal.tta_Style = ns->scr->Font->ta_Style ;
        Normal.tta_Flags = ns->scr->Font->ta_Flags ;

        strcpy((char *)TinyName, (char *)GfxBase->DefaultFont->tf_Message.mn_Node.ln_Name) ;
        Tiny.tta_Name = TinyName ;
        Tiny.tta_YSize = GfxBase->DefaultFont->tf_YSize ;
        Tiny.tta_Style = GfxBase->DefaultFont->tf_Style ;
        Tiny.tta_Flags = GfxBase->DefaultFont->tf_Flags ;

        if (AmigaGuideBase) {
            sprintf(basename, "Envman.guide") ;
            nag.nag_Name = basename ;
            nag.nag_Node = "MAIN" ;
            nag.nag_Line = 0;
            nag.nag_PubScreen = NULL ;
            CacheClearU() ;
            if (HelpHandle = OpenAmigaGuideAsync(&nag, NULL)) {
                asig = AmigaGuideSignal(HelpHandle) ;
            }
        }

        if (ew = new envwindow(10, 10, 500, 120)) {
            ew->open(ns) ;
            if (ew->initok) {
                ew->eventloop() ;
                ew->close() ;
            }
            delete ew ;
        }
        if (AmigaGuideBase && HelpHandle) {
            CloseAmigaGuide(HelpHandle) ;
        }
        delete envlist ;
        delete envarclist ;
        delete envloclist ;

    }
    delete ns ;

}

#if defined( __GNUG__ )
ADD2INIT(_STI_4500_initcat, -40);
ADD2EXIT(_STD_4500_closecat,-40);
#endif
