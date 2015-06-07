#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/System/Application.h>
#include <Pt/Ui/Point.h>
#include <Pt/Ui/Size.h>
#include <Pt/Ui/Rect.h>
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

	Ui::PointF toUnit(const Ui::Point& value);
	Ui::SizeF toUnit(const Ui::Size& value);
	double toUnit(int value);

	Ui::Point fromUnit(const Ui::PointF& value);
	Ui::Size fromUnit(const Ui::SizeF& value);
	Ui::Rect fromUnit(const Ui::RectF& value);
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
  std::string _cursorName;
};

}}

#endif
