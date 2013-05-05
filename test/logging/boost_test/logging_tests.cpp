#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "Logging.h"

using namespace std;

#include <time.h>
void test_case_direct_logging()
{
    int NUM_LINES = 100;

    //timespec ts_beg, ts_end;
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;

    Logging testlog;

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    for(int kk = 0; kk < 3; kk++)
    {
        testlog.begin("logfile_testlog.log");

        NUM_LINES = 10*NUM_LINES;
        vector<long> v_fTime_s;
        vector<long> v_fTime_us;
        vector<float> v_fDifftime;

        for(jj=0; jj < NUM_LINES; jj++)
        {
            //clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
            start_time = boost::posix_time::microsec_clock::local_time();
            testlog.write(std::ostringstream().flush()
                          << testline.str().c_str()
                          << endl
                         );

            //clock_gettime(CLOCK_REALTIME, &ts_end);
            stop_time = boost::posix_time::microsec_clock::local_time();
            time_duration = (stop_time - start_time);

//            v_fTime_s.push_back(ts_end.tv_sec);
//            v_fTime_us.push_back(ts_end.tv_nsec/1e+3);
//            v_fDifftime.push_back((ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9);
            v_fTime_s.push_back( (stop_time-boost::posix_time::from_time_t(0)).total_seconds() );
            v_fTime_us.push_back( (stop_time-boost::posix_time::from_time_t(0)).fractional_seconds() );
            v_fDifftime.push_back( time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
        }

        vector<float>::iterator  itr_max_dT =    max_element( v_fDifftime.begin(), v_fDifftime.end() );
        vector<float>::iterator  itr_min_dT =    min_element( v_fDifftime.begin(), v_fDifftime.end() );

        float difftime = accumulate(v_fDifftime.begin(), v_fDifftime.end(), 0.0);
        std::cout << NUM_LINES                 << " bytes in "
                  << difftime                  << " s, "
                  <<  1.e+6*difftime/NUM_LINES << " us/line, "
                  << NUM_LINES*64/difftime/8   << " kbyte/s, "
                  << "min: " << *itr_min_dT*1e+3 << " ms, "
                  << "max: " << *itr_max_dT*1e+3 << " ms "
                  << std::endl;
        testlog.end();

        testlog.begin("timing_testlog.log");
        testlog.write(std::ostringstream().flush()
                 << "time_s" << "\t"
                 << "time_us" << "\t"
                 << "runtime_ms"
                 << endl
                );
        for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
        {
            testlog.write(std::ostringstream().flush()
                          << v_fTime_s.at(ii) << "\t"
                          << v_fTime_us.at(ii) << "\t"
                          << fixed << 1.e+3*v_fDifftime.at(ii)
                          << endl
                         );
        }
        testlog.end();
    }
}


//void test_case_threaded_logging()
//{
//    int NUM_LINES = 100;
//
//    timespec ts_beg, ts_end;
//
//    Logging testlog;
//
//    ostringstream testline;
//
//    int jj=0;
//    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
//    {
//        if(++jj > 9) jj = 0;
//        testline << jj;
//    }
//
//    for(int kk = 0; kk < 3; kk++)
//    {
//        testlog.begin("logfile_testlog.log");
//
//        NUM_LINES = 10*NUM_LINES;
//        vector<long> v_fTime_s;
//        vector<long> v_fTime_us;
//        vector<float> v_fDifftime;
//
//        for(jj=0; jj < NUM_LINES; jj++)
//        {
//            clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
//
//            testlog.write(std::ostringstream().flush()
//                          << testline.str().c_str()
//                          << endl
//                         );
//
//            clock_gettime(CLOCK_REALTIME, &ts_end);
//            v_fTime_s.push_back(ts_end.tv_sec);
//            v_fTime_us.push_back(ts_end.tv_nsec/1e+3);
//            v_fDifftime.push_back((ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9);
//        }
//
//        vector<float>::iterator  itr_max_dT =    max_element( v_fDifftime.begin(), v_fDifftime.end() );
//        vector<float>::iterator  itr_min_dT =    min_element( v_fDifftime.begin(), v_fDifftime.end() );
//
//        float difftime = accumulate(v_fDifftime.begin(), v_fDifftime.end(), 0.0);
//        std::cout << NUM_LINES                 << " bytes in "
//                  << difftime                  << " s, "
//                  <<  1.e+6*difftime/NUM_LINES << " us/line, "
//                  << NUM_LINES*64/difftime/8   << " kbyte/s, "
//                  << "min: " << *itr_min_dT*1e+3 << " ms, "
//                  << "max: " << *itr_max_dT*1e+3 << " ms "
//                  << std::endl;
//        testlog.end();
//
//        testlog.begin("timing_testlog.log");
//        testlog.write(std::ostringstream().flush()
//                 << "time_s" << "\t"
//                 << "time_us" << "\t"
//                 << "runtime_ms"
//                 << endl
//                );
//        for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
//        {
//            testlog.write(std::ostringstream().flush()
//                          << v_fTime_s.at(ii) << "\t"
//                          << v_fTime_us.at(ii) << "\t"
//                          << fixed << 1.e+3*v_fDifftime.at(ii)
//                          << endl
//                         );
//        }
//        testlog.end();
//    }
//}
