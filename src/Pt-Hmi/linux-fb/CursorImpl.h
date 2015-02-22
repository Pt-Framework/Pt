#ifndef Pt_Hmi_Model_CursorImpl_h
#define Pt_Hmi_Model_CursorImpl_h

#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>

namespace Pt{
namespace Hmi{

class WindowConroller;

class CursorImpl
{

public:
	CursorImpl();
	virtual ~CursorImpl();

	void setCursor(Cursors::Type c);
	Cursors::Type getCursor() const;

private:
	Cursors::Type _type;
	::Cursor _cursorId;
};

}}

#endif
