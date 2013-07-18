#ifndef Pt_Hmi_Controller_H
#define Pt_Hmi_Controller_H

#include <Pt/Connectable.h>
#include <Pt/Hmi/InputDevice.h>
#include <Pt/Hmi/OutputDevice.h>
#include <Pt/Hmi/Model.h>
#include <Pt/Hmi/Renderer.h>
#include <Pt/Hmi/Api.h>

namespace Pt{
namespace Hmi{

class PT_HMI_API Controller  : public Pt::Connectable
{
protected:
	Controller(Controller* parent = 0)
	: _model(0)
	, _renderer(0)
	, _parent(0)
	{ }

public:
	virtual ~Controller()
	{ }

	inline void setModel(Model* model)
	{
		_model = model;
		_model->Changed += Pt::slot(*this, &Controller::modelChanged);
	}

	inline Model* model()
	{
		return _model;
	}

	
	inline void setRenderer(Renderer* r)
	{
		_renderer = r;
	}

	inline Renderer* renderer()
	{
		return _renderer;
	}

	inline void addInputDevice(InputDevice* device)
	{
		_inputDevices.push_back(device);
	}

	inline void removeInputDevice(InputDevice* device)
	{
		for(size_t i = 0; i < _inputDevices.size(); ++i)
		{
			if(_inputDevices[i] == device)
			{
				_inputDevices.erase(_inputDevices.begin() + i);
				return;
			}
		}
	}

	inline void addOutputDevice(OutputDevice* device)
	{
		_outputDevices.push_back(device);
	}

	inline void removeOutputDevice(OutputDevice* device)
	{
		for(size_t i = 0; i < _outputDevices.size(); ++i)
		{
			if(_outputDevices[i] == device)
			{
				_outputDevices.erase(_outputDevices.begin() + i);
				return;
			}
		}
	}

	inline const std::vector<InputDevice*>& inputDevices() const
	{
		return _inputDevices;
	}


	inline const std::vector<OutputDevice*>& outputDevices() const
	{
		return _outputDevices;
	}

	inline const std::vector<Controller*>& children() const
	{
		return _children;
	}

	inline void addChildren(Controller* base)
	{
		_children.push_back(base);
	}

	inline void removeChildren(Controller* base)
	{
		
	}	
	const Controller* parent() const
	{
		return _parent;
	}

	Controller* parent() 
	{
		return _parent;
	}

protected:
	void modelChanged()
	{
		onModelChanged();
	}

	virtual void onModelChanged() = 0;

private:
	std::vector<InputDevice*>	_inputDevices;
	std::vector<OutputDevice*>	_outputDevices;
	Model*						_model;
	Renderer*					_renderer;
	std::vector<Controller*>    _children;
	Controller*					_parent;
};

}}
#endif
