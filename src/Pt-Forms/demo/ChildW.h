#ifndef Pt_Forms_Demo_ChildW_h
#define Pt_Forms_Demo_ChildW_h

#include "Dialog1.h"
#include <Pt/Forms/Window.h>
#include <Pt/Forms/Workspace.h>
#include <Pt/Forms/PushButton.h>
#include <Pt/Forms/CheckBox.h>
#include <Pt/Forms/ComboBox.h>
#include <Pt/Forms/MenuItem.h>
#include <Pt/Forms/MenuSubItem.h>
#include <Pt/Forms/Panel.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/LineEdit.h>
#include <Pt/Forms/DockingLayout.h>
#include <Pt/Forms/FlowLayout.h>
#include <Pt/Forms/MenuBar.h>
#include <Pt/Forms/Menu.h>
#include <Pt/Forms/ScrollView.h>
#include <Pt/Forms/Application.h>
#include <Pt/Forms/ProgressBar.h>
#include <Pt/Forms/Slider.h>
#include <Pt/Forms/SpinBox.h>

namespace Pt {

namespace Forms {

namespace Demo {

class ChildW : public Forms::Window
{
    public:
        ChildW(const std::string& title);
        
        ~ChildW();

        DockingLayout& layout()
        {
            return _mainLayout;
        }

        Pt::Signal<>& closeRequested()
        { return _closeButton.clicked(); }

    private:
        void onShowDialog();

        void onCheckBox();

        void onComboSelected(Pt::Forms::ListBoxItem& item);

        void onTextEntered(const Pt::String& text);

        void checkInput(const Pt::String& s);

        void checkSpinBox(int value);

        void onMenuExit(MenuItemBase& item);

    private:
        DockingLayout _mainLayout;
        MenuBar _menuBar;

        Workspace _workspace;

        Menu     _fileMenu;
        MenuBarItem _fileMenuItem;
        MenuItem _item1;
        MenuItem _item2;
        MenuItem _item3;

        Menu     _editMenu;
        MenuBarItem _editMenuItem;
        MenuItem _edit1;
        MenuItem _edit2;
        MenuItem _edit3;

        std::vector<ListBoxItem*> _comboItems;

        Window         _childWindow2;
        DockingLayout  _childView;
        PushButton     _closeButton;
        PushButton     _toggleButton;
        PushButton     _dialogButton;
        CheckBox       _checkBox;
        LineEdit       _lineEdit;
        ComboBox       _comboBox;
        Label          _textLabel;
        DockingLayout  _buttonBar;
        ProgressBar    _progressBar;
        Slider         _slider;
        SpinBox        _spinBox;
        Label          _iconLabel;
};

} // namespace

} // namespace

} // namespace

#endif