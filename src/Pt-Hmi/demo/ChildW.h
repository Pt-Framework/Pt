#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include "Dialog1.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/DockingLayout.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/ScrollView.h>
#include <Pt/Hmi/Application.h>

namespace Pt {

namespace Hmi {

namespace Demo {

class ChildW : public Hmi::Window
{
    public:
        ChildW(const std::string& title);
        
        ~ChildW();

    private:
        void onShowDialog(Button&);

        void onCheckBox(Button&);

        void onTextEntered(const Pt::String& text);
        
        void onCloseApp(Button&);
        
        void checkInput(const Pt::String& s);

    private:
        FlowLayout _mainLayout;
        MenuBar _menuBar;

        Menu     _fileMenu;
        MenuItem _item1;
        MenuItem _item2;
        MenuItem _item3;

        Menu     _editMenu;
        MenuItem _edit1;
        MenuItem _edit2;
        MenuItem _edit3;

        ScrollBar _vscroll;

        Window _childWindow2;
        DockingLayout  _childView;
        PushButton _closeButton;
        PushButton _toggleButton;
        PushButton _dialogButton;
        CheckBox   _checkBox;
        LineEdit   _lineEdit;
        LineEdit   _lineEdit2;
        Label  _textLabel;
        DockingLayout _buttonBar;
};

} // namespace

} // namespace

} // namespace

#endif