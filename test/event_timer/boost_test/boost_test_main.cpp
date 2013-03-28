#include <boost/test/included/unit_test.hpp>
#include <iostream>

// Read this:
// http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide/test-organization/manual-nullary-test-case.html

using namespace boost::unit_test;
using namespace std;


/** DECLARATION **/
void test_case_sleep_loop(void);
void test_case_boosted_thread_loop(void);
void test_case_stream_logging_parser(void);

void test_case_io_service(void);

/** REGISTRATION **/
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    if(system("rm -rf *logfile_sleep_tester.log"))
    {;}

    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_sleep_loop ));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_boosted_thread_loop ));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_io_service ));
    cout << "=============================================" << endl
         << "logs: 0 = std. prio sleep"                     << endl
         << "logs: 1 = max. prio sleep"                     << endl
         << "logs: 2 = std. prio asio.io"                   << endl
         << "=============================================" << endl;
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_stream_logging_parser ));


    return 0;
}


