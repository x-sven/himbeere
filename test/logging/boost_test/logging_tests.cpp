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
void test_case_logging_timing()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#define WITH_BOOST_TIME

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    Logging testlog, timelog;

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    cout << "Starting test using Logging class." << endl;

//    do
//    {
        struct sched_param thread_param;
        thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
        pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

        testlog.begin("logfile_testlog.log");

        vector<long> v_fTime_s;
        vector<long> v_fTime_us;
        vector<float> v_fDifftime;

        for(jj=0; jj < NUM_LINES; jj++)
        {
#if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
            start_time = boost::posix_time::microsec_clock::local_time();
#endif

            // ********************************* //
            testlog.write(testline.str().c_str());
            // ********************************* //

#if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_REALTIME, &ts_end);
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
            boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
        }

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
        testlog.end();

        timelog.begin("timing_testlog.log");

        //timelog.headline(std::ostringstream().flush()
        timelog.write(std::ostringstream().flush()
                      << "time_s" << "\t"
                      << "time_us" << "\t"
                      << "runtime_ms"
                      << endl
                     );
        for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
        {

            if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
            {
                timelog.write(std::ostringstream().flush()
                              << v_fTime_s.at(ii) << "\t"
                              << v_fTime_us.at(ii) << "\t"
                              << fixed << 1.e+3*v_fDifftime.at(ii)
                              << endl
                             );
            }
        }
        timelog.end();
//        NUM_LINES = 2*NUM_LINES;
//    }
//    while(3 > fl_difftime); //at least XX seconds run time

}


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


void test_case_mmap_logging()
{

    const char FILEPATH[] = "mmapped.bin";
    const int NUMINTS = 1000;
    const int FILESIZE = (NUMINTS * sizeof(int));

    int i;
    int fd;
    int result;
    int *map;  /* mmapped array of int's */

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    fd = open(FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, FILESIZE-1, SEEK_SET);
    if (result == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */
    result = write(fd, "", 1);
    if (result != 1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    map = (int*)mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    /* Now write int's to the file as if it were memory (an array of ints).
     */
    for (i = 1; i <=NUMINTS; ++i)
    {
        map[i] = 2 * i;
    }

    /* Don't forget to free the mmapped memory
     */
    if (munmap(map, FILESIZE) == -1)
    {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

void test_case_mmap2_logging()
{
    int fd;
    void* file_memory;

    /* Prepare a file large enough to hold an unsigned integer.  */
    fd = open ("mapTester", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    //Make the file big enough
    lseek (fd, 4 * 10 + 1, SEEK_SET);
    ssize_t ret = write (fd, "", 1);
    (void)ret; //prevent unused warning
    lseek (fd, 0, SEEK_SET);

    /* Create the memory mapping.  */
    file_memory = mmap (0, 4 * 10, PROT_WRITE, MAP_SHARED, fd, 0);
    close (fd);

    /* Write a random integer to memory-mapped area.  */
    sprintf((char*) file_memory, "%d\n", 22);

    /* Release the memory (unnecessary because the program exits).  */
    munmap (file_memory, 4 * 10);

    cout << "Mark" << endl;

//Start the part where I read from the file

    int integer;

    /* Open the file.  */
    fd = open ("mapTester", O_RDWR, S_IRUSR | S_IWUSR);

    /* Create the memory mapping.  */
    file_memory = mmap (0, 4 * 10, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    close (fd);

    /* Read the integer, print it out, and double it.  */
    sscanf ((char *) file_memory, "%d", &integer);
    printf ("value: %d\n", integer);
    sprintf ((char*) file_memory, "%d\n", 2 * integer);

    /* Release the memory (unnecessary because the program exits).  */
    munmap (file_memory, 4 * 10);
}

#include <iostream>
#include <string>
#include <cstring>
#include <boost/iostreams/device/mapped_file.hpp>
using std::cout;
using std::endl;
//http://www.boost.org/doc/libs/1_53_0/libs/iostreams/doc/classes/mapped_file.html
void test_case_mmap_boost_logging()
{
    const int blockSize = 64;
    bool writer = true;

    boost::iostreams::mapped_file_params  params;
    params.path = "map.dat";
    // params.length = 1024;     // default: all the file
    params.new_file_size = blockSize;

    if(writer)
    {
        cout << "Writer" << endl;
        params.mode = std::ios_base::out;
    }
    else
    {
        cout << "Reader" << endl;
        params.mode = std::ios_base::in;
    }

    boost::iostreams::mapped_file  mf;
    mf.open(params);

    if(writer)
    {
        char *block = mf.data();
        strcpy(block, "Test data block...\0");
        cout << "Written: " << block << endl;
    }
    else
    {
        cout << "Reading: " << mf.const_data() << endl;
    }

    mf.close();
}


#include <time.h>
void test_case_logging_ofstream()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    std::ofstream testlog, timelog;
    testlog.open ("ofstream_testlog.log", ios::out);
    timelog.open ("ofstream_timelog.log", ios::out);


    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);


    cout << "Starting test using ofstream." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif
        testlog << testline.str().c_str() << "\n";
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

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
    testlog.close();

    timelog << "time_s" << "\t"
            << "time_us" << "\t"
            << "runtime_ms"
            << "\n";
    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            timelog << v_fTime_s.at(ii) << "\t"
                    << v_fTime_us.at(ii) << "\t"
                    << fixed << 1.e+3*v_fDifftime.at(ii)
                    << "\n";
        }
    }
    timelog.close();
}

void test_case_logging_ofstream_nobuffer()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    std::ofstream testlog, timelog;
    testlog.open ("ofstream_nobuffer_testlog.log", ios::out);
    testlog.rdbuf()->pubsetbuf(0,0);

    timelog.open ("ofstream_nobuffer_timelog.log", ios::out);


    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using ofstream without buffer." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif
        testlog << testline.str().c_str() << "\n";
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
                 << "\n";
        }
#endif
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

    vector<float>::iterator  itr_max_dT =    max_element( v_fDifftime.begin(), v_fDifftime.end() );
    vector<float>::iterator  itr_min_dT =    min_element( v_fDifftime.begin(), v_fDifftime.end() );

    fl_difftime = accumulate(v_fDifftime.begin(), v_fDifftime.end(), 0.0);
    std::cout << NUM_LINES                      << " bytes in "
              << fl_difftime                    << " s, "
              <<  1.e+6*fl_difftime/NUM_LINES   << " us/line, "
              << NUM_LINES*64/fl_difftime/1024  << " kbyte/s, "
              << "min: " << *itr_min_dT*1e+3    << " ms, "
              << "max: " << *itr_max_dT*1e+3    << " ms "
              << "\n";
    testlog.close();

    timelog << "time_s" << "\t"
            << "time_us" << "\t"
            << "runtime_ms"
            << "\n";
    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            timelog << v_fTime_s.at(ii) << "\t"
                    << v_fTime_us.at(ii) << "\t"
                    << fixed << 1.e+3*v_fDifftime.at(ii)
                    << "\n";
        }
    }
    timelog.close();
}

