#include <stdio.h>
#include "ctrl_module.h"

ctrl_module::ctrl_module()
{
    m_sf = NULL;
    ctrl_module::begin();
}
ctrl_module::ctrl_module(SensorFusion* _sf)
{
    m_sf = _sf;
    ctrl_module::begin();
}

void ctrl_module::begin(void)
{
    set_default_gains();
    start_thread();
}

void ctrl_module::set_default_gains(void)
{
    CtrlLoop.setGainKp(1.0);
}


void ctrl_module::start_thread(void)
{
    // create thread
    thread_running = true;
    the_thread = boost::thread( boost::bind(&ctrl_module::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = 20;
    pthread_t threadID = (pthread_t) the_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }
}

void ctrl_module::loop(void)
{
    boost::posix_time::time_duration interval_50Hz(boost::posix_time::milliseconds(1000 / 50));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval_50Hz;

    while(thread_running)
    {
        update();
        signal_newdata();

        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_50Hz; // update timer
    }
}

void ctrl_module::update(void)
{
// TODO (sven#1#): We should update control here!

    float vel_scale = 5.0;
    float psi = 0.0;



    uint16_t ped_us, col_us,  lon_us, lat_us;

    uint16_t pilot[3] = {lon_us, lat_us, col_us};


    float vel_cmd[3] = {vel_scale*((float)lon_us-1500.)/500.,
                        vel_scale*((float)lat_us-1500.)/500.,
                        vel_scale*((float)col_us-1500.)/500.};

    float T_rot[3][3] = {{std::cos(psi),-std::sin(psi), 0.0},
                         {std::sin(psi), std::cos(psi), 0.0},
                         {0.0, 0.0, 1.0 }};

    float vel_geo[3] = {0., 0., 0.};
    float vel_err[3] = {0., 0., 0.};

    for(size_t ii=0; ii<3; ii++)
    {
        vel_geo[ii] = 0.0;
        for(size_t jj=0; jj<3; jj++)
        {
            vel_geo[ii] += T_rot[ii][jj]*vel_cmd[jj];
        }
    }

    for(size_t ii=0;ii<3;ii++)
        vel_err[ii]=vel_geo[ii] - m_sf->g


}


void ctrl_module::getControl(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us)
{

}

ctrl_module::~ctrl_module()
{
    //dtor
}
