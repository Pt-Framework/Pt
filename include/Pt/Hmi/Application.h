#ifndef Pt_Hmi_Application_h
#define Pt_Hmi_Application_h

#include <Pt/Hmi/Api.h>
#include <Pt/System/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>
#include <Pt/Singleton.h>

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
	int fromUnit(double value);

	double unitSizeInch() const;
	double unitSizeMm() const;

	void setResolution(double dpi);
	double resolutionDPI() const;

	ApplicationImpl* impl();

private:     
     ApplicationImpl* _impl; 
};

} // namespace gui

} // namespace Pt

#endif
