#include <iostream>
#include <QtCore/QObject>

class MyObject : public QObject
{
  Q_OBJECT
  
  public:
      MyObject()
      {}

      void go();

  public slots:
      void done()
      {
          std::cout << "+++++++++ DONE ++++++++++" << std::endl;
      }

  signals:
      void exec();
};
