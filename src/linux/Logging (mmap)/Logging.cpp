#include "Logging.h"

//#define DEBUG

#if defined(WITH_STDIO)
#define Debug(fmt, args ...)  {printf("Logging::%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); }
#else
#define Debug(fmt, args ...)
#endif

using namespace std;


Logging::Logging(): FileSize(10*1024*1024)
{
    byte_written = 0;
}

bool Logging::FreeSpace(boost::filesystem::path p)
{
    bool retval = false;
    try
    {
        boost::filesystem::space_info s = boost::filesystem::space(p);
        if(s.available > 50000000) //approx 48MB
        {
            retval = true;
#if defined(DEBUG)
            std::cout << s.available   << " Bytes available for logging." << std::endl;
#endif// (DEBUG)
        }
    }
    catch (boost::filesystem::filesystem_error &e)
    {
#if defined(DEBUG)
        std::cerr << e.what() << std::endl;
#endif //DEBUG
    }
    return(retval);
}

bool Logging::NewFile(const char* basename)
{
    uint16_t counter = 0;
    bool retval = false;

    std::ostringstream logfile;
    logfile << "_" << counter << "_" << basename;

    while (boost::filesystem::exists(boost::filesystem::path( logfile.str() )))
    {
#if defined(DEBUG)
        std::cout << logfile.str() << " already exists!" << std::endl;
#endif //DEBUG
        logfile.str("");
        logfile << "_" << ++counter << "_" << basename;
    }
#if defined(DEBUG)
    std::cout << "--> Using " << logfile.str() <<" for logging" << std::endl;
#endif //DEBUG

    if( FreeSpace("/") )
    {
        boost::iostreams::mapped_file_params  params;
        params.path = logfile.str().c_str();
        params.new_file_size = FileSize;
        params.mode = std::ios_base::out;

        if (_logfile.is_open())
        {
            _logfile.close();
        }
        _logfile.open(params);
//        _logfile.open (logfile.str().c_str(), ios::out);
//        _logfilename = logfile.str().c_str();
        write(std::ostringstream().flush() << string_headline);
        byte_written = string_headline.length();
        retval = true;
    }

    if (!_logfile.is_open())
    {
        Debug("Could not open logfile!");
        retval = false;
    }
//    else
//    {
//        _logfile.close();
//    }
    return(retval);
}

void Logging::begin(const char* basename)
{
    _logfilebase = basename;
    if(true == NewFile( _logfilebase.c_str() ))
    {
        thread_running = true;
        background_thread = boost::thread( boost::bind(&Logging::loop, this));
    }
}

void Logging::record(const char* data)
{
    if( FreeSpace("/") )
    {
        strcpy(_logfile.data(), data);
//        _logfile.open (_logfilename.c_str(), ios::out|ios::app);
//        _logfile << data;
//        _logfile.close();
    }
}

void Logging::headline(std::ostream &data)
{
    std::ostringstream& s = dynamic_cast<std::ostringstream&>(data);
    string_headline = s.str();
}

void Logging::write(const char* data)
{
    this->write_buffer(data);
}

void Logging::write(std::ostream &data)
{
    std::ostringstream& s = dynamic_cast<std::ostringstream&>(data);
    //this->record((const char*)s.str().c_str());
    this->write_buffer((const char*)s.str().c_str());
}

void Logging::write_buffer(const char* data)
{
    mutex_buffer.lock();
    string_buffer += data;
    mutex_buffer.unlock();
    if(4*1024 < string_buffer.length()) //write 4 kbyte blocks
    {
        thread_trigger.notify_all();
    }
}

void Logging::loop(void)
{
    boost::unique_lock<boost::mutex> lock(mutex);

    while(thread_running)
    {
        thread_trigger.wait(lock);
        if(FileSize < byte_written+string_buffer.length())
        {
            NewFile(_logfilebase.c_str());
        }
        mutex_buffer.lock();
        this->record((const char*)string_buffer.c_str());
        byte_written += string_buffer.length();
        string_buffer.clear();
        mutex_buffer.unlock();
    }
}

void Logging::end(void)
{
    thread_running = false;
    thread_trigger.notify_all();
    background_thread.join();

    if (_logfile.is_open())
    {
        _logfile.close();
#if defined(DEBUG)
        if (_logfile.is_open())
            Debug("Could not close logfile: %s, is still open!", _logfilename.c_str());
#endif //DEBUG
    }
#if defined(DEBUG)
    else
    {
        if (_logfile.is_open())
            Debug("Could not close logfile: %s, because not open.", _logfilename.c_str());
    }
#endif //DEBUG
}

Logging::~Logging()
{
    //dtor
}
