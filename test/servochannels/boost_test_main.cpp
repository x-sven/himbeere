#include <boost/test/included/unit_test.hpp>
#include <iostream>

// Read this:
// http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide/test-organization/manual-nullary-test-case.html

using namespace boost::unit_test;
using namespace std;


/** DECLARATION **/
void test_servochannels_constructor(void);
void test_messageparser(void);

/** REGISTRATION **/
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    if(system("rm -rf *logfile_sleep_tester.log"))
    {;}

    framework::master_test_suite().add( BOOST_TEST_CASE( &test_servochannels_constructor ));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_messageparser ));


    return 0;
}


