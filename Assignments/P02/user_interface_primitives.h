///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the header information for the 
//                   classes necessary for the various elements inside 
//                   a user interface.
//                   The user interface classes on this page should be
//                   so general that they can be used in many other programs.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

#include <Windows.h>

//#include "basewin.h"
#include <d2d1.h>

// Sometimes uncommenting one of these fixes user's include issues
// for direct2D.
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")




//////////////////////////////////////////////////////////////////////////////
//                             Parent Classes
//////////////////////////////////////////////////////////////////////////////

// Parent shape classes, such as shapes for the UI elements

// Class:   Circle
// Purpose: A class which can draw and store information relevant to a circle
// Usage:   Use the nondefault constructor to intitialize the circle.
//          Use setCoords with the new center and radius to change the 
//          circle to as desired. Coordinates are from 0 - 1 with 0
//          being the top left of the window and 1 being bottom right.
//          Note, radius is also 0-1, but goes with the smaller of
//          the height or width of the window.
//          Circles don't store color information,
//          their color is chosen on cycleCircle. cycleCircle draws
//          the circle and should ideally be called every time the 
//          screen is drawn once a frame inside the program control loop.
class Circle
{
public:
    Circle();// Mandatory.
    Circle(HWND inHandle);
    void setHWND(HWND inHandle) { hWnd = inHandle; }
    void setCoords(float inX, float inY, float inRad);
    void cycleCircle(float rValue, float gValue, float bValue, HRESULT& hr,
        ID2D1HwndRenderTarget*& pRenderTarget);
protected:
    HWND localHandle;
    float centerX = 0;
    float centerY = 0;
    float radius = 0;

    HWND hWnd;
};

// Class:   Rectangle
// Purpose: A class which can draw and store information relevant to a rect.
// Usage:   Use the nondefault constructor to intitialize the rectangle.
//          Use setCoords with the top left and bottom right coordinates of
//          the rectangle as desired. Coordinates are from 0 - 1 with 0
//          being the top left of the window and 1 being bottom right.
//          Note, radius is also 0-1, but goes with the smaller of
//          the height or width of the window.
//          Circles don't store color information,
//          their color is chosen on cycleCircle. cycleCircle draws
//          the circle and should ideally be called every time the 
//          screen is drawn once a frame inside the program control loop.
class RectangleShape
{
public:
    RectangleShape();// Mandatory.
    RectangleShape(HWND inHandle);
    void setHWND(HWND inHandle) { hWnd = inHandle; }
    void setCoords(float inTopX, float inTopY, float inBottomX, float inBottomY);
    void cycleRectangle(float rValue, float gValue, float bValue, HRESULT& hr,
        ID2D1HwndRenderTarget*& pRenderTarget);
protected:
    HWND localHandle;
    float topX = 0;
    float topY = 0;
    float bottomX = 0;
    float bottomY = 0;

    HWND hWnd;
};


//////////////////////////////////////////////////////////////////////////////
//                             Child Classes
//////////////////////////////////////////////////////////////////////////////

// Class:   RectanglularBorder
// Purpose: A class which can draw and store information relevant to a 
//          rectangle, but just the border
// Usage:   See RectangleShape
class RectangularBorder :public RectangleShape
{
public:
    RectangularBorder();// Mandatory.
    RectangularBorder(HWND inHandle);
    void cycleRectangle(float rValue, float gValue, float bValue, HRESULT& hr,
        ID2D1HwndRenderTarget*& pRenderTarget);
};

// Class:   ToggleButton
// Purpose: A class which can create a traditional web style radio button.
// Usage:   Use the nondefault constructor to intitialize like a Circle.
//          use cycleButton, passing in mouse coordinates and mouse
//          states. It returns true if clicked and stays as such
//          until once toggled off with the public variable toggled.
// Inheritance Info: See Circle
class ToggleButton : public Circle
{
public:
    ToggleButton();// Mandatory.
    ToggleButton(HWND inHandle);
    bool cycleButton(int inX, int inY, bool inClicked, HRESULT& hr,
        ID2D1HwndRenderTarget*& pRenderTarget);
    bool toggled = false;
private:
    bool boundsCheck(const int& inX, const int& inY, D2D1_SIZE_F size);
};


