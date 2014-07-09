#ifndef CTRL_MODULE_H
#define CTRL_MODULE_H

#include "ControlSystem/ControlSystem.h"
#include "SensorFusion/SensorFusion.h"

class ctrl_module
{
    public:
        ctrl_module();
        ctrl_module(SensorFusion*);

// TODO (sven#1#): Better with cServoChannels!? ...
        void getControl(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us);

//        iCTRL* get_ctrl_ptr(){return(&CtrlLoop);};

        virtual ~ctrl_module();

        boost::signal<void (void)>  signal_newdata;

    protected:
    private:
        ControlSystem CtrlLoop;
        SensorFusion* m_sf;

        void begin(void);
        void set_default_gains(void);
        void start_thread(void);
        void loop(void);
        void update(void);

        boost::thread the_thread;
        bool thread_running;
};

#endif // CTRL_MODULE_H
