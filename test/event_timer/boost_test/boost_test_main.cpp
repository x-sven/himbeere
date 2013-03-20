#include <boost/test/included/unit_test.hpp>

// Read this:
// http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide/test-organization/manual-nullary-test-case.html

using namespace boost::unit_test;


/** DECLARATION **/
void test_case_sleep_loop(void);
void test_case_io_service(void);

/** REGISTRATION **/
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_sleep_loop ) );
    //framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_io_service ) );

    return 0;
}


