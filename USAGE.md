# OUI Library Usage Guide

This guide explains how to use the OUI (Object User Interface) library to create Amiga applications with C++.

## Table of Contents

1. [Basic Application Structure](#basic-application-structure)
2. [Memory Management](#memory-management)
3. [Screen Management](#screen-management)
4. [Window Creation](#window-creation)
5. [Gadgets and User Interface](#gadgets-and-user-interface)
6. [Event Handling](#event-handling)
7. [Localization](#localization)
8. [Complete Example](#complete-example)
9. [Compilation](#compilation)

## Basic Application Structure

Every OUI application follows this basic pattern:

```cpp
#include <exec/types.h>
#include <exec/execbase.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

// OUI includes
#include "screen.h"
#include "window.h"
#include "gadget.h"

class MyApplication : public window {
public:
    MyApplication() : window(100, 100, 400, 300) {
        // Constructor - set up window parameters
    }
    
    virtual ~MyApplication() {
        // Destructor - cleanup handled automatically
    }
    
    virtual void open(screen *s) {
        window::open(s);  // Call parent open method
        
        // Add your gadgets and setup here
        setupGadgets();
    }
    
private:
    void setupGadgets() {
        // Create and add gadgets to the window
    }
};

int main() {
    // Initialize OUI memory management
    // Create screen
    // Create and open window
    // Run event loop
    // Cleanup
}
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

OUI provides many gadget types for building user interfaces:

### Gadget List Setup
```cpp
#include "gadgetlist.h"

// Create gadget list (allocate for maximum number of gadgets)
gadgetlist *gadgets = new gadgetlist(10);  // Max 10 gadgets

// Add gadgets to the list
button *okButton = new button(gadgets, &MyWindow::fok, "OK");
button *cancelButton = new button(gadgets, &MyWindow::fcancel, "Cancel");
```

### Available Gadget Types

#### Buttons
```cpp
#include "gadgets/button.h"

// Standard button
button *btn = new button(gadgets, &MyWindow::buttonHandler, "Button Text");

// Default button (highlighted)
button *defBtn = new button(gadgets, &MyWindow::buttonHandler, "OK", TRUE);
```

#### String Gadgets
```cpp
#include "gadgets/string.h"

// String input
string *str = new string(gadgets, 100, 20, "Default Text");
```

#### Number Gadgets
```cpp
#include "gadgets/number.h"

// Number input
number *num = new number(gadgets, 0, 100, 50);  // min, max, default
```

#### Checkboxes
```cpp
#include "gadgets/checkbox.h"

// Checkbox
checkbox *cb = new checkbox(gadgets, "Checkbox Label", FALSE);
```

#### Radio Buttons
```cpp
#include "gadgets/radio.h"

// Radio button group
radio *radio1 = new radio(gadgets, "Option 1", TRUE);
radio *radio2 = new radio(gadgets, "Option 2", FALSE);
```

#### Sliders
```cpp
#include "gadgets/slider.h"

// Slider
slider *sl = new slider(gadgets, 0, 100, 50, 200);  // min, max, default, width
```

#### Cycle Gadgets
```cpp
#include "gadgets/cycle.h"

// Cycle gadget (dropdown)
const char *options[] = {"Option 1", "Option 2", "Option 3", NULL};
cycle *cyc = new cycle(gadgets, options, 0);
```

### Gadget Event Handling
```cpp
class MyWindow : public window {
protected:
    // Override gadget event handlers
    virtual void fok(gadget *g, unsigned long classe, unsigned short code) {
        // OK button pressed
        okflag = TRUE;
        active = FALSE;
    }
    
    virtual void fcancel(gadget *g, unsigned long classe, unsigned short code) {
        // Cancel button pressed
        okflag = FALSE;
        active = FALSE;
    }
    
    virtual void fapply(gadget *g, unsigned long classe, unsigned short code) {
        // Apply button pressed
        okflag = TRUE;
        applique = TRUE;
        active = FALSE;
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

OUI provides built-in localization support:

### Setting Up Localization
```cpp
#include "locale.h"

// Create catalog
catalog *myCatalog = new catalog("MyApp", "english", "francais");

// Create localized strings
lstring okText(myCatalog, "OK", "MSG_OK");
lstring cancelText(myCatalog, "Cancel", "MSG_CANCEL");
```

### Using Localized Strings
```cpp
// Use in gadgets
button *okBtn = new button(gadgets, &MyWindow::fok, okText);
button *cancelBtn = new button(gadgets, &MyWindow::fcancel, cancelText);

// Use in text output
otext(10, 10, okText);
```

## Complete Example

Here's a complete working example:

```cpp
#include <exec/types.h>
#include <exec/execbase.h>
#include <intuition/intuition.h>

#include "screen.h"
#include "window.h"
#include "gadget.h"
#include "gadgets/button.h"
#include "gadgets/string.h"

class SimpleApp : public window {
private:
    string *nameField;
    button *okButton;
    button *cancelButton;

public:
    SimpleApp() : window(100, 100, 300, 150) {
        // Constructor - window not open yet
    }
    
    virtual ~SimpleApp() {
        // Destructor
    }
    
    virtual void open(screen *s) {
        window::open(s);
        setupGadgets();
    }
    
private:
    void setupGadgets() {
        // Create gadget list
        g = new gadgetlist(3);
        
        // Create gadgets
        nameField = new string(g, 100, 20, "Enter your name");
        okButton = new button(g, &SimpleApp::fok, "OK");
        cancelButton = new button(g, &SimpleApp::fcancel, "Cancel");
    }
    
    virtual void fok(gadget *g, unsigned long classe, unsigned short code) {
        // OK button pressed
        okflag = TRUE;
        active = FALSE;
    }
    
    virtual void fcancel(gadget *g, unsigned long classe, unsigned short code) {
        // Cancel button pressed
        okflag = FALSE;
        active = FALSE;
    }
};

int main() {
    // Create screen
    screen *myScreen = new screen(NULL, "Workbench");
    
    if (!myScreen->status) {
        // Screen creation failed
        return 1;
    }
    
    // Create and open window
    SimpleApp *app = new SimpleApp();
    app->open(myScreen);
    
    if (!app->initok) {
        // Window creation failed
        delete app;
        delete myScreen;
        return 1;
    }
    
    // Run event loop
    app->eventloop();
    
    // Cleanup
    delete app;
    delete myScreen;
    
    return 0;
}
```

## Compilation

### SMakefile Example
```makefile
TARGET = MyApp
OBJS = main.o

CC = sc
LINK = slink
CFLAGS = -Iinclude: -Iinclude:OUI -Iinclude:OUI/gadgets
LDFLAGS = -Llib: -lOUI -lsc -lsmall

$(TARGET): $(OBJS)
	$(LINK) $(OBJS) -o $(TARGET) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
```

### Compilation Steps
1. **Include paths**: Add `OUI/include` and `OUI/include/gadgets` to your include path
2. **Link libraries**: Link against `oui.lib` and standard Amiga libraries
3. **Compiler**: Use SAS/C or GCC with appropriate flags

### Required Libraries
- `oui.lib` - OUI library
- `sc.lib` - SAS/C library
- `small.lib` - Small library
- Standard Amiga libraries (exec, intuition, gadtools, etc.)

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
