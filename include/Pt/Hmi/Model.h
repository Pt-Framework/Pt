#ifndef Pt_Hmi_Model_Base_h
#define Pt_Hmi_Model_Base_h

#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Hmi/Api.h>
#include <Pt/Hmi/Property.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Model : public Pt::Connectable
{
public:
	virtual ~Model()
	{ }

public:
	Property<bool>  Enable;
	Property<bool>  Active;
	Property<void*> Tag;  

public:
	Pt::Signal<> Changed;

protected:
	Model()
	{ }

};

}}

#endif