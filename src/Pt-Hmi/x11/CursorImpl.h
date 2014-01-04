#ifndef Pt_Hmi_Model_CursorImpl_h
#define Pt_Hmi_Model_CursorImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

namespace Pt{
namespace Hmi{

class WindowConroller;

class CursorImpl
{

public:
	CursorImpl();
	virtual ~CursorImpl();

	void setCursor(Cursors::Type c, WindowController* parent);
	Cursors::Type getCursor() const;

private:
	Cursors::Type _type;
	::Cursor _cursorId;
};

}}

#endif
