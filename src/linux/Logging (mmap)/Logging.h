#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <boost/signal.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/iostreams/device/mapped_file.hpp>

class Logging
{
public:
    Logging();
    virtual ~Logging();

    void begin(const char* filename);
    void end(void);

    void headline(std::ostream &data);

    void write(const char* data);
    void write(std::ostream &data);

    const uint32_t FileSize;

protected:
private:
    bool FreeSpace(boost::filesystem::path p);
    bool NewFile(const char* basename);

    boost::iostreams::mapped_file _logfile;
    std::string _logfilename, _logfilebase;
    uint32_t byte_written;

    void loop(void);
    void write_buffer(const char* data);
    void record(const char* data);

    std::string string_buffer;
    std::string string_headline;

    bool thread_running;
    boost::thread background_thread;

    boost::condition_variable thread_trigger;
    boost::mutex mutex;
    boost::mutex mutex_buffer;
};

#endif // _LOGGING_H_
