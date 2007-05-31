#ifndef PT_UNIT_TEXTPROTOCOL_H
#define PT_UNIT_TEXTPROTOCOL_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/TestSuite.h>


namespace Pt
{

namespace Unit
{

/**
 * The ComponentTestProtocol is used for running component tests
 * defined in component test protocol files (ctp).
 */
class PT_UNIT_API TextProtocol : public Pt::Unit::TestProtocol
    {
    public:

        /**
         * Constructor.
         *
         * @param iniFileName initializes the component test protocol file name
         */
        TextProtocol(const std::string& iniFileName)
        : m_iniFileName(iniFileName)
        {
        }

        /**
         * Loads and parses the component test protocol file, then runs the tests on the specified suite.
         *
         * @param suite the test suite to run the tests on
         */
        virtual void run(Pt::Unit::TestSuite& suite);

    private:
        std::string m_iniFileName;
    };

}   // namespace Unit

}   // namespace Pt


#endif  // PT_UNIT_TEXTPROTOCOL_H
