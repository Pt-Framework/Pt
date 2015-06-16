#ifndef Pt_Hmi_Label_H
#define Pt_Hmi_Label_H

#include <Pt/Hmi/Panel.h>

namespace Pt{
namespace Hmi{


class PT_HMI_API Label  : public Panel
{
	public:
		Label();
		virtual ~Label();	

	public:
		ValueProperty<bool> AutoSize;	

	protected:
		virtual void onRender(PaintSurface& paintSurface);
    virtual void onAutoSizeChanged(const bool& a);
    virtual void onCaptionChanged(const std::string& cap);

  private:
    void recalcNewSize();
};

}}

#endif