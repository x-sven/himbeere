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
    m_in_ped_us=m_in_col_us=m_in_lon_us=m_in_lat_us=0;
    m_out_ped_us=m_out_col_us=m_out_lon_us=m_out_lat_us=0;
}

void ctrl_module::set_default_gains(void)
{
    CtrlLoops[Ctrl_Loop_Vel_U].setGainKp(1.0);
    CtrlLoops[Ctrl_Loop_Vel_V].setGainKp(1.0);
    CtrlLoops[Ctrl_Loop_Vel_W].setGainKp(1.0);
    CtrlLoops[Ctrl_Loop_Yaw_angle].setGainKp(1.0);
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
    float vel_scale = 5.0;  //for scaling +/-1-inputs to vel cmd: vel = scale*ctrl_input;
    //uint16_t pilot[3] = {lon_us, lat_us, col_us};


    //velocity command from pilot input, represents flat earth vel. rotated by yaw
    float vel_cmd[3] = {vel_scale*((float)m_in_lon_us-1500.0f)/500.0f,
                        vel_scale*((float)m_in_lat_us-1500.0f)/500.0f,
                        vel_scale*((float)m_in_col_us-1500.0f)/500.0f
                       };
    // Yaw rotation matrix
    float psi = m_sf->get_euler_angles_rad().yaw;
    float T_rot[3][3] =
    {
        {std::cos(psi),-std::sin(psi), 0.0},
        {std::sin(psi), std::cos(psi), 0.0},
        {0.0, 0.0, 1.0 }
    };
    // geodetic velocity command from pilot wrt. vehicles yaw angle
    float vel_geo[3] = {0., 0., 0.};
    for(size_t ii=0; ii<3; ii++)
    {
        vel_geo[ii] = 0.0;
        for(size_t jj=0; jj<3; jj++)
        {
            vel_geo[ii] += T_rot[ii][jj]*vel_cmd[jj];
        }
    }
    // vel. error, defined as: err=cmd-state
    float ctrl_cmd[Ctrl_Loop_max_number];
    for(size_t ii=Ctrl_Loop_Vel_U; ii < Ctrl_Loop_Vel_W; ii++)
    {
        ctrl_cmd[ii] = CtrlLoops[ii].getControl(vel_geo[ii], m_sf->get_speed_ms(ii), 0.0f);
    }
    // ToDo: ctrl_cmd[Ctrl_Loop_Yaw_angle] = CtrlLoops[Ctrl_Loop_Yaw_angle].getControl(???, m_sf->get_euler_angles_rad().yaw, 0.0f);
    m_out_ped_us = (uint16_t)m_in_ped_us;
    m_out_lon_us = (uint16_t)((ctrl_cmd[Ctrl_Loop_Vel_U] * 500.f) + 1500.f);
    m_out_lat_us = (uint16_t)((ctrl_cmd[Ctrl_Loop_Vel_V] * 500.f) + 1500.f);
    m_out_col_us = (uint16_t)((ctrl_cmd[Ctrl_Loop_Vel_W] * 500.f) + 1500.f);
    signal_newdata();
}


void ctrl_module::getControl(uint16_t *ped_us, uint16_t *col_us, uint16_t *lon_us, uint16_t *lat_us)
{
    *ped_us = m_out_ped_us;
    *col_us = m_out_col_us;
    *lon_us = m_out_lon_us;
    *lat_us = m_out_lat_us;
}

ctrl_module::~ctrl_module()
{
    //dtor
}
