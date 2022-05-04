///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the implementation details for the 
//                   user interface class along with other more general
//                   classes for the objects inside of a user interface, 
//                   such as a numeric input textbox, along with
//                   some basic shapes used as parent classes to create
//                   said items, such as a Circle class.
// Credit:           Much of the code on this page is heavily based upon
//                   code from Microsoft's online documentation and tutorial
//                   code for Windows.h and Direct 2D found here:
//                      https://docs.microsoft.com/en-us/windows/win32/
//                      learnwin32/learn-to-program-for-windows
//////////////////////////////////////////////////////////////////////////////

#pragma once

// Ensures UNICODE precompiler directive is activated
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif
#ifndef UNICODE
#define UNICODE
#endif


#include "user_interface_primitives.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <cctype>

// Precompiler directive in case anyone wants to modify relevant
// code to work for Mac or Linux builds. Commented out for now
// because it interfered with the warning squiggles that show up
// before you compile, which is likely a VS bug.
//#define WINDOWSRELEASE
//#ifdef WINDOWSRELEASE
#include <Windows.h>
#include <Windowsx.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

//#include "basewin.h"
#include <d2d1.h>
#include <dwmapi.h>


// Sometimes uncommenting one of these fixes user's include issues.
// Use the linker in the command line and comment these out
// if your compiler doesn't support pragma comment.
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Dwmapi")

#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <Unknwn.h>



//////////////////////////////////////////////////////////////////////////////
//                             Parent Classes
//////////////////////////////////////////////////////////////////////////////

// Circle
Circle::Circle()
{
    // Only exists to be replaced by calling below constructor.
}
Circle::Circle(HWND inHandle)
{
    hWnd = inHandle;
}
void Circle::setCoords(float inX, float inY, float inRad)
{
    centerX = inX;
    centerY = inY;
    radius = inRad;
}
void Circle::cycleCircle(float rValue, float gValue, float bValue, HRESULT& hr,
    ID2D1HwndRenderTarget*& pRenderTarget)
{
    D2D1_ELLIPSE ellipse;
    ID2D1SolidColorBrush* pBrush = NULL;
    hr = S_OK;
    const D2D1_COLOR_F color = D2D1::ColorF(rValue, gValue, bValue);
    hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float x = size.width * centerX;
    const float y = size.height * centerY;
    const float adjustedRadius = min(size.width, size.height) * radius;
    ellipse = D2D1::Ellipse(D2D1::Point2F(x, y),
        adjustedRadius, adjustedRadius);

    if (SUCCEEDED(hr))
    {
        pRenderTarget->FillEllipse(ellipse, pBrush);
    }
    (*&pBrush)->Release();
}


// Rectangle
RectangleShape::RectangleShape()
{
    // Only exists to be replaced by calling below constructor.
}
RectangleShape::RectangleShape(HWND inHandle)
{
    hWnd = inHandle;
}
void RectangleShape::setCoords(float inTopX, float inTopY, float inBottomX, float inBottomY)
{
    // In case of invalid coordinates, set all values to -1.
    if (inTopX > inBottomX || inTopY > inBottomY)
    {
        topX = -1;
        topY = -1;
        bottomX = -1;
        bottomY = -1;
    }
    else
    {
        topX = inTopX;
        topY = inTopY;
        bottomX = inBottomX;
        bottomY = inBottomY;
    }
}
void RectangleShape::cycleRectangle(float rValue, float gValue, float bValue,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    static D2D1_RECT_F thisRect;
    ID2D1SolidColorBrush* pBrush = nullptr;

    // Early exit if invalid coordinates
    if (topX == -1)
    {
        return;
    }

    hr = S_OK;
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float adjustedTopX = size.width * topX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedBottomY = size.height * bottomY;
    thisRect = D2D1::RectF(adjustedTopX, adjustedTopY,
        adjustedBottomX, adjustedBottomY);
    const D2D1_COLOR_F color = D2D1::ColorF(rValue, gValue, bValue);
    hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);


    if (SUCCEEDED(hr))
    {
        pRenderTarget->FillRectangle(thisRect, pBrush);
    }
    (*&pBrush)->Release();
}

