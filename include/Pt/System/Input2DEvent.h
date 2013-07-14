#ifndef Pt_System_Input2DEvent_h
#define Pt_System_Input2DEvent_h

#include <Pt/Event.h>
#include <Pt/Allocator.h>
#include <cstddef>
#include <Pt/System/Api.h>

namespace Pt {
namespace System {

class PT_SYSTEM_API Input2DEvent : public Pt::Event
{
public:
	static const std::type_info& TYPE_INFO;

public:
	enum Button 
	{
        LeftButton = 1 << 0,   //! The left mouse button was pressed, released or double-clicked.
        MiddleButton = 1 <<1,  //! The middle mouse button was pressed, released or double-clicked.
        RightButton = 1 << 2,  //! The right mouse button was pressed, released or double-clicked.
        WheelUp = 1 << 3,      //! The mouse-wheel was moved up.
        WheelDown = 1 << 4     //! The mouse-wheel was moved down.
    };

    enum Action 
	{
		None,
        Press,        //! A mouse button was pressed or a mouse-wheel moved.
        Release,      //! A mouse button was released.
        DoubleClick   //! A mouse button was double-clicked.
    };

    enum Modifier 
	{
        LeftButtonDown   = 1 << 0,  //! The left mouse button was down
        RightButtonDown  = 1 << 1,  //! The right mouse button was down
        MiddleButtonDown = 1 << 2   //! The middle mouse button was down
    };

public:	
	Input2DEvent(int button, Action action, int modifier, double x, double y);
	Input2DEvent(const Input2DEvent& copy);
	virtual ~Input2DEvent();

	inline int button() const
	{
		return _button;
	}

	inline Action action() const
	{
		return _action;
	}

	inline int modifier() const
	{
		return _modifier;
	}

	inline double x() const
	{
		return _x;
	}

	inline double y() const
	{
		return _y;
	}

protected:
    virtual Pt::Event& onClone(Pt::Allocator& allocator) const;
    virtual void onDestroy(Pt::Allocator& allocator);
    virtual const std::type_info& onTypeInfo() const;

private:
	int _button;
	Action _action;
	int _modifier;
	double _x;
	double _y;
};

}}

#endif

