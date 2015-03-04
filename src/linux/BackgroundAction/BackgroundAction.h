#ifndef __BACKGROUNDPROCESS_H__
#define __BACKGROUNDPROCESS_H__

#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

class BackgroundAction
{
    public:
        BackgroundAction();
        virtual ~BackgroundAction();

        void update(void);
        boost::signals2::signal<void (void)> signal_trigger;

    protected:
    private:
        void loop(void);

        boost::condition_variable thread_trigger;
        boost::mutex mut;

        bool thread_running;
        boost::thread background_thread;
};

#endif // __BACKGROUNDPROCESS_H__