//////////////////////////////////////////////////////////////////////////////
//                             Child Classes
//////////////////////////////////////////////////////////////////////////////

//
RectangularBorder::RectangularBorder()
{
    // Only exists to be replaced by calling below constructor.
}
RectangularBorder::RectangularBorder(HWND inHandle)
{
    hWnd = inHandle;
}
void RectangularBorder::cycleRectangle(float rValue, float gValue, float bValue,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    static D2D1_RECT_F thisRect;
    ID2D1SolidColorBrush* pBrush = nullptr;

    hr = S_OK;
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float adjustedTopX = size.width * topX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedBottomY = size.height * bottomY;
    thisRect = D2D1::RectF(adjustedTopX, adjustedTopY,
        adjustedBottomX, adjustedBottomY);
    const D2D1_COLOR_F color = D2D1::ColorF(rValue, gValue, bValue);
    hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);


    if (SUCCEEDED(hr))
    {
        pRenderTarget->DrawRectangle(thisRect, pBrush);
    }
    (*&pBrush)->Release();
}

// ToggleButton
ToggleButton::ToggleButton()
{
    // Only exists to be replaced by calling below constructor.
}
ToggleButton::ToggleButton(HWND inHandle)
{
    hWnd = inHandle;
}
bool ToggleButton::cycleButton(int inX, int inY, bool inClicked,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float adjustedRadius = min(size.width, size.height) * radius / 2;
    float useRadius;

    // Show either onClick state, or toggle state if clicked

    if (boundsCheck(inX, inY, size))
    {
        // Show onClick
        if (inClicked == true)
        {
            this->cycleCircle(0.5, 0.5, 0.8, hr, pRenderTarget);
            toggled = true;
        }
        // Show onHover
        else
        {
            float tempRad = radius;
            this->cycleCircle(0.7, 0.7, 0.7, hr, pRenderTarget);
            radius = radius * 0.67;
            this->cycleCircle(0.75, 0.75, 0.75, hr, pRenderTarget);
            radius = tempRad;
        }
    }
    else
    {
        // Show toggled
        if (toggled == true)
        {
            float tempRad = radius;
            this->cycleCircle(0.8, 0.8, 0.8, hr, pRenderTarget);
            radius = radius * 0.67;
            this->cycleCircle(0.4, 0.4, 0.4, hr, pRenderTarget);
            radius = tempRad;
        }
        // Show default
        else
        {
            float tempRad = radius;
            this->cycleCircle(0.8, 0.8, 0.8, hr, pRenderTarget);
            radius = radius * 0.67;
            this->cycleCircle(0.75, 0.75, 0.75, hr, pRenderTarget);
            radius = tempRad;
        }
    }

    return toggled;
}
bool ToggleButton::boundsCheck(const int& inX, const int& inY,
    D2D1_SIZE_F size)
{
    const float adjustedRadius = min(size.width, size.height) * radius / 2;
    float useRadius;
    // Check cursor is within upper and lower bounds for X and Y
    if (((size.width * centerX - adjustedRadius) < inX) &&
        (((size.width * centerX) + adjustedRadius) > inX) &&
        (((size.height * centerY) - adjustedRadius) < inY) &&
        (((size.height * centerY) + adjustedRadius) > inY))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// TextBox
TextBox::TextBox()
{
    // Only exists to be replaced by calling below constructor.
}
TextBox::TextBox(HWND inHandle)
{
    hWnd = inHandle;
}
void TextBox::setText(std::string inText)
{
    text = inText;
}
void TextBox::cycleText(float rValue, float gValue, float bValue,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // One time only initializations
    static ID2D1Factory* pD2DFactory = nullptr;
    static IDWriteFactory* pDWriteFactory = nullptr;
    static const WCHAR msc_fontName[] = L"Verdana";
    ID2D1SolidColorBrush* textBoxBrush = nullptr;
    static IDWriteTextFormat* textBoxTextFormat = nullptr;

    // Early exit if invalid coordinates
    if (topX == -1)
    {
        return;
    }

    // Measure various sizes.
    // Sort out what rectangular coordinates text needs
    D2D1_RECT_F thisRect;
    RECT rc;
    GetClientRect(hWnd, &rc);
    D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();
    hr = S_OK;
    const int localWidth = rc.right - rc.left;
    const int localHeight = rc.bottom - rc.top;
    D2D1_SIZE_U intSize = D2D1::SizeU(localWidth, localHeight);
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float adjustedTopX = size.width * topX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedBottomY = size.height * bottomY;
    thisRect = D2D1::RectF(adjustedTopX, adjustedTopY,
        adjustedBottomX, adjustedBottomY);

    // Convert to WCHAR and attempt to find a good font size
    int arraySize = text.size();
    // Cast text to wide string so it can be cast to a WCHAR
    std::wstring wideText = std::wstring(text.begin(), text.end());
    const WCHAR* convertedText = wideText.c_str();
    // Each font point is about 1/72 inch by definition
    // A digital inch is 96 pixels long by definition
    const float fontPoint = (1.0 / 72.0);
    const float digitalSquareInch = 96.0 * 96;
    // We want a logarithmic relationship since text shouldn't explode
    // in font size, but needs a minimum to be readable. Font
    // size is also already a bit exponential anyway.
    // Spaces also cause a fair bit of trouble by wasting space
    // working around them neatly for wrapping, so we throw in
    // the 0.75 * just to add some wiggle room. Also, ensure it
    // is at least a minimum so it doesn't break things / get
    // broken by dividing by zero.
    FLOAT msc_fontSize;
    if (minScaling == true)
    {
        msc_fontSize = max((0.75 * log2(digitalSquareInch *
            (adjustedBottomY - adjustedTopY) * 
            (adjustedBottomX - adjustedTopX) *
                fontPoint / (arraySize))), 12);
    }
    else
    {
        msc_fontSize = max((0.6 * log2(digitalSquareInch *
            (adjustedBottomY - adjustedTopY) *
            (adjustedBottomX - adjustedTopX) *
            fontPoint / (arraySize))), 1);
    }
    
    

    // Create factories on first object function call.
    // For details see:
    //      https://docs.microsoft.com/en-us/windows/win32/direct2d/
    //      how-to--draw-text
    if (pD2DFactory == nullptr)
    {
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &pD2DFactory);
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&pDWriteFactory));

        // Create a DirectWrite text format object.
        pDWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"en-us", //locale
            &textBoxTextFormat);


    }
    // Create a DirectWrite text format object.
    pDWriteFactory->CreateTextFormat(
        msc_fontName,
        NULL,
        DWRITE_FONT_WEIGHT_REGULAR,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        msc_fontSize,
        L"en-us", //locale
        &textBoxTextFormat);

    textBoxTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textBoxTextFormat->SetParagraphAlignment(
        DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


    if (!pRenderTarget)
    {
        // Create a Direct2D render target.
        hr = pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                hWnd,
                intSize
            ),
            &pRenderTarget);
    }

    // Actually draw the text
    if (pRenderTarget != nullptr)
    {
        // Create the colored brush.
        const D2D1_COLOR_F thisColor = D2D1::ColorF(rValue, gValue, bValue);
        hr = pRenderTarget->CreateSolidColorBrush(
            thisColor, &textBoxBrush);

        pRenderTarget->DrawText(
            convertedText,
            arraySize,
            textBoxTextFormat,
            thisRect,
            textBoxBrush);
    }

    // Free device resources
    if (FAILED(hr))
    {
        if (pRenderTarget)
        {
            (pRenderTarget)->Release();
            pRenderTarget = NULL;
        }
    }
    (*&textBoxBrush)->Release();
    (*&textBoxTextFormat)->Release();
}