void test_case_logging_ofstream_write()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    std::ofstream testlog, timelog;
    testlog.open ("ofstream_write_testlog.log", ios::out);
    timelog.open ("ofstream_write_timelog.log", ios::out);


    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using ofstream with write." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif
        testlog.write(testline.str().c_str(), testline.str().length());
        testlog.write("\n", 1);

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

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
    testlog.close();

    timelog << "time_s" << "\t"
            << "time_us" << "\t"
            << "runtime_ms"
            << "\n";
    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            timelog << v_fTime_s.at(ii) << "\t"
                    << v_fTime_us.at(ii) << "\t"
                    << fixed << 1.e+3*v_fDifftime.at(ii)
                    << "\n";
        }
    }
    timelog.close();
}

void test_case_logging_fwrite()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE *testlog, *timelog;
    testlog = fopen("fwrite_testlog.log","w");
    timelog = fopen("fwrite_timelog.log","w");

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using fwrite." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        fwrite(testline.str().c_str(), 1, testline.str().length(), testlog);
        fprintf(testlog, "\n");

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

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
    fclose(testlog);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_fwrite_sync()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE *testlog, *timelog;
    int fd = open("fwrite_sync_testlog.log", O_CREAT | O_WRONLY | O_DIRECT |O_SYNC, 00666);
    BOOST_REQUIRE_MESSAGE(fd != -1, "Could not open file descriptor!");

    testlog = fdopen(fd,"w");
    BOOST_REQUIRE_MESSAGE(testlog != NULL, "Could not open testlog file!");

    timelog = fopen("fwrite_sync_timelog.log","w");
    BOOST_REQUIRE_MESSAGE(timelog != NULL, "Could not open timelog file!");

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using fwrite with sync." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        fwrite(testline.str().c_str(), 1, testline.str().length(), testlog);
        fprintf(testlog, "\n");

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

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
    fclose(testlog);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_fwrite_direct()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE *testlog, *timelog;
    int fd = open("fwrite_direct_testlog.log", O_CREAT | O_WRONLY | O_DIRECT , 00666);
    BOOST_REQUIRE_MESSAGE(fd != -1, "Could not open file descriptor!");

    testlog = fdopen(fd,"w");
    BOOST_REQUIRE_MESSAGE(testlog != NULL, "Could not open testlog file!");

    timelog = fopen("fwrite_direct_timelog.log","w");
    BOOST_REQUIRE_MESSAGE(timelog != NULL, "Could not open timelog file!");

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using fwrite with direct." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        fwrite(testline.str().c_str(), 1, testline.str().length(), testlog);
        fprintf(testlog, "\n");

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    }

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
    fclose(testlog);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_mmap_sprintf()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE  *timelog;
    void* file_memory;
    int result;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

