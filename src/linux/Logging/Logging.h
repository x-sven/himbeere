#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

class Logging
{
public:
    Logging();
    virtual ~Logging();

    void begin(const char* filename);
    void end(void);

    void write(const char* data);
    void write(std::ostream &data);


protected:
private:
    bool FreeSpace(boost::filesystem::path p);

    std::ofstream _logfile;
    std::string _logfilename;

};

#endif // _LOGGING_H_
