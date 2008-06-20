/***************************************************************************
 *   Copyright (C) 2008 Peter Barth                                        *
 *   Copyright (C) 2008 PTV AG                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SYMBAPPUI_H_
#define SYMBAPPUI_H_

#include <Pt/Gui/KeyEvent.h>

#include <aknappui.h>

// forward declarations
namespace Pt { 
namespace Gui { 

    class Widget; 
    class ApplicationImpl; 
    
} 
}

/**
 * @brief Symbian view class used by Eikon framework.
 * 
 * This class is the last class in the MVC based application being instantiated 
 * by the Eikon framework.
 * 
 * In this class all the Pt-Widgets and Pixmaps which have been created
 * prior to Application::run() will be finally constructed.
 * It will also start the event loop which is held in the application implementation.
 * 
 * During runtime this class will handle key events and dispatch them to all
 * top level widgets.
 * 
 * <b>IMPORTANT NOTE:</b>
 * ALL Symbian classes have to reside in the global namespace
 * otherwise results are undefined
 */
class SymbAppUi : public CAknAppUi
{
public:     
    /**
     * @brief second phase constructor.
     */ 
    void ConstructL();    
    
    /**
     * @brief Destructor.
     */
    ~SymbAppUi();
    
    /**
     * @brief Provide access to the UI default font.
     */
    const CFont& Font() const { return *_font; }

    /**
     * @brief Handle exit.
     */
    void HandleExit();    
    
    /**
     * @brief This will synchronize the current widgets with their frontend.
     * It will force a resize and move event being sent to all widgets.
     */
    void SynchronizeWidgets();
    
private:
    /**
     * @brief From CAknAppUi: Does nothing.
     */
    void DynInitMenuPaneL(TInt, CEikMenuPane*);

    /**
     * @brief From CAknAppUi: Does nothing.
     */
    void HandleCommandL(TInt);
    
    /**
     * @brief From CAknAppUi: Handle key events. Is called by Eikon framework.
     */
    virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * @brief Handle key down events.
     */
    bool HandleEventKeyDown(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * @brief Handle key events.
     */
    bool HandleEventKey(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * @brief Handle key up events.
     */
    bool HandleEventKeyUp(const TKeyEvent& aKeyEvent, TEventCode aType);
    
    /**
     * @brief This will dispatch a fake pointer event to all top level windows.
     */
    void DispatchFakePointerEvent(const TPointerEvent& event);
    
    /**
     * @brief This will simulate fake pointer events from various key codes.
     */
    bool HandleFakePointer(const TKeyEvent& aKeyEvent, TEventCode aType, int offset = 5);

    /**
     * @brief This will force a redraw of all top level windows.
     */
    void RedrawWindows();
    
    /**
     * @brief This will construct a KeyEvent and dispatch it to all top level windows.
     * @param type Pt keyboard event type (Pressed/Released)
     * @param code Pt keyboard event code
     * @param chr Wide character code of event if readable character.
     */
    void DispatchKeyEvent(Pt::Gui::KeyEvent::Type type,
            Pt::Gui::KeyEvent::KeyCode code,
            wchar_t chr);
    
    /**
     * @brief Mark a key as pressed
     */
    void MarkKeyDown(TUint scanCode, TUint keyCode);

    /**
     * @brief Mark a key as not pressed.
     */
    void ResetKeyDown(TUint scanCode);
    
    /**
     * @brief Query state of key (pressed/unpressed).
     */
    bool IsKeyDown(TUint scanCode, TUint& keyCode);
    
    /**
     * @brief Mark a key as first pressed.
     * When key is held down key events are coming repeatedly, this will
     * mark the first event.
     */
    void MarkFirstKey(TUint scanCode, bool down = true);
    
    /**
     * @brief Query first pressed state of key.
     * @see MarkFirstKey
     */
    bool IsFirstKey(TUint scanCode);    
    
    // This is the document that has created us
    //class SymbDoc* _parentDoc;

    // default font we're providing to widgets and pixmaps
    CFont* _font;
    
    // little top level window to simulate mouse cursor
    // TODO: decide whether to leave that in
    class CCursorControl* _cursorControl;

    // greatest scancode
    enum { KMaxScancode = 256 };
    // Flag that indicates if this is the first key event being produced
    // (for each scancode)
    bool _firstKey[KMaxScancode];
    // This table holds the virtual key code for each key being pressed
    // (for each scancode)
    TUint _keyDown[KMaxScancode];
};

#endif /*SYMBAPPUI_H_*/
