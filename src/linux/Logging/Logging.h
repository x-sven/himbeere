#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

class Logging
{
public:
    Logging();
    virtual ~Logging();

    void begin(const char* filename);
    void begin(const char* filename, unsigned short prescale);

    void end(void);

    void write(const char* data);
    void write(std::ostream &data);

    void data(const std::string str);
    void header(const std::string str);


protected:
private:
    bool FreeSpace(boost::filesystem::path p);
    bool NewFile(const char* basename);

    //std::ofstream _logfile;
    FILE *_logfile;
    std::string _logfilename;

    void loop(void);
    void write_buffer(const char* data);
    //void record(const char* data);

    std::string string_buffer_1, string_buffer_2;
    std::string *active_buffer;

    std::string m_headline_str;

    bool thread_running;
    boost::thread background_thread;

    boost::condition_variable thread_trigger;
    boost::mutex mutex;
    boost::mutex mutex_buffer;

    unsigned short m_prescale;
    unsigned short m_prescale_counter;
};

#endif // _LOGGING_H_