#if !defined(WITH_BOOST_TIME)
    clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
    start_time = boost::posix_time::microsec_clock::local_time();
#endif
    int fd = open ("mmap_sprintf_testlog.log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    timelog = fopen("mmap_sprintf_timelog.log","w");

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    const int FILESIZE = (testline.str().length()+1) * NUM_LINES;
    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, FILESIZE-1, SEEK_SET);
    if (result == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
    * have the file actually have the new size.
    * Just writing an empty string at the current file position will do.
    *
    * Note:
    *  - The current position in the file is at the end of the stretched
    *    file due to the call to lseek().
    *  - An empty string is actually a single '\0' character, so a zero-byte
    *    will be written at the last byte of the file.
    */
    result = write(fd, "", 1);
    if (result != 1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }
#if !defined(WITH_BOOST_TIME)
    clock_gettime(CLOCK_REALTIME, &ts_end);
    f_DT_s = (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9;
#else
    stop_time = boost::posix_time::microsec_clock::local_time();
    time_duration = (stop_time - start_time);
    f_DT_s = (time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
#endif
    cout << "Initialization of mmap-file takes: "
         << 1.e+3*f_DT_s << " ms."
         << endl;

    /* Now the file is ready to be mmapped.
     */
    file_memory = (void*)mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_memory == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    cout << "Starting test using mmap with sprintf." << endl;
    int nPos = 0;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        nPos += sprintf( ((char*)file_memory)+nPos, "%s\n", testline.str().c_str() );

//        strcat((char*)file_memory, testline.str().c_str()); // very slow!!!
//        strcat((char*)file_memory, "\n");

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    } // for

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

    /* Don't forget to free the mmapped memory
     */
    if (munmap(file_memory, FILESIZE) == -1)
    {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_mmap_strcat()
{
    int NUM_LINES = 2e+4;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE  *timelog;
    void* file_memory;
    int result;

    int fd = open ("mmap_strcat_testlog.log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    timelog = fopen("mmap_strcat_timelog.log","w");

    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    const int FILESIZE = (testline.str().length()+1) * NUM_LINES;
    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, FILESIZE-1, SEEK_SET);
    if (result == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
    * have the file actually have the new size.
    * Just writing an empty string at the current file position will do.
    *
    * Note:
    *  - The current position in the file is at the end of the stretched
    *    file due to the call to lseek().
    *  - An empty string is actually a single '\0' character, so a zero-byte
    *    will be written at the last byte of the file.
    */
    result = write(fd, "", 1);
    if (result != 1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    file_memory = (void*)mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_memory == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    cout << "Starting test using mmap with strcat." << endl;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        strcat((char*)file_memory, testline.str().c_str()); // very slow!!!
        strcat((char*)file_memory, "\n");

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    } // for

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

    /* Don't forget to free the mmapped memory
     */
    if (munmap(file_memory, FILESIZE) == -1)
    {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_mmap_memcpy()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE  *timelog;
    void* file_memory;
    int result;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

#if !defined(WITH_BOOST_TIME)
    clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
    start_time = boost::posix_time::microsec_clock::local_time();
#endif
    int fd = open ("mmap_memcpy_testlog.log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    timelog = fopen("mmap_memcpy_timelog.log","w");

    char buffer[63];

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        buffer[ii] = jj;
    }

    const int FILESIZE = (sizeof(buffer)+1) * NUM_LINES;
    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, FILESIZE-1, SEEK_SET);
    if (result == -1)
    {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
    * have the file actually have the new size.
    * Just writing an empty string at the current file position will do.
    *
    * Note:
    *  - The current position in the file is at the end of the stretched
    *    file due to the call to lseek().
    *  - An empty string is actually a single '\0' character, so a zero-byte
    *    will be written at the last byte of the file.
    */
    result = write(fd, "", 1);
    if (result != 1)
    {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    file_memory = (void*)mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_memory == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
#if !defined(WITH_BOOST_TIME)
    clock_gettime(CLOCK_REALTIME, &ts_end);
    f_DT_s = (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9;
#else
    stop_time = boost::posix_time::microsec_clock::local_time();
    time_duration = (stop_time - start_time);
    f_DT_s = (time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
#endif
    cout << "Initialization of mmap-file takes: "
         << 1.e+3*f_DT_s << " ms."
         << endl;

    vector<long> v_fTime_s;
    vector<long> v_fTime_us;
    vector<float> v_fDifftime;

    cout << "Starting test using mmap with memcpy." << endl;
    int nPos = 0;
    for(jj=0; jj < NUM_LINES; jj++)
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
        start_time = boost::posix_time::microsec_clock::local_time();
#endif

        memcpy((void*)((char*)file_memory+nPos), (void*)buffer, sizeof(buffer));
        memcpy((void*)file_memory, "\n", 1);
        nPos += sizeof(buffer)+1;

#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
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
        boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
    } // for

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

    /* Don't forget to free the mmapped memory
     */
    if (munmap(file_memory, FILESIZE) == -1)
    {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);

    fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

    for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
    {

        if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
        {
            fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
        }
    }
    fclose(timelog);
}

void test_case_logging_mmap_boost()
{
    int NUM_LINES = 2e+5;
    float fl_difftime = 0.;
    float f_DT_s = 0.;

#if !defined(WITH_BOOST_TIME)
    timespec ts_beg, ts_end;
#else
    boost::posix_time::ptime start_time, stop_time;
    boost::posix_time::time_duration time_duration;
#endif

    FILE  *timelog;
    void* file_memory;

    struct sched_param thread_param;
    thread_param.sched_priority = 10; //sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    boost::iostreams::mapped_file_params  params;
    params.path = "mmap_boost_testlog.log";

#if !defined(WITH_BOOST_TIME)
    clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
    start_time = boost::posix_time::microsec_clock::local_time();
#endif
    timelog = fopen("mmap_boost_timelog.log","w");
    ostringstream testline;

    int jj=0;
    for(int ii=0; ii < 8*8-1; ii++) //64 = 63 chars + end line
    {
        if(++jj > 9) jj = 0;
        testline << jj;
    }

    const int FILESIZE = (testline.str().length()+1) * NUM_LINES;
    //params.length = 1024;     // default: all the file
    params.new_file_size = FILESIZE;

    boost::iostreams::mapped_file_sink  mf;
    mf.open(params);
    if(mf.is_open())
    {
#if !defined(WITH_BOOST_TIME)
        clock_gettime(CLOCK_REALTIME, &ts_end);
        f_DT_s = (ts_end.tv_sec - ts_beg.tv_sec)  + (ts_end.tv_nsec - ts_beg.tv_nsec) / 1e9;
#else
        stop_time = boost::posix_time::microsec_clock::local_time();
        time_duration = (stop_time - start_time);
        f_DT_s = (time_duration.total_seconds()+ time_duration.fractional_seconds()/ 1e6);
#endif
        cout << "Initialization of mmap-file takes: "
             << 1.e+3*f_DT_s << " ms."
             << endl;

        /* Now the file is ready to be mmapped.
         */
        file_memory = (void*)mf.data();

        if (file_memory == NULL)
        {
            perror("Error mmapping the file");
            exit(EXIT_FAILURE);
        }

        vector<long> v_fTime_s;
        vector<long> v_fTime_us;
        vector<float> v_fDifftime;

        cout << "Starting test using boost-mmap with sprintf." << endl;
        int nPos = 0;
        for(jj=0; jj < NUM_LINES; jj++)
        {
#if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_REALTIME, &ts_beg); // http://linux.die.net/man/3/clock_gettime
#else
            start_time = boost::posix_time::microsec_clock::local_time();
#endif

            nPos += sprintf( ((char*)file_memory)+nPos, "%s\n", testline.str().c_str() );

#if !defined(WITH_BOOST_TIME)
            clock_gettime(CLOCK_REALTIME, &ts_end);
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
            boost::this_thread::sleep(boost::posix_time::microseconds(1e+6*f_DT_s)); //about 50% CPU load
        } // for

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

        mf.close();

        fprintf(timelog, "time_s\ttime_us\truntime_ms\n");

        for(unsigned int ii=0; ii < v_fTime_s.size(); ii++)
        {

            if(0.5 < 1.e+3*v_fDifftime.at(ii)) // log only values above 0.5 ms
            {
                fprintf(timelog,"%ld\t%ld\t%lf\n", v_fTime_s.at(ii), v_fTime_us.at(ii), 1.e+3*v_fDifftime.at(ii));
            }
        }
        fclose(timelog);
    }
    else
    {
        cout << "Failed to open memory map!" << endl;
    }
}


