#ifndef __DATALOGGER_H__
#define __DATALOGGER_H__


#include "../BackgroundAction/BackgroundAction.h"
#include "../Logging/Logging.h"

class DataLogger: public Logging, public BackgroundAction
{
    public:
        DataLogger();
        virtual ~DataLogger();

        void record(void){};
    protected:
    private:
};

#endif // __DATALOGGER_H__
