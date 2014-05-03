#ifndef Pt_Qt_Application_h
#define Pt_Qt_Application_h

#include <Pt/Qt/Api.h>
#include <Pt/System/Application.h>
#include <Pt/Singleton.h>
#include <QtWidgets/QApplication>

namespace Pt {
namespace Qt {

class ApplicationImpl;

class PT_QT_API Application : public Pt::System::Application, public QApplication
{
public:
    Application(int argc = 0, char** argv = 0);
    virtual ~Application();

	static Application& instance();
	ApplicationImpl* impl();

private:     
     ApplicationImpl* _impl; 
};

}}

#endif
