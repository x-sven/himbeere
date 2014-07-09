#ifndef __DATALOGGER_H__
#define __DATALOGGER_H__

#include <boost/signal.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include "../Logging/Logging.h"

class DataLogger: public Logging
{
    public:
        DataLogger();
        virtual ~DataLogger();

        void update(void);
        boost::signal<void (void)> signal_trigger;

    protected:
    private:
        void loop(void);

        boost::condition_variable thread_trigger;
        boost::mutex mut;

        bool thread_running;
        boost::thread logger_thread;
};

#endif // __DATALOGGER_H__
