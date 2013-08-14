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
		AppStarting,
		Arrow,
		ArrowCD,
		Cross,
		Hand,
		Help,		
		IBeam,
		No,
		Pen,
		ScrollAll,
		ScrollE,
		ScrollN,
		ScrollNE,
		ScrollNS,
		ScrollNW,
		ScrollS,
		ScrollSE,
		ScrollSW,
		ScrollW,
		ScrollWE,
		SizeAll,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE,
		UpArrow,
		Wait 
	};
}

class CursorImpl;

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
