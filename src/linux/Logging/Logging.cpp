#include "Logging.h"


#if defined(WITH_STDIO)
#define Debug(fmt, args ...)  {printf("Logging::%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); }
#else
#define Debug(fmt, args ...)
#endif

using namespace std;


Logging::Logging()
{
    //ctor
    active_buffer = &string_buffer_1;
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
        //_logfile.open (logfile.str().c_str(), ios::out);
        _logfile = fopen(logfile.str().c_str(),"w");
        _logfilename = logfile.str().c_str();
        retval = true;
    }

    if (NULL == _logfile)
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
    if(true == NewFile(basename))
    {
        thread_running = true;
        background_thread = boost::thread( boost::bind(&Logging::loop, this));

        struct sched_param thread_param;
        thread_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
        int retcode;
        if ((retcode = pthread_setschedparam(background_thread.native_handle(), SCHED_FIFO, &thread_param)) != 0)
        {
            errno = retcode;
            perror("pthread_setschedparam");
        }
    }
}

//void Logging::record(const char* data)
//{
//    //if( FreeSpace("/") )
//    {
//        //_logfile.open (_logfilename.c_str(), ios::out|ios::app);
//        _logfile << data;
//        //_logfile.close();
//    }
//}

void Logging::write(const char* data)
{
    this->write_buffer(data);
}

void Logging::write(std::ostream &data)
{
    std::ostringstream& s = dynamic_cast<std::ostringstream&>(data);
    //this->record((const char*)s.str().c_str()); //direct
    this->write_buffer((const char*)s.str().c_str()); // with buffer
}

void Logging::write_buffer(const char* data)
{
    //mutex_buffer.lock();
    *active_buffer += data;
    //mutex_buffer.unlock();
    if(4*1024 < active_buffer->length()) //write 4 kbyte blocks
    {
        thread_trigger.notify_all();
    }
}

void Logging::loop(void)
{
    int buffer_no = 0;

    boost::unique_lock<boost::mutex> lock(mutex);
    while(thread_running)
    {
        thread_trigger.wait(lock);
        if(buffer_no < 2)
        {
            active_buffer = &string_buffer_2;
            fwrite(string_buffer_1.c_str(), sizeof(char), string_buffer_1.length(), _logfile);
            string_buffer_1.clear();
            buffer_no = 2;
        }
        else
        {
            active_buffer = &string_buffer_1;
            fwrite(string_buffer_2.c_str(), sizeof(char), string_buffer_2.length(), _logfile);
            string_buffer_2.clear();
            buffer_no = 1;
        }
//        thread_trigger.wait(lock);
//        this->record((const char*)string_buffer.c_str());
//        mutex_buffer.lock();
//        string_buffer.clear();
//        mutex_buffer.unlock();
    }
}

void Logging::end(void)
{
    thread_running = false;
    thread_trigger.notify_all();
    background_thread.join();

//    if (_logfile.is_open())
//    {
//        _logfile.close();
    fclose(_logfile);
//#if defined(DEBUG)
//        if (_logfile.is_open())
//            Debug("Could not close logfile: %s, is still open!", _logfilename.c_str());
//#endif //DEBUG
//    }
//#if defined(DEBUG)
//    else
//    {
//        if (_logfile.is_open())
//            Debug("Could not close logfile: %s, because not open.", _logfilename.c_str());
//    }
//#endif //DEBUG
}

Logging::~Logging()
{
    //dtor
}