// TextEnterInt
TextEnterInt::TextEnterInt()
{
    // Only exists to be replaced by calling below constructor.
    text = "0";
    buffer = "0";
}
TextEnterInt::TextEnterInt(HWND inHandle)
{
    text = "0";
    buffer = "0";
    hWnd = inHandle;
}
int TextEnterInt::cycleBox(char inputKey, int inX, int inY, bool inClicked,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Convert to string to use later
    std::string stringKey = "";
    stringKey += inputKey;

    D2D1_SIZE_F size = pRenderTarget->GetSize();

    // Show either onClick state, or toggle state if clicked
    if (boundsCheck(inX, inY, size))
    {

        // Show onClick, blue, and activate
        if (inClicked == true)
        {
            processKey(stringKey);
            cycleRectangle(0.5, 0.5, 0.8, hr, pRenderTarget);
            active = true;
        }
        // Show onHover
        else
        {
            if (active == true)
            {
                processKey(stringKey);
            }
            cycleRectangle(0.75, 0.75, 0.78, hr, pRenderTarget);
        }
    }
    else
    {
        cycleRectangle(0.75, 0.75, 0.78, hr, pRenderTarget);
        if (active == true)
        {
            // Deactivate if the user clicks off.
            if (inClicked == true)
            {
                buffer = text;
                active = false;
            }
            else
            {
                processKey(stringKey);
            }
        }
    }

    // Show the text itself
    this->cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    return stoi(buffer);
}
bool TextEnterInt::boundsCheck(const int& inX, const int& inY,
    D2D1_SIZE_F size)
{
    const float adjustedTopX = size.width * topX;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomY = size.height * bottomY;

    // Check cursor is within upper and lower bounds for button area
    if ((adjustedTopX < inX) &&
        (adjustedBottomX > inX) &&
        (adjustedTopY < inY) &&
        (adjustedBottomY > inY))
    {
        return true;
    }
    else
    {
        return false;
    }
}
void TextEnterInt::processKey(std::string inputKey)
{

    // Early exit for no key
    if (inputKey != "")
    {
        int tempText = stoi(text);

        //Cut off right most digit with integer division
        if (!std::isdigit((char)inputKey[0]))
        {
            if (inputKey == "b")
            {
                tempText = tempText / 10;
                text = std::to_string(tempText);
            }
            else if (inputKey == "e")
            {
                active = false;
                buffer = text;
            }
        }
        else
        {
            tempText = (tempText * 10) + stoi(inputKey);
            text = std::to_string(tempText);
        }
    }
}
void TextEnterInt::setText(std::string inText)
{
    buffer = inText;
    text = inText;
}

