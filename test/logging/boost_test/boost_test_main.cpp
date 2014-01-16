#include <boost/test/included/unit_test.hpp>
#include <iostream>

// Read this:
// http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide/test-organization/manual-nullary-test-case.html

using namespace boost::unit_test;
using namespace std;


/** DECLARATION **/
void test_case_logging_timing(void);
void test_case_mmap_logging(void);
void test_case_mmap2_logging(void);
void test_case_mmap_boost_logging(void);
void test_case_logging_ofstream(void);
void test_case_logging_fwrite(void);
void test_case_logging_fwrite_sync(void);
void test_case_logging_ofstream_nobuffer(void);
void test_case_logging_ofstream_write(void);
void test_case_logging_fwrite_direct(void);
void test_case_logging_mmap_sprintf(void);
void test_case_logging_mmap_strcat(void);
void test_case_logging_mmap_memcpy(void);
void test_case_logging_mmap_boost(void);
void test_case_logging_stream_with_thread(void);
void test_case_logging_fwrite_with_thread(void);

/** REGISTRATION **/
test_suite* init_unit_test_suite( int argc, char* argv[] )
{
    if(system("rm -rf *logfile_testlog.log"))
    {;}
    if(system("rm -rf *timing_testlog.log"))
    {;}

//    // examples: logging_tests.cpp
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_mmap_logging ));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_mmap2_logging ));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_mmap_boost_logging ));

    // timing measurement
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_ofstream));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_ofstream_nobuffer));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_ofstream_write));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_fwrite));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_fwrite_sync));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_fwrite_direct));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_mmap_sprintf));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_mmap_strcat)); //too slow in any cases
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_mmap_memcpy));
//    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_mmap_boost));

    // implementations with threads: logging_thread_tests.cpp
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_fwrite_with_thread));
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_stream_with_thread));

    // Current logging implementation
    framework::master_test_suite().add( BOOST_TEST_CASE( &test_case_logging_timing ));

    return 0;
}


// run with: sudo chrt -f 99 ./test_logging_*

