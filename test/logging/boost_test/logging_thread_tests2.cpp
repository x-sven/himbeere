#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;

namespace __stream__
{

    bool record_thread_running = true;
    bool datagen_thread_running = true;

    #if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
    #else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
    #endif

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;
    float f_DT_s = 0.;

    std::string string_buffer_1, string_buffer_2;
    std::string *active_buffer = &string_buffer_1;

    boost::condition_variable record_trigger;
    boost::mutex mutex;

    const int NUM_LINES = 2e+5;
    bool datagen_done = false;

    /* threat functions*/
    void datagen_thread_loop(void)
    {
        ostringstream testline;

        /* create testdata*/
        int jj=0;
        for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
        {
            if(++jj > 9) jj = 0;
            testline << jj;
        }
        testline << "\n";

        jj=0;
        while(record_thread_running && jj < NUM_LINES)
        {
    #if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_MONOTONIC, &ts_beg); // http://linux.die.net/man/3/clock_gettime
    #else
            start_time = boost::posix_time::microsec_clock::local_time();
    #endif

            // ********************************* //
            *active_buffer += testline.str().c_str();
            if(4*1024 < active_buffer->length()) //write 4 kbyte blocks
            {
                record_trigger.notify_all();
            }
            // ********************************* //

    #if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_MONOTONIC, &ts_end);
            v_fTime_s.push_back(ts_end.tv_sec);
            v_fTime_us.push_back(ts_end.tv_nsec/1e+3);
            v_fDifftime.push_back((ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9);
            f_DT_s = (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9;
    #else
            stop_time = boost::posix_time::microsec_clock::local_time();
            time_duration = (stop_time - start_time);
            v_fTime_s.push_back( (stop_time-boost::posix_time::from_time_t(0)).total_seconds() );
            v_fTime_us.push_back( (stop_time-boost::posix_time::from_time_t(0)).fractional_seconds() );
            v_fDifftime.push_back( time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
            f_DT_s = (time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
    #endif
    #if defined(DEBUG)
            if(0.5 < 1.e+3*f_DT_s) // log only values above 0.5 ms
            {
                cout << "Line " << jj << " of " << NUM_LINES << ":"
                     << "\t" << v_fTime_s.back() << "." << v_fTime_us.back() << "s: "
                     << "\tdT: " << fixed << 1.e+3*f_DT_s << " ms"
                     << endl;
            }
    #endif
            boost::this_thread::sleep(boost::posix_time::microseconds(4*1e+6*f_DT_s)); //about 50% CPU load
            jj++;
        }//while

        datagen_done = true;
    }

    void record_thread_loop(void)
    {
        int buffer_no = 0;
        std::ofstream logfile;

        /* open log file*/
        logfile.open ("stream_thread_testlog.log", ios::out);
        if (!logfile.is_open())
        {
            cout << "Could not open logfile!" << endl;
        }

        boost::unique_lock<boost::mutex> lock(mutex);

        while(datagen_thread_running)
        {
            record_trigger.wait(lock);
            if(buffer_no < 2)
            {
                active_buffer = &string_buffer_2;
                logfile << string_buffer_1;
                string_buffer_1.clear();
                buffer_no = 2;
            }
            else
            {
                active_buffer = &string_buffer_1;
                logfile << string_buffer_2;
                string_buffer_2.clear();
                buffer_no = 1;
            }
        }
        logfile.close();
    }
}; //namespace __stream__

/* test case*/
void test_case_logging_stream_with_thread()
{
    using namespace __stream__;

    float fl_difftime = 0.;

    cout << "Starting test using stream with thread." << endl;

    /* create threats*/
    boost::thread record_thread;
    boost::thread datagen_thread;

    record_thread = boost::thread( &record_thread_loop);
    datagen_thread = boost::thread( &datagen_thread_loop);

    struct sched_param thread_param;

    thread_param.sched_priority = 10;
    int retcode;
    if ((retcode = pthread_setschedparam(record_thread.native_handle(), SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }

    thread_param.sched_priority = 90;
    if ((retcode = pthread_setschedparam(datagen_thread.native_handle(), SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }

    //boost::this_thread::sleep(boost::posix_time::seconds(10));
    while(!datagen_done)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }

    record_thread_running = false;
    datagen_thread_running = false;
    record_trigger.notify_all();

    vector<float>::iterator  itr_max_dT =    max_element( v_fDifftime.begin(), v_fDifftime.end() );
    vector<float>::iterator  itr_min_dT =    min_element( v_fDifftime.begin(), v_fDifftime.end() );

    fl_difftime = accumulate(v_fDifftime.begin(), v_fDifftime.end(), 0.0);
    std::cout << NUM_LINES                      << " bytes in "
              << fl_difftime                    << " s, "
              <<  1.e+6*fl_difftime/NUM_LINES   << " us/line, "
              << NUM_LINES*64/fl_difftime/1024  << " kbyte/s, "
              << "min: " << *itr_min_dT*1e+3    << " ms, "
              << "max: " << *itr_max_dT*1e+3    << " ms "
              << std::endl;

    std::ofstream timelog;
    timelog.open ("stream_thread_timelog.log", ios::out);
    timelog << "time_s" << "\t"
            << "time_us" << "\t"
            << "runtime_ms"
            << "\n";
    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.1 ms
        {
            timelog << v_fTime_s.at(ii) << "\t"
                    << v_fTime_us.at(ii) << "\t"
                    << fixed << 1.e+3*v_fDifftime.at(ii)
                    << "\n";
        }
    }
    timelog.close();
}


