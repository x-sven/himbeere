#include "DataLogger.h"

DataLogger::DataLogger()
{
        //imulog.signal_trigger.connect( (boost::function<void (void)>)imu_log );
    BackgroundAction::signal_trigger.connect(boost::bind(&DataLogger::record, this)) ;
//    thread_running = true;
//    logger_thread = boost::thread( boost::bind(&DataLogger::loop, this));
}
//
//void DataLogger::update(void)
//{
//    thread_trigger.notify_all();
//}
//
//void DataLogger::loop(void)
//{
//    boost::unique_lock<boost::mutex> lock(mut);
//
//    while(thread_running)
//    {
//        thread_trigger.wait(lock);
//        signal_trigger();
//    }
//}
//
DataLogger::~DataLogger()
{
//    thread_running = false;
//    thread_trigger.notify_all();
//    logger_thread.join();
}