// Dropdown
Dropdown::Dropdown()
{
    // Only exists to be replaced by calling below constructor.
}
Dropdown::Dropdown(HWND inHandle)
{
    hWnd = inHandle;
}
void Dropdown::appendItem(std::string newItem)
{
    prompts.push_back(newItem);

    // Ensure some item will display if none were before
    if (prompts.size() == 1)
    {
        text = prompts[0];
    }
}
int Dropdown::boundsCheck(const int& inX, const int& inY,
    D2D1_SIZE_F size)
{
    const float adjustedTopX = size.width * topX;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomY = size.height * bottomY;
    const float offset = adjustedBottomY - adjustedTopY;
    const int listSize = prompts.size();

    // Check cursor is within upper and lower bounds for button area
    // If the dropdown is closed, it will only be one long. Otherwise,
    // they all pop down and it needs to check for each. It
    // will return the index of whichever item the cursor is over,
    // or just the focus if it is over the closed item.
    if (active == true)
    {
        // Check if it is within the area of the whole dropdown box
        if ((adjustedTopX < inX) &&
            (adjustedBottomX > inX) &&
            (adjustedTopY < inY) &&
            (adjustedBottomY + (offset * (listSize - 1)) > inY))
        {
            // Calculate and return which one if it is
            return int((inY - adjustedTopY) / offset);
        }
    }
    else
    {
        if ((adjustedTopX < inX) &&
            (adjustedBottomX > inX) &&
            (adjustedTopY < inY) &&
            (adjustedBottomY > inY))
        {
            return focus;
        }
    }

    // Reached the end of the list without any intersections
    // valid mouse in a button range, return -1 signalling
    // as such.
    return -1;
}
int Dropdown::cycleBox(int inX, int inY, bool inClicked,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Copy main coordinates
    const float tempTopY = topY;
    const float tempBottomY = bottomY;
    const float offset = bottomY - topY;

    // Return -1 if nothing is clicked, otherwise the index of what is.
    int selected = -1;

    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const int listSize = prompts.size();

    // Early exit if no items to draw
    if (listSize < 1)
    {
        return -1;
    }

    int tempSelection = boundsCheck(inX, inY, size);

    // Show either the collapsed menu, or the whole list
    if (active == false)
    {
        // Show onHover or onClick state if within, otherwise normal
        if (tempSelection != -1)
        {
            // If its clicked, activate and change state
            if (inClicked == true)
            {
                cycleRectangle(0.5, 0.5, 0.8, hr, pRenderTarget);
                text = prompts[focus];
                TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
                active = true;
                collapseWait = true;
            }
            else
            {
                RectangleShape::cycleRectangle(0.70, 0.70, 0.70, hr, pRenderTarget);
                text = prompts[focus];
                TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            }
        }
        else
        {
            RectangleShape::cycleRectangle(0.75, 0.75, 0.75, hr, pRenderTarget);
            text = prompts[focus];
            TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            collapseWait = false;
        }
    }
    else
    {
        for (int index = 0; index < listSize; index++)
        {
            // 
            topY = tempTopY + (offset * index);
            bottomY = tempBottomY + (offset * index);

            // Show onHover or onClick state if within, otherwise normal
            if (tempSelection == index)
            {
                // If its clicked, activate and change state
                if (inClicked == true)
                {
                    cycleRectangle(0.5, 0.5, 0.8, hr, pRenderTarget);
                    text = prompts[index];
                    TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
                    active = false;
                    selected = index;
                    focus = index;
                }
                else
                {
                    RectangleShape::cycleRectangle(0.70, 0.70, 0.70, hr, pRenderTarget);
                    text = prompts[index];
                    TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
                }
            }
            else
            {
                RectangleShape::cycleRectangle(0.75, 0.75, 0.75, hr, pRenderTarget);
                text = prompts[index];
                TextBox::cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            }
        }
    }

    topY = tempTopY;
    bottomY = tempBottomY;

    // Collapse menu if user clicks outside of it.
    if (selected == -1 && inClicked == true && collapseWait != true)
    {
        active = false;
    }


    return selected;
}

