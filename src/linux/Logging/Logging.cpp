#include "Logging.h"


#if defined(WITH_STDIO)
#define Debug(fmt, args ...)  {printf("Logging::%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ## args); }
#else
#define Debug(fmt, args ...)
#endif

Logging::Logging()
{
    //ctor
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

void Logging::begin(const char* filename)
{
    uint16_t counter = 0;


    std::ostringstream logfile;
    logfile << "_" << counter++ << "_" << filename;

    while (boost::filesystem::exists(boost::filesystem::path( logfile.str() )))
    {
        #if defined(DEBUG)
        std::cout << logfile.str() << " already exists!" << std::endl;
        #endif //DEBUG
        logfile.str("");
        logfile << "_" << counter++ << "_" << filename;
    }
    #if defined(DEBUG)
    std::cout << "--> Using " << logfile.str() <<" for logging" << std::endl;
    #endif //DEBUG

    if( FreeSpace("/") )
    {
        _logfile.open (logfile.str().c_str(), ios::out);
        _logfilename = logfile.str().c_str();
    }

    if (!_logfile.is_open())
    {
        Debug("Could not open logfile!");
    }
    else
    {
        _logfile.close();
    }

}

void Logging::write(const char* data)
{
    if( FreeSpace("/") )
    {
        _logfile.open (_logfilename.c_str(), ios::out|ios::app);
        _logfile << data;
        _logfile.close();
    }
}

void Logging::write(std::ostream &data)
{
    std::ostringstream& s = dynamic_cast<std::ostringstream&>(data);
    this->write((const char*)s.str().c_str());
}

void Logging::end(void)
{


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
