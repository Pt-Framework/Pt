#ifndef Pt_Hmi_Base_Property_h
#define Pt_Hmi_Base_Property_h


#include <Pt/Signal.h>
#include <string>

namespace Pt {
namespace Hmi {

#define DefineProperty(prop,value) prop(#prop,me(),value)
#define DefinePropertyDefault(prop) prop(#prop,me())

class Model;
    
class PropertyBase
{
public:
	PropertyBase(const char* name, Model* parent);
    
    inline const Model* parent() const
    {
        return _parent;
    }
    
    inline Model* parent()
    {
        return _parent;
    }
    
    const std::string& name() const
    {
        return _name;
    }
protected:
    void changed();
    
protected:
	Model* _parent;
    std::string _name;
};

template<typename T>
class  Property  : public PropertyBase
{
public:
    Property(const char* name, Model* parent)
	: PropertyBase(name, parent)
	{
	}

	Property(const char* name, Model* parent, const T& value)
	: PropertyBase(name, parent)
	, _value(value)
	{
	}

    virtual ~Property()
	{
	}

	inline T& get()
	{
		return _value;
	}

	inline const T& get() const
	{
		return _value;
	}
	
	inline void set(const T& value) 
	{
		_value = value;
	}

	T& operator=(const T& value)
	{
		_value = value;		
		PropertyChanged.send(_parent, *this);
         changed();
		return _value;
	}

public:	
	Pt::Signal<const void*, const PropertyBase&> PropertyChanged;

private:
	T _value;
};

}}
#endif

