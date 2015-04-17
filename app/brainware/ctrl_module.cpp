#include <stdio.h>
#include "ctrl_module.h"
#include "fcci_module.h"

#ifndef constrain
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

extern fcci_module m_fcci;


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
    m_in_ped_us=m_in_col_us=m_in_lon_us=m_in_lat_us=1500;
    m_out_ped_us=m_out_col_us=m_out_lon_us=m_out_lat_us=1500;

    if(NULL != m_sf)
    {
        update(); // run once for initialization
    }
    start_thread();
}

void ctrl_module::set_gain(std::string name, float value)
{
    if (strcmp(name.c_str(), "XY"))
    {
        CtrlLoops[eCtrl_Loop_Vel_U].setGainKp(value);
    }
    else
    {
        std::cerr << "Gain does not exist in ctrl_module! (" << name << ")" << std::endl;
    }

}

void ctrl_module::set_default_gains(void)
{
    const uint16_t CTRL_ID = 0;
    // U-Loop
    cParameter::set( 1.0 , "c_u_gain_p"   , CtrlLoops[eCtrl_Loop_Vel_U].Kp(), CTRL_ID );
    cParameter::set( 0.0 , "c_u_gain_i"   , CtrlLoops[eCtrl_Loop_Vel_U].Ki(), CTRL_ID );
    cParameter::set( 0.0 , "c_u_gain_d"   , CtrlLoops[eCtrl_Loop_Vel_U].Kd(), CTRL_ID );
    cParameter::set( -1.0, "c_u_int_l_ms" , CtrlLoops[eCtrl_Loop_Vel_U].ErrorMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_u_int_h_ms" , CtrlLoops[eCtrl_Loop_Vel_U].ErrorMax(), CTRL_ID );

    cParameter::set( -1.0, "c_u_lim_l_rad" , CtrlLoops[eCtrl_Loop_Vel_U].ControlMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_u_lim_h_rad" , CtrlLoops[eCtrl_Loop_Vel_U].ControlMax(), CTRL_ID );


//    CtrlLoops[eCtrl_Loop_Vel_U].setControlMinMax  ( cParameter::set(-1.0 , "c_u_lim_l_rad", CTRL_ID)->get_value(), // about 60deg
//                                                    cParameter::set( 1.0 , "c_u_lim_h_rad", CTRL_ID)->get_value());
//    // V-Loop

    cParameter::set( 1.0 , "c_v_gain_p"   , CtrlLoops[eCtrl_Loop_Vel_V].Kp(), CTRL_ID );
    cParameter::set( 0.0 , "c_v_gain_i"   , CtrlLoops[eCtrl_Loop_Vel_V].Ki(), CTRL_ID );
    cParameter::set( 0.0 , "c_v_gain_d"   , CtrlLoops[eCtrl_Loop_Vel_V].Kd(), CTRL_ID );
    cParameter::set( -1.0, "c_v_int_l_ms" , CtrlLoops[eCtrl_Loop_Vel_V].ErrorMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_v_int_h_ms" , CtrlLoops[eCtrl_Loop_Vel_V].ErrorMax(), CTRL_ID );

    cParameter::set( -1.0, "c_v_lim_l_rad" , CtrlLoops[eCtrl_Loop_Vel_V].ControlMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_v_lim_h_rad" , CtrlLoops[eCtrl_Loop_Vel_V].ControlMax(), CTRL_ID );


//    // V-Loop
//    CtrlLoops[eCtrl_Loop_Vel_V].setGainKp         ( cParameter::set( 1.0 , "c_v_gain_p" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_V].setGainKi         ( cParameter::set( 0.0 , "c_v_gain_i" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_V].setGainKd         ( cParameter::set( 0.0 , "c_v_gain_d" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_V].setErrorMinMax    ( cParameter::set(-1.0 , "c_v_int_l_ms", CTRL_ID)->get_value(), // 1m/s
//                                                    cParameter::set( 1.0 , "c_v_int_h_ms", CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_V].setControlMinMax  ( cParameter::set(-1.0 , "c_v_lim_l_rad", CTRL_ID)->get_value(), // about 60deg
//                                                    cParameter::set( 1.0 , "c_v_lim_h_rad", CTRL_ID)->get_value());


//    // W-Loop

    cParameter::set( 1.0 , "c_w_gain_p"   , CtrlLoops[eCtrl_Loop_Vel_W].Kp(), CTRL_ID );
    cParameter::set( 0.0 , "c_w_gain_i"   , CtrlLoops[eCtrl_Loop_Vel_W].Ki(), CTRL_ID );
    cParameter::set( 0.0 , "c_w_gain_d"   , CtrlLoops[eCtrl_Loop_Vel_W].Kd(), CTRL_ID );
    cParameter::set( -1.0, "c_w_int_l_ms" , CtrlLoops[eCtrl_Loop_Vel_W].ErrorMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_w_int_h_ms" , CtrlLoops[eCtrl_Loop_Vel_W].ErrorMax(), CTRL_ID );

    cParameter::set( -1.0, "c_w_lim_l_rad" , CtrlLoops[eCtrl_Loop_Vel_W].ControlMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_w_lim_h_rad" , CtrlLoops[eCtrl_Loop_Vel_W].ControlMax(), CTRL_ID );
//    CtrlLoops[eCtrl_Loop_Vel_W].setGainKp         ( cParameter::set( 1.0 , "c_w_gain_p" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_W].setGainKi         ( cParameter::set( 0.0 , "c_w_gain_i" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_W].setGainKd         ( cParameter::set( 0.0 , "c_w_gain_d" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_W].setErrorMinMax    ( cParameter::set(-1.0 , "c_w_int_l_ms", CTRL_ID)->get_value(), // 1m/s
//                                                    cParameter::set( 1.0 , "c_w_int_h_ms", CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Vel_W].setControlMinMax  ( cParameter::set(-1.0 , "c_w_lim_l_rad", CTRL_ID)->get_value(), // about 60deg
//                                                    cParameter::set( 1.0 , "c_w_lim_h_rad", CTRL_ID)->get_value());
//    // Heading-Loop
    cParameter::set( 1.0 , "c_yaw_gain_p"   , CtrlLoops[eCtrl_Loop_Yaw_angle].Kp(), CTRL_ID );
    cParameter::set( 0.0 , "c_yaw_gain_i"   , CtrlLoops[eCtrl_Loop_Yaw_angle].Ki(), CTRL_ID );
    cParameter::set( 0.0 , "c_yaw_gain_d"   , CtrlLoops[eCtrl_Loop_Yaw_angle].Kd(), CTRL_ID );
    cParameter::set( -1.0, "c_yaw_int_l_ms" , CtrlLoops[eCtrl_Loop_Yaw_angle].ErrorMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_yaw_int_h_ms" , CtrlLoops[eCtrl_Loop_Yaw_angle].ErrorMax(), CTRL_ID );

    cParameter::set( -1.0, "c_yaw_lim_l_rad" , CtrlLoops[eCtrl_Loop_Yaw_angle].ControlMin(), CTRL_ID );
    cParameter::set( 1.0 , "c_yaw_lim_h_rad" , CtrlLoops[eCtrl_Loop_Yaw_angle].ControlMax(), CTRL_ID );
//    CtrlLoops[eCtrl_Loop_Yaw_angle].setGainKp     ( cParameter::set( 1.0 , "c_yaw_gain_p" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Yaw_angle].setGainKi     ( cParameter::set( 0.0 , "c_yaw_gain_i" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Yaw_angle].setGainKd     ( cParameter::set( 0.0 , "c_yaw_gain_d" , CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Yaw_angle].setErrorMinMax( cParameter::set(-0.5 , "c_yaw_int_l_rad", CTRL_ID)->get_value(), //about 30 deg
//                                                    cParameter::set( 0.5 , "c_yaw_int_h_rad", CTRL_ID)->get_value());
//    CtrlLoops[eCtrl_Loop_Yaw_angle].setControlMinMax( cParameter::set(-1.0 , "c_yaw_lim_l_rad", CTRL_ID)->get_value(), // about 60deg
//                                                      cParameter::set( 1.0 , "c_yaw_lim_h_rad", CTRL_ID)->get_value());
}

void ctrl_module::start_thread(void)
{
    printf("Starting Controller Thread!\n");

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
        if(NULL != m_sf)
        {
            update();
            signal_newdata();
        }
        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_50Hz; // update timer
    }
    printf("Leaving Controller Thread!\n");
}

void ctrl_module::update(void)
{
    float vel_scale = 5.0;  //for scaling +/-1-inputs to vel cmd: vel = scale*ctrl_input;

    uint16_t dummy;
    m_fcci.getChannels(&dummy,  &m_in_col_us,  &m_in_lon_us,  &m_in_lat_us, &dummy, &dummy);

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
    float vel_geo_cmd[3] = {0., 0., 0.};
    for(size_t ii=0; ii<3; ii++)
    {
        vel_geo_cmd[ii] = 0.0;
        for(size_t jj=0; jj<3; jj++)
        {
            vel_geo_cmd[ii] += T_rot[ii][jj]*vel_cmd[jj];
        }
    }

    //error velocity in geo = scaled stick in geo - sensor vel in geo
    float vel_error_geo[3] = {0., 0., 0.};
    for(int ii=0; ii<3; ii++)
        vel_error_geo[ii]=vel_geo_cmd[ii]-m_sf->get_speed_ms(ii);

    float vel_error_body_star[3] = {0., 0., 0.};
    //Transform from geo to body*
    for(size_t ii=0; ii<3; ii++)
    {
        for(size_t jj=0; jj<3; jj++)
        {
            vel_error_body_star[ii] += T_rot[jj][ii]*vel_error_geo[jj];
        }
    }

    // vel. error, defined as: err=cmd-state
    float ctrl_cmd[3];
    for(size_t ii=eCtrl_Loop_Vel_U; ii <= eCtrl_Loop_Vel_W; ii++)
    {
        ctrl_cmd[ii] = CtrlLoops[ii].getControl(0.0f, vel_error_body_star[ii], 0.0f);
        //constrain(ctrl_cmd[ii], -1.0f, 1.0f);
    }
    // ToDo: ctrl_cmd[eCtrl_Loop_Yaw_angle] = CtrlLoops[eCtrl_Loop_Yaw_angle].getControl(???, m_sf->get_euler_angles_rad().yaw, 0.0f);
    m_out_ped_us = (uint16_t)m_in_ped_us;
    m_out_lon_us = (uint16_t)((ctrl_cmd[eCtrl_Loop_Vel_U] * 500.f) + 1500.f);
    m_out_lat_us = (uint16_t)((ctrl_cmd[eCtrl_Loop_Vel_V] * 500.f) + 1500.f);
    m_out_col_us = (uint16_t)((ctrl_cmd[eCtrl_Loop_Vel_W] * 500.f) + 1500.f);
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
    thread_running = false;
}
