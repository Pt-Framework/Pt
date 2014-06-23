#ifndef Pt_Hmi_Model_Cursor_h
#define Pt_Hmi_Model_Cursor_h

#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

namespace Cursors
{
	enum Type
	{
		Default,
		Arrow,
		Cross,
		Hand,
		IBeam,
		SizeNS,
		SizeWE,
		UserDefined,
	};
}

class CursorImpl;
class WindowController;

class PT_HMI_API Cursor
{

public:
	Cursor();
	virtual ~Cursor();

	void setCursor(Cursors::Type c);
	Cursors::Type getCursor() const;

private:
	CursorImpl* _impl;
};

}}

#endif
