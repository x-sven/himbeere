#ifndef CTRL_MODULE_H
#define CTRL_MODULE_H

#include "ControlSystem/ControlSystem.h"
#include "SensorFusion/SensorFusion.h"
#include "cParameter/cParameter.h"

extern SensorFusion* m_sf;

class ctrl_module
{
    public:
        ctrl_module();
        ctrl_module(SensorFusion*);

// TODO (sven#1#): Better with cServoChannels!? ...
        void getControl(uint16_t *ped_us, uint16_t *col_us, uint16_t *lon_us, uint16_t *lat_us);

//        iCTRL* get_ctrl_ptr(){return(&CtrlLoop);};
        void set_gain(std::string name, float value);
        ~ctrl_module();

        boost::signals2::signal<void (void)>  signal_newdata;

    protected:
    private:
    enum Ctrl_Loop
    {
        eCtrl_Loop_Vel_U = 0,
        eCtrl_Loop_Vel_V,
        eCtrl_Loop_Vel_W,
        eCtrl_Loop_Yaw_angle,
        eCtrl_Loop_max_number
    };

        ControlSystem CtrlLoops[eCtrl_Loop_max_number];

        void begin(void);
        void set_default_gains(void);
        void start_thread(void);
        void loop(void);
        void update(void);

        boost::thread the_thread;
        bool thread_running;

        uint16_t m_in_ped_us, m_in_col_us, m_in_lon_us, m_in_lat_us;
        uint16_t m_out_ped_us, m_out_col_us, m_out_lon_us, m_out_lat_us;

};

#endif // CTRL_MODULE_H
