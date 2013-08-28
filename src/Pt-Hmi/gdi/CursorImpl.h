#ifndef Pt_Hmi_Model_CursorImpl_h
#define Pt_Hmi_Model_CursorImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <Windows.h>

namespace Pt{
namespace Hmi{


class 	WindowController;

class CursorImpl
{

public:
	CursorImpl(WindowController* parent);
	virtual ~CursorImpl();

	void setCursor(Cursors::Type c);
	Cursors::Type getCursor() const;

private:
	Cursors::Type _type;
	HCURSOR _cursor;
	WindowController*  _parent;
};

}}

#endif
