#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Button.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/DockingLayout.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/ScrollView.h>
#include "Dialog1.h"

namespace Pt {

namespace Hmi {

namespace Demo {

class ChildW : public Hmi::Window
{
    public:
        ChildW(const std::string& title);
        
        ~ChildW();

    private:
        void onShowDialog(Button& button);
        
        void onCloseApp(Button& button);

        void onVScroll(ScrollBar&, int pos)
        {
            std::clog << "vscroll: " << pos << std::endl;
        }

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
        Button _closeButton;
        Button _toggleButton;
        Button _dialogButton;
        Label  _textLabel;
        DockingLayout _buttonBar;
        ScrollView _scrollView;
};

} // namespace

} // namespace

} // namespace

#endif