// Class:   TextBox
// Purpose: A class which can display text inside the bounded area.
// Usage:   Use the nondefault constructor to intitialize like a 
//          RectangleShape. Use setText to initialize or change
//          what text the TextBox will show, as well as 
//          setCoords similar to RectangleShape. Use setMinscaling
//          to determine whether the text will try to keep
//          at least a readable size. Finally, use cycleText
//          to show the text.
// Inheritance Info: See RectangleShape
class TextBox : public RectangleShape
{
public:
    TextBox();// Mandatory.
    TextBox(HWND inHandle);
    void cycleText(float rValue, float gValue, float bValue,
        HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
    void setText(std::string inText);
    void setMinScaling(bool inScale) { minScaling = inScale; }
protected:
    bool minScaling = true;
    std::string text = "";
};

// Class:   TextEnterInt
// Purpose: A class which can display text inside the bounded area
//          for an integer input. Additionally, can calculate and
//          do some bounds checking for integer text input given
//          certain keyboard and mouse states as inputs.
// Usage:   See TextBox for most placement and initialization
//          information. Instead of cycleText, use cycleBox
//          in order to perform the standard logic for the
//          button during one program processing cycle.
//          Pass the mouse state as parameters. For the cycleBox
//          inputKey char parameter, pass the key being pressed,
//          such as 0-9. 'e' and 'b' are special characters
//          which will represent "enter" and "backspace"
//          respectively, so if the input variable is
//          serving several other processes that collect key
//          state, care must be taken to avoid accidentally
//          passing in the real 'e' and 'b' keys. 
//          The function will pass in its last "Set" state,
//          as in, the last valid number either entered
//          with setText, or after the user begins typing
//          and either presses enter or clicks away from 
//          text entry field.
// Inheritance Info: See TextBox and RectangleShape
class TextEnterInt : public TextBox
{
public:
    TextEnterInt();// Mandatory.
    TextEnterInt(HWND inHandle);
    int cycleBox(char inputKey, int inX, int inY, bool inClicked,
        HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
    void setText(std::string inText);
    bool active = false;
protected:
    bool boundsCheck(const int& inX, const int& inY, D2D1_SIZE_F size);
    void processKey(std::string inputKey);
    bool clicked = false;
    std::string buffer;
};

// Class:   DropDown
// Purpose: A class which can create a traditional web style dropdown list.
// Usage:   Use the nondefault constructor to intitialize like a TextBox.
//          Use appendItem to add entries to the list. Items are immutable
//          after appending. Use cycleBox to perform a single standard cycle
//          of logic, passing in mouse state. The box will normally return
//          -1 signifying that nothing was selected that cycle.
//          If the box is first clicked, it will open. When clicked again
//          it will select the clicked item, returning its numeric 
//          position in the list, starting at 0. 
// Inheritance Info: See TextBox and RectangleShape
class Dropdown : public TextBox
{
public:
    Dropdown();// Mandatory.
    Dropdown(HWND inHandle);
    int cycleBox(int inX, int inY, bool inClicked,
        HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
    void appendItem(std::string newItem);
    bool active = false;
protected:
    int boundsCheck(const int& inX, const int& inY, D2D1_SIZE_F size);
    int focus = 0;
    bool clicked = false;
    bool collapseWait = false;
    std::vector<std::string> prompts;
};

// Class:   Button
// Purpose: A class which can create a traditional web style
//          rectangular button.
// Usage:   Use the nondefault constructor to intitialize like 
//          a RectangleShape. Button captions can be intitialized
//          like normal TextBox text.
//          use cycleButton, passing in mouse
//          coordinates and mouse states. It returns true if clicked 
//          and stays as such for only as long as it is clicked. 
//          Note, if the program passing mouse states takes no effort
//          to ensure mouses only provide a single signal per depression
//          of the mouse button, this can result in multiple true return
//          singals.
// Inheritance Info: See TextBox and RectangleShape.
class Button : public TextBox
{
    public:
        Button();// Mandatory.
        Button(HWND inHandle);
        bool cycleButton(int inX, int inY, bool inClicked,
            HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
    protected:
        int boundsCheck(const int& inX, const int& inY, D2D1_SIZE_F size);
};

// Class:   ImageAsset
// Purpose: A class which can display an image asset in the local
//          directory, or some other directory as long as the
//          proper path is given.
// Usage:   Initialize like a RectangleShape. Use the setFile
//          to pass the filename and applicable path
//          if not in the local directory to the object.
//          Use cycleImage to paint the image to that location.
// Inheritance Info: See RectangleShape.
class ImageAsset : public RectangleShape
{
    public:
        ImageAsset() {}// Mandatory
        ImageAsset(HWND inHandle) { localHandle = inHandle; }
        void setFile(std::string inFileName);
        void cycleImage(HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
    protected:
        bool imageSet = false;
        std::string fileName;
};