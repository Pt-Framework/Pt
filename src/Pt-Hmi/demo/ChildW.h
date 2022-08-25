#ifndef Pt_Hmi_Demo_ChildW_h
#define Pt_Hmi_Demo_ChildW_h

#include "Dialog1.h"
#include <Pt/Hmi/Window.h>
#include <Pt/Hmi/Shell.h>
#include <Pt/Hmi/PushButton.h>
#include <Pt/Hmi/CheckBox.h>
#include <Pt/Hmi/ComboBox.h>
#include <Pt/Hmi/Panel.h>
#include <Pt/Hmi/Label.h>
#include <Pt/Hmi/LineEdit.h>
#include <Pt/Hmi/DockingLayout.h>
#include <Pt/Hmi/FlowLayout.h>
#include <Pt/Hmi/MenuBar.h>
#include <Pt/Hmi/Menu.h>
#include <Pt/Hmi/ScrollView.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/ProgressBar.h>
#include <Pt/Hmi/Slider.h>
#include <Pt/Hmi/SpinBox.h>

namespace Pt {

namespace Hmi {

namespace Demo {

class ChildW : public Hmi::Window
{
    public:
        ChildW(const std::string& title);
        
        ~ChildW();

        Shell& shell()
        {
            return _shell;
        }

        Pt::Signal<>& closeRequested()
        { return _closeButton.clicked(); }

    private:
        void onShowDialog();

        void onCheckBox();

        void onComboSelected(Pt::Hmi::ListBoxItem& item);

        void onTextEntered(const Pt::String& text);

        void checkInput(const Pt::String& s);

        void checkSpinBox(int value);

    private:
        FlowLayout _mainLayout;
        MenuBar _menuBar;

        Shell _shell;

        Menu     _fileMenu;
        MenuItem _item1;
        MenuItem _item2;
        MenuItem _item3;

        Menu     _editMenu;
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