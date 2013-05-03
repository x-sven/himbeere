#include "BackgroundAction.h"

BackgroundAction::BackgroundAction()
{
    thread_running = true;
    background_thread = boost::thread( boost::bind(&BackgroundAction::loop, this));
}

void BackgroundAction::update(void)
{
    thread_trigger.notify_all();
}

void BackgroundAction::loop(void)
{
    boost::unique_lock<boost::mutex> lock(mut);

    while(thread_running)
    {
        thread_trigger.wait(lock);
        signal_trigger();
    }
}

BackgroundAction::~BackgroundAction()
{
    thread_running = false;
    thread_trigger.notify_all();
    background_thread.join();
}