// Button
Button::Button()
{
    // Only exists to be replaced by calling below constructor.
}
Button::Button(HWND inHandle)
{
    hWnd = inHandle;
}
int Button::boundsCheck(const int& inX, const int& inY,
    D2D1_SIZE_F size)
{
    const float adjustedTopX = size.width * topX;
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedTopY = size.height * topY;
    const float adjustedBottomY = size.height * bottomY;

    // Check cursor is within upper and lower bounds for button area
    if ((adjustedTopX < inX) &&
        (adjustedBottomX > inX) &&
        (adjustedTopY < inY) &&
        (adjustedBottomY > inY))
    {
        // Calculate and return which one if it is
        return true;
    }
    else
    {
        return false;
    }
}
bool Button::cycleButton(int inX, int inY, bool inClicked,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Calculate window size
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    int tempSelection = boundsCheck(inX, inY, size);

    // Check if mouse is within bounds of button
    if (boundsCheck(inX, inY, size))
    {
        // Show onClick
        if (inClicked == true)
        {
            // Show a colored rectangle overlayed by the text
            this->cycleRectangle(0.5, 0.5, 0.8, hr, pRenderTarget);
            this->cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            return true;
        }
        // Show onHover
        else
        {
            this->cycleRectangle(0.7, 0.7, 0.7, hr, pRenderTarget);
            this->cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            return false;
        }
    }
    else
    {
        this->cycleRectangle(0.75, 0.75, 0.75, hr, pRenderTarget);
        this->cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
        return false;
    }
}

