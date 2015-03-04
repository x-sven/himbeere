#ifndef FCCI_MODULE_H
#define FCCI_MODULE_H

#include <stdint.h>
#include "HardwareSerial/HardwareSerial.h"
#include "ServoInterface/ServoInterface.h"
#include "cServochannels/c_parser.h"
#include "Logging/Logging.h"

#include "ctrl_module.h" // ugly this way!

extern ctrl_module m_ctrl;


class fcci_module
{
    public:
        fcci_module();
//        fcci_module(ctrl_module*);

        boost::signals2::signal<void (void)>  signal_newdata;
        std::string getString(void);
        void getChannels(uint16_t *ped_us, uint16_t *col_us, uint16_t *lon_us,
                         uint16_t *lat_us, uint16_t *aux_us, uint16_t *mod_us);
        void send_commands(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us);
        virtual ~fcci_module();
    protected:
    private:
        HardwareSerial *fcci_serial;
        Logging fccilog;
        cParser parser;


        boost::thread the_thread;
        bool thread_running;
        void begin(void);
        void loop(void);
        void ctrl_update(void);
        boolean bControlUpdated;

        uint8_t command_buffer[12];
        uint16_t ped_us, col_us, lon_us, lat_us; // better with cServoChannels!

        cServoChannels *fcci_in_channel;

        void fcci_log(void) { fccilog.data(this->getString()); }


};

#endif // FCCI_MODULE_H
