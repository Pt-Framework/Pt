#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/System/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Cursor.h>
#include <Pt/Hmi/Event.h>
#include <Pt/Hmi/Screen.h>

namespace Pt {
namespace Hmi {

class ApplicationImpl;

class PT_HMI_API Application : public Pt::System::Application
{
public:
  Application(int argc = 0, char** argv = 0);

  virtual ~Application();

	static Application& instance();

	Pt::Gfx::PointF toUnit(const Pt::Gfx::Point& value);
	Pt::Gfx::SizeF toUnit(const Pt::Gfx::Size& value);
	double toUnit(int value);

	Pt::Gfx::Point fromUnit(const Pt::Gfx::PointF& value);
	Pt::Gfx::Size fromUnit(const Pt::Gfx::SizeF& value);
	Pt::Gfx::Rect fromUnit(const Pt::Gfx::RectF& value);
	int fromUnit(double value);

	double unitSizeInch() const;
	double unitSizeMm() const;

	void setResolution(double dpi);
	double resolutionDPI() const;

	ApplicationImpl* impl();
	
	void nextEvent();

	void setCursor(const Cursor* cursor = 0);

	const Screen& mainScreen() const
	{
		return _mainScreen;
	}

	Screen& mainScreen()
	{
		return _mainScreen;
	}

private:     
  ApplicationImpl* _impl; 
  Screen _mainScreen;
};

}}

#endif
