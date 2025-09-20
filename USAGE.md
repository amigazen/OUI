# OUI Library Usage Guide

This guide explains how to use the OUI (Object User Interface) library to create Amiga applications with C++. Based on the EnvManager example application.

## Table of Contents

1. [Basic Application Structure](#basic-application-structure)
2. [Memory Management](#memory-management)
3. [Screen Management](#screen-management)
4. [Window Creation](#window-creation)
5. [Gadgets and User Interface](#gadgets-and-user-interface)
6. [Event Handling](#event-handling)
7. [Localization](#localization)
8. [Advanced Features](#advanced-features)
9. [Complete Example](#complete-example)
10. [Compilation](#compilation)

## Basic Application Structure

Every OUI application follows this pattern, as demonstrated in the EnvManager example:

```cpp
#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
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

// Your application includes
#include "envdef.h"
#include "enventry.h"
#include "envwin.h"

// Global variables for OUI
screen *ns;
catalog *ecat;

// Library initialization functions
void _STI_200_initlibs() {
    LocaleBase = OpenLibrary((UBYTE *)"locale.library", 0);
    AmigaGuideBase = OpenLibrary((UBYTE *)"amigaguide.library", 39);
}

void _STD_200_closelibs() {
    if (LocaleBase) CloseLibrary(LocaleBase);
    if (AmigaGuideBase) CloseLibrary(AmigaGuideBase);
}

extern "C" {
void _STI_4500_initcat() {
    ecat = new catalog("myapp.catalog", "english", NULL);
}

void _STD_4500_closecat() {
    delete ecat;
}
}

int main() {
    // Create screen
    ns = new screen(NULL, "Workbench");
    if (ns && ns->status) {
        // Create and open main window
        MyWindow *app = new MyWindow(10, 10, 500, 120);
        app->open(ns);
        if (app->initok) {
            app->eventloop();
            app->close();
        }
        delete app;
    }
    delete ns;
    return 0;
}

#if defined( __GNUG__ )
ADD2INIT(_STI_4500_initcat, -40);
ADD2EXIT(_STD_4500_closecat,-40);
#endif
```

## Memory Management

OUI provides its own memory management system that's more robust than standard C++ for AmigaOS:

### Key Features
- **Pool-based allocation**: More efficient than malloc/free
- **Oxygene feature**: Emergency memory reserve for safe shutdown
- **Automatic cleanup**: Memory is freed on program exit

### Usage
```cpp
#include "new.h"  // OUI memory management

// Memory is automatically managed
// Use normal C++ new/delete - OUI handles the rest
MyClass *obj = new MyClass();
// ... use obj ...
delete obj;  // Optional - will be cleaned up on exit anyway
```

### Important Notes
- **Not compatible with memoration**: OUI's memory system conflicts with memoration testing
- **Automatic cleanup**: All allocated memory is freed on program exit
- **Emergency reserve**: OUI maintains a small memory pool for safe shutdown

## Screen Management

Every OUI application needs a screen before creating windows:

### Creating a Screen
```cpp
#include "screen.h"

// Method 1: Public screen
screen *myScreen = new screen(NULL, "Workbench");

// Method 2: Custom screen with extra pens
UWORD extraPens[] = {GREEN_PEN, RED_PEN, WHITE_PEN, BLACK_PEN};
screen *myScreen = new screen(extraPens, "MyApp");

// Method 3: Using TagItems
TagItem screenTags[] = {
    SA_Width, 640,
    SA_Height, 480,
    SA_Depth, 4,
    TAG_END
};
screen *myScreen = new screen(NULL, screenTags);
```

### Screen Properties
```cpp
// Access screen properties
if (myScreen->status) {
    // Screen opened successfully
    int width = myScreen->width;
    int height = myScreen->height;
    int xratio = myScreen->xratio;  // Scaling factor
    int yratio = myScreen->yratio;
    
    // Get RastPort for drawing
    RastPort *rp = myScreen->rp;
    
    // Get font
    TextFont *font = myScreen->font;
}
```

### Screen Management
```cpp
// Bring screen to front
myScreen->front();

// Send to back
myScreen->back();

// Play beep
myScreen->beep();
```

## Window Creation

OUI uses a two-step window creation process:

### Step 1: Constructor
```cpp
class MyWindow : public window {
public:
    MyWindow() : window(left, top, width, height) {
        // Set up window parameters
        // Don't open the window yet
    }
};
```

### Step 2: Open Method
```cpp
void MyWindow::open(screen *s) {
    // Call parent open method
    window::open(s);
    
    // Now the window is open and ready for gadgets
    setupGadgets();
}
```

### Window Properties and Methods
```cpp
// Window state
if (initok) {
    // Window opened successfully
}

// Window management
move(dx, dy);           // Move window
size(dw, dh);           // Resize window
box(x, y, w, h);        // Set position and size
limits(wmin, hmin, wmax, hmax);  // Set size limits
titles("Window Title", "Screen Title");  // Set titles

// Window visibility
activate();             // Activate window
front();                // Bring to front
back();                 // Send to back
```

## Gadgets and User Interface

OUI provides many gadget types for building user interfaces. The EnvManager example shows practical usage:

### Gadget List Setup
```cpp
#include "gadgetlist.h"

// In your window's open() method
g = new gadgetlist(this, 10);  // Max 10 gadgets for this window

// Set font for gadgets
g->setfont(&Tiny);  // or &Normal

// Position gadgets using box() method
g->box(20, ws->winbarheight+8, g->ltext(local)+8, g->fontheight*3/2);
new cstring(g, NULL, local, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL);

// Update gadgets after creation
if (initok) g->updategadgets();
```

### Available Gadget Types

#### Buttons
```cpp
#include "gadgets/fbutton.h"

// Standard button
new fbutton(g, WFUNC(&MyWindow::buttonHandler), "Button Text", FALSE);

// Default button (highlighted)
new fbutton(g, WFUNC(&MyWindow::buttonHandler), "OK", TRUE);
```

#### String Gadgets
```cpp
#include "gadgets/string.h"
#include "gadgets/cstring.h"

// String input with label
new cstring(g, NULL, "Label:", NULL, FALSE, PLACETEXT_IN);
new string(g, NULL, NULL, "Default Text", 50);

// String input without label
new string(g, WFUNC(&MyWindow::stringHandler), NULL, "Default Text", 50);
```

#### Number Gadgets
```cpp
#include "gadgets/cnumber.h"

// Number input with label
new cnumber(g, NULL, "Size:", 50, TRUE, PLACETEXT_LEFT|NG_HIGHLABEL);
```

#### Checkboxes
```cpp
#include "gadgets/checkbox.h"

// Checkbox with label
new checkbox(g, WFUNC(&MyWindow::checkboxHandler), "Checkbox Label", FALSE, PLACETEXT_LEFT);

// Checkbox without handler (read-only)
new checkbox(g, NULL, "Read Only", TRUE, PLACETEXT_LEFT, TRUE);
```

#### List Views
```cpp
#include "gadgets/listview.h"

// List view showing data from nlist
listview *lloc = new listview(g, WFUNC(&MyWindow::listHandler), NULL, NULL, 
                              myList, 0, 0);
```

#### Specialized Gadgets
```cpp
// Custom string gadget with specific behavior
cstring *label = new cstring(g, NULL, "Label:", NULL, FALSE, PLACETEXT_IN);
string *input = new string(g, NULL, NULL, defaultValue, maxLength);
```

### Gadget Event Handling
```cpp
class MyWindow : public window {
protected:
    // Custom gadget event handlers
    void flist(gadget *g, ULONG classe, USHORT code) {
        // List view item selected
        curenv = (enventry *)myList->get(code);
        if (curenv) {
            // Handle selection
        }
    }
    
    void fnew(gadget *g, ULONG classe, USHORT code) {
        // Create new item button pressed
        STRPTR name = StringRequest("Enter Name:", "");
        if (name) {
            // Process new item
            delete name;
        }
    }
    
    void fhelp(gadget *g, ULONG classe, USHORT code) {
        // Help button pressed
        if (AmigaGuideBase && HelpHandle)
            SendAmigaGuideCmd(HelpHandle, "LINK MAIN", NULL);
    }
    
    // Override keyboard handling
    virtual void handlevkey(USHORT code) {
        if (code == 0x0D) {  // Enter
            g->selectgadget(okid, FALSE);
        }
        else if (code == 0x1B) {  // Escape
            g->selectgadget(cancelid, FALSE);
        }
        else {
            g->parsegadgets(code);
        }
    }
};
```

## Event Handling

OUI provides comprehensive event handling:

### Event Loop
```cpp
// Main event loop
void MyWindow::eventloop() {
    while (active) {
        // Process events
        processevents();
    }
}
```

### Event Processing
```cpp
virtual void simpleevent(IntuiMessage *msg) {
    switch (msg->Class) {
        case IDCMP_GADGETUP:
            // Gadget was released
            break;
        case IDCMP_GADGETDOWN:
            // Gadget was pressed
            break;
        case IDCMP_CLOSEWINDOW:
            // Close window requested
            active = FALSE;
            break;
        case IDCMP_RAWKEY:
            // Raw key input
            handlevkey(msg->Code);
            break;
    }
}
```

### Keyboard Handling
```cpp
virtual void handlevkey(USHORT code) {
    switch (code) {
        case 0x0D:  // Enter
            // Handle Enter key
            break;
        case 0x1B:  // Escape
            // Handle Escape key
            break;
        default:
            // Pass to gadgets
            g->parsegadgets(code);
            break;
    }
}
```

## Localization

OUI provides built-in localization support with catalog system:

### Setting Up Localization
```cpp
#include "locale.h"

// Global catalog variable
catalog *ecat;

// Initialize catalog in library init function
extern "C" {
void _STI_4500_initcat() {
    ecat = new catalog("myapp.catalog", "english", NULL);
}

void _STD_4500_closecat() {
    delete ecat;
}
}

// Define localized strings using macros
GLOBAL const lstring FAR local NPARMS(ecat, "L_ocal", "MSG_LOCAL_GAD", 0);
GLOBAL const lstring FAR global LPARMS(ecat, "G_lobal", "MSG_GLOBAL_GAD");
GLOBAL const lstring FAR archive LPARMS(ecat, "A_rchive", "MSG_ARCHIVE_GAD");
GLOBAL const lstring FAR newentry LPARMS(ecat, "Create/Search", "MSG_CREATE_AND_SEARCH");
```

### Using Localized Strings
```cpp
// Use in gadgets
new cstring(g, NULL, local, NULL, FALSE, PLACETEXT_IN|NG_HIGHLABEL);
new fbutton(g, WFUNC(&MyWindow::fnew), newentry, FALSE);

// Use in window titles
sprintf(titre, "MyApp - %s: %s", process.string, proc->pr_Task.tc_Node.ln_Name);
```

### Catalog File Structure
The catalog system uses `.catalog` files with specific format:
- Contains localized strings for different languages
- Supports multiple languages (English, French, German, Spanish, etc.)
- Uses CatComp format for compatibility

## Advanced Features

### String Request Dialogs
```cpp
#include "stringreq.h"

// Create a string request dialog
STRPTR StringRequest(STRPTR title, STRPTR def) {
    STRPTR retstr = NULL;
    rstring rs(120, 20, 400, 70);
    
    rs.def = def;
    rs.titre = title;
    rs.open(ns);
    rs.eventloop();
    rs.close();
    
    if (rs.okflag) {
        retstr = new char[strlen(rs.sname)+1];
        strcpy(retstr, rs.sname);
    }
    return retstr;
}

// Use the dialog
STRPTR name = StringRequest("Enter Name:", "");
if (name) {
    // Process the input
    delete name;
}
```

### File Operations with ASL
```cpp
#include <proto/asl.h>

// File requester setup
BOOL OpenAsl(void) {
    AslBase = OpenLibrary((UBYTE *)"asl.library", 0);
    return (AslBase) ? TRUE : FALSE;
}

FileRequester *InitAslFileReq(screen *s, STRPTR drawer, STRPTR title) {
    FileRequester *fr = NULL;
    
    if (OpenAsl()) {
        fr = (FileRequester *)AllocAslRequestTags(ASL_FileRequest,
            ASLFR_Screen,           s->scr,
            ASLFR_RejectIcons,      TRUE,
            ASLFR_TitleText,        title,
            ASLFR_InitialDrawer,    drawer,
            TAG_DONE);
    }
    return fr;
}

// File selection
BOOL getfile(screen *s, STRPTR name, STRPTR drawer, STRPTR pat, STRPTR title, BOOL savemode) {
    FileRequester *fr;
    BOOL retval = FALSE;
    
    if (fr = InitAslFileReq(s, drawer, title)) {
        if (AslRequestTags(fr,
            ASLFR_InitialFile,      name,
            ASLFR_InitialPattern,   pat,
            ASLFR_DoSaveMode,       savemode,
            ASLFR_DoPatterns,       TRUE,
            TAG_DONE)) {
            strcpy(name, fr->fr_File);
            strcpy(drawer, fr->fr_Drawer);
            retval = TRUE;
        }
        FreeAslRequest(fr);
        CloseAsl();
    }
    return retval;
}
```

### AmigaGuide Integration
```cpp
#include <proto/amigaguide.h>

// Global variables
Library *AmigaGuideBase;
AMIGAGUIDECONTEXT HelpHandle;
ULONG asig;
NewAmigaGuide nag = {NULL};
char basename[80];

// Initialize AmigaGuide
if (AmigaGuideBase) {
    sprintf(basename, "MyApp.guide");
    nag.nag_Name = basename;
    nag.nag_Node = "MAIN";
    nag.nag_Line = 0;
    nag.nag_PubScreen = NULL;
    CacheClearU();
    if (HelpHandle = OpenAmigaGuideAsync(&nag, NULL)) {
        asig = AmigaGuideSignal(HelpHandle);
    }
}

// Open help
void fhelp(gadget *g, ULONG classe, USHORT code) {
    if (AmigaGuideBase && HelpHandle)
        SendAmigaGuideCmd(HelpHandle, "LINK MAIN", NULL);
}
```

### Window Zoom Support
```cpp
// Define zoom dimensions
WORD winZoom[4] = { 10, 10, 200, 11 };

// In window open method
winZoom[2] = g->ltext(titre) + 120;  // Calculate width
winZoom[3] = ns->winbarheight;       // Height

// Add to window tags
_open(NULL, SCROLLERIDCMP|ARROWIDCMP|STRINGIDCMP|BUTTONIDCMP|
            IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW,
    WA_Title,           titre,
    WA_Gadgets,         g->glist,
    WA_DragBar,         TRUE,
    WA_DepthGadget,     TRUE,
    WA_CloseGadget,     TRUE,
    WA_Activate,        TRUE,
    WA_Zoom,            winZoom,
    TAG_DONE);
```

### Dynamic Layout Calculation
```cpp
// Calculate layout based on screen dimensions
long mw = ws->scr->Width - 40;
long nl = (ws->scr->Height - 
          (ws->winbarheight + ws->scr->WBorBottom + 
          (Tiny.tta_YSize*3/2 + 8) * 3 + 
          12 * 3 + 
          Normal.tta_YSize*3/2 + 
          + 8)) / 
         (3*Tiny.tta_YSize);

// Use calculated values for gadget positioning
g->box(20, g->maxh+4, mw, g->fontheight*nl+4);
```

## Complete Example

Here's a simplified example based on the EnvManager structure:

```cpp
// main.cc - Main application file
#include <exec/types.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <graphics/gfxbase.h>
#include <string.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <mydebug.h>
#include <compiler.h>

#define NO_CLASS_WINDOW_INLINES
#include <screen.h>
#include <locale.h>

#include "myapp.h"

// Global variables
screen *ns;
catalog *ecat;

// Library initialization
void _STI_200_initlibs() {
    LocaleBase = OpenLibrary((UBYTE *)"locale.library", 0);
}

void _STD_200_closelibs() {
    if (LocaleBase) CloseLibrary(LocaleBase);
}

extern "C" {
void _STI_4500_initcat() {
    ecat = new catalog("myapp.catalog", "english", NULL);
}

void _STD_4500_closecat() {
    delete ecat;
}
}

int main() {
    // Create screen
    ns = new screen(NULL, "Workbench");
    if (ns && ns->status) {
        // Create and open main window
        MyApp *app = new MyApp(10, 10, 400, 200);
        app->open(ns);
        if (app->initok) {
            app->eventloop();
            app->close();
        }
        delete app;
    }
    delete ns;
    return 0;
}

#if defined( __GNUG__ )
ADD2INIT(_STI_4500_initcat, -40);
ADD2EXIT(_STD_4500_closecat,-40);
#endif
```

```cpp
// myapp.h - Application header
#ifndef MYAPP_H
#define MYAPP_H

#include <window.h>
#include <gadgetlist.h>
#include <gadgets/fbutton.h>
#include <gadgets/string.h>
#include <gadgets/cstring.h>
#include <locale.h>

// Localized strings
IMPORT catalog *ecat;
GLOBAL const lstring FAR okText LPARMS(ecat, "OK", "MSG_OK");
GLOBAL const lstring FAR cancelText LPARMS(ecat, "Cancel", "MSG_CANCEL");
GLOBAL const lstring FAR nameLabel LPARMS(ecat, "Name:", "MSG_NAME");

class MyApp : public window {
private:
    string *nameField;
    fbutton *okButton;
    fbutton *cancelButton;

public:
    MyApp(short l, short t, short w, short h) : window(l,t,w,h) {}
    virtual ~MyApp() {}
    
    virtual void open(screen *ns);
    virtual void handlevkey(USHORT code);
    
private:
    void fok(gadget *g, ULONG classe, USHORT code);
    void fcancel(gadget *g, ULONG classe, USHORT code);
};

#endif
```

```cpp
// myapp.cc - Application implementation
#include "myapp.h"

void MyApp::open(screen *ns) {
    ws = ns;
    g = new gadgetlist(this, 4);
    
    // Position gadgets
    g->box(20, ws->winbarheight+20, g->ltext(nameLabel)+8, g->fontheight+4);
    new cstring(g, NULL, nameLabel, NULL, FALSE, PLACETEXT_IN);
    
    g->box(20, g->maxh+8, 200, g->fontheight+4);
    nameField = new string(g, NULL, NULL, "Enter your name", 50);
    
    g->box(20, g->maxh+20, g->ltext(okText)+8, g->fontheight+4);
    okButton = new fbutton(g, WFUNC(&MyApp::fok), okText, FALSE);
    
    g->box(100, g->maxh+20, g->ltext(cancelText)+8, g->fontheight+4);
    cancelButton = new fbutton(g, WFUNC(&MyApp::fcancel), cancelText, FALSE);
    
    // Open window
    prepbox(TRUE);
    _open(NULL, BUTTONIDCMP|IDCMP_VANILLAKEY|IDCMP_CLOSEWINDOW,
        WA_Title,           "My OUI Application",
        WA_Gadgets,         g->glist,
        WA_DragBar,         TRUE,
        WA_CloseGadget,     TRUE,
        WA_Activate,        TRUE,
        TAG_DONE);
    
    if (initok) g->updategadgets();
}

void MyApp::handlevkey(USHORT code) {
    if (code == 0x0D) {  // Enter
        g->selectgadget(okButton->id, FALSE);
    }
    else if (code == 0x1B) {  // Escape
        g->selectgadget(cancelButton->id, FALSE);
    }
    else {
        g->parsegadgets(code);
    }
}

void MyApp::fok(gadget *g, ULONG classe, USHORT code) {
    // Process the name field
    printf("Name entered: %s\n", nameField->curstring);
    okflag = TRUE;
    active = FALSE;
}

void MyApp::fcancel(gadget *g, ULONG classe, USHORT code) {
    okflag = FALSE;
    active = FALSE;
}
```

## Compilation

### SMakefile Example
```makefile
# Makefile for OUI application
OBJS= main.o myapp.o
LIBS= LIB:debug.lib ULIB:oui.lib LIB:scm.lib\
       LIB:sc.lib LIB:amiga.lib

.cc.o:
    sc $< srcis=$>.cc

myapp: $(OBJS) ULIB:oui.lib
    slink with <<
    LIB:c.o
    $(OBJS)
    LIB $(LIBS)
    SMALLCODE
    SMALLDATA
    STRIPDEBUG
    BATCH
    VERBOSE
    NOICONS
    DEFINE @__chkabort=@__dummy
    DEFINE __CXM33=__UCXM33
    DEFINE __CXD33=__UCXD33
    DEFINE __CXM22=__UCXM22
    DEFINE __CXD22=__UCXD22
<

# Dependencies
main.o: main.cc myapp.h
myapp.o: myapp.cc myapp.h
```

### GNU Makefile Example
```makefile
O = ./GNU/mc68000/
CXXFLAGS = -noixemul
CPPFLAGS = -I ../oui/include/
OBJS= $(O)main.o $(O)myapp.o

myapp: $(OBJS)
	g++ -noixemul -o myapp $(OBJS) -loui -ldebug

$(O)main.o: main.cc myapp.h
$(O)myapp.o: myapp.cc myapp.h
```

### Compilation Steps
1. **Include paths**: Add `OUI/include` and `OUI/include/gadgets` to your include path
2. **Link libraries**: Link against `oui.lib` and standard Amiga libraries
3. **Compiler**: Use SAS/C or GCC with appropriate flags
4. **Library initialization**: Use `ADD2INIT` and `ADD2EXIT` macros for GCC

### Required Libraries
- `oui.lib` - OUI library (in ULIB:)
- `debug.lib` - Debug library (in LIB:)
- `scm.lib` - SAS/C math library (in LIB:)
- `sc.lib` - SAS/C library (in LIB:)
- `amiga.lib` - Amiga library (in LIB:)
- Standard Amiga libraries (exec, intuition, gadtools, etc.)

### Compiler-Specific Notes
- **SAS/C**: Use `sc` compiler with `srcis=` parameter
- **GCC**: Use `g++` with `-noixemul` flag
- **Library initialization**: Required for proper OUI memory management
- **Debug builds**: Include `debug.lib` for debugging support

## Tips and Best Practices

1. **Always check status**: Check `screen->status` and `window->initok` before proceeding
2. **Use virtual functions**: Override virtual functions for custom behavior
3. **Memory management**: Let OUI handle memory - don't mix with standard malloc/free
4. **Event handling**: Use the provided event system rather than raw Intuition
5. **Gadget lists**: Pre-allocate gadget lists with maximum expected gadgets
6. **Error handling**: Check return values and handle errors gracefully
7. **Cleanup**: OUI handles most cleanup automatically, but delete objects when done

## Troubleshooting

### Common Issues
- **Screen won't open**: Check screen name and available memory
- **Window won't open**: Ensure screen is valid and window parameters are correct
- **Gadgets not responding**: Check gadget list allocation and event handling
- **Memory issues**: Avoid mixing OUI memory management with standard C++

### Debug Tips
- Use `myScreen->beep()` to test if screen is working
- Check `initok` flags after creating screens and windows
- Use OUI's built-in error reporting
- Test with simple examples before complex applications

This guide should get you started with OUI development. For more advanced features, refer to the source code and example applications.