// ImageAsset
void ImageAsset::setFile(std::string inFileName) 
{ 
    imageSet = true; 
    fileName = inFileName; 
}

// Credit: Partially made from code obtained here.
// See:
// https://docs.microsoft.com/en-us/windows/win32/wic/
// -wic-bitmapsources-howto-drawusingd2d
void ImageAsset::cycleImage(HRESULT& hr, 
    ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Declare/Initialize statics
    static IWICBitmapDecoder* pDecoder = nullptr;
    static IWICImagingFactory* m_pIWICFactory;
    static ID2D1Bitmap* m_pD2DBitmap = nullptr;
    static IWICFormatConverter* m_pConvertedSourceBitmap = nullptr;
    static IWICBitmapFrameDecode* pFrame = NULL;
    static D2D1_RECT_F thisRect;
    static RECT rc;

    // Early exit for invalid coordinates
    if (topX == -1)
    {
        return;
    }


    // Convert filename to WCHAR *
    std::wstring wideName = std::wstring(fileName.begin(), fileName.end());
    const WCHAR* convertedName = wideName.c_str();

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pIWICFactory)
    );
    hr = m_pIWICFactory->CreateDecoderFromFilename(
        convertedName,                      // Image to be decoded
        NULL,                            // Do not prefer a particular vendor
        GENERIC_READ,                    // Desired read access to the file
        WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
        &pDecoder                        // Pointer to the decoder
    );


    if (SUCCEEDED(hr))
    {
        hr = pDecoder->GetFrame(0, &pFrame);
    }

    // Format convert the frame to 32bppPBGRA
    if (SUCCEEDED(hr))
    {
        if (m_pConvertedSourceBitmap)
        {
            (m_pConvertedSourceBitmap)->Release();
            m_pConvertedSourceBitmap = NULL;
        }
        hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pConvertedSourceBitmap->Initialize(
            pFrame,                          // Input bitmap to convert
            GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
            WICBitmapDitherTypeNone,         // Specified dither pattern
            NULL,                            // Specify a particular palette 
            0.f,                             // Alpha threshold
            WICBitmapPaletteTypeCustom       // Palette translation type
        );
    }


    GetClientRect(hWnd, &rc);
    // Retrieve the size of the render target.
    D2D1_SIZE_F renderTargetSize = pRenderTarget->GetSize();
    hr = S_OK;

    const int localWidth = rc.right - rc.left;
    const int localHeight = rc.bottom - rc.top;
    D2D1_SIZE_F size = pRenderTarget->GetSize();
    const float adjustedTopX = size.width * topX; // Border
    const float adjustedTopY = size.height * topY; // Border
    const float adjustedBottomX = size.width * bottomX;
    const float adjustedBottomY = size.height * bottomY;
    thisRect = D2D1::RectF(adjustedTopX, adjustedTopY,
        adjustedBottomX, adjustedBottomY);

    // D2DBitmap may have been released due to device loss. 
    // If so, re-create it from the source bitmap
    pRenderTarget->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap,
        NULL, &m_pD2DBitmap);

    // Draws an image and scales it to the current window size
    if (m_pD2DBitmap)
    {
        pRenderTarget->DrawBitmap(m_pD2DBitmap, thisRect);
    }
}