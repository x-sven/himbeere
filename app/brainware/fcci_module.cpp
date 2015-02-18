#include "fcci_module.h"

#include "crc16/crc16.h"
#include "cServochannels/c_parser.h"

fcci_module::fcci_module()
{
//    m_ctrl = NULL;
    fcci_in_channel = new cServoChannels(_num_channels);
    fcci_module::begin();
}

//fcci_module::fcci_module(ctrl_module* _ctrl)
//{
//    m_ctrl = _ctrl;
//    fcci_module::begin();
//}

void fcci_module::begin(void)
{
    ped_us=col_us=lon_us=lat_us=0;

    fcci_serial = NULL;
    fcci_serial = new HardwareSerial("/dev/ttyAMA0");
    if(NULL != fcci_serial)
    {
        fcci_serial->begin(38400);
    }

    std::stringstream headline;

    for(size_t ii=0; ii<parser.get_channels().size(); ii++)
    {
        headline << "ch_" << parser.get_channels().getChannel(ii) << "\t";
    }
    headline << "ped_us" << "\t"
             << "col_us" << "\t"
             << "lon_us" << "\t"
             << "lat_us" << "\t"
             << "err_count"
             << std::endl;

    fccilog.header(headline.str());
    fccilog.begin("logfile_fcci.log", 5); // 10Hz

    // create thread
    thread_running = true;
    the_thread = boost::thread( boost::bind(&fcci_module::loop, this));

    struct sched_param thread_param;
    thread_param.sched_priority = 5;
    pthread_t threadID = (pthread_t) the_thread.native_handle();

    int retcode;
    if ((retcode = pthread_setschedparam(threadID, SCHED_FIFO, &thread_param)) != 0)
    {
        errno = retcode;
        perror("pthread_setschedparam");
    }

    bControlUpdated = false;
    m_ctrl.signal_newdata.connect( boost::bind(&fcci_module::ctrl_update,this) );
    fcci_module::signal_newdata.connect( boost::bind(&fcci_module::fcci_log,this) );
}

void fcci_module::loop(void)
{
    boost::posix_time::time_duration interval_50Hz(boost::posix_time::milliseconds(1000 / 50));
    boost::posix_time::ptime timer = boost::posix_time::microsec_clock::local_time() + interval_50Hz;

    while(thread_running)
    {
        int16_t numc = fcci_serial->available();
        if(0 < numc)
        {
            for (int16_t i=0; i < numc; i++)
            {
                parser.process_byte(fcci_serial->read());
            }
            if(true == bControlUpdated)
            {
                m_ctrl.getControl(&ped_us, &col_us, &lon_us, &lat_us);
                fcci_module::send_commands(ped_us, col_us, lon_us, lat_us);
                bControlUpdated = false;
            }
            if(parser.new_channels())
            {
                *fcci_in_channel = parser.get_channels();
                fcci_module::signal_newdata();
            }
        }
        boost::this_thread::sleep(timer - boost::posix_time::microsec_clock::local_time());
        timer += interval_50Hz; // update timer
    }
}

std::string fcci_module::getString(void)
{
    std::stringstream ss;

    for(size_t ii=0; ii<parser.get_channels().size(); ii++)
    {
        ss << parser.get_channels().getChannel(ii) << "\t";
    }
    ss << ped_us << "\t"
       << col_us << "\t"
       << lon_us << "\t"
       << lat_us << "\t"
       << parser.get_errors()
       << std::endl;

    parser.reset_errors();
    return(ss.str());
}

void fcci_module::ctrl_update(void)
{
    bControlUpdated = true;
}

void fcci_module::send_commands(uint16_t ped_us, uint16_t col_us, uint16_t lon_us, uint16_t lat_us)
{
    unsigned int ppm_min=900;
    unsigned int ppm_max=2100;
    uint8_t command_state = 0;


//    printf("cmd: %d %d %d %d\n", ped_us, col_us, lon_us, lat_us);

    if(lat_us<ppm_min || lat_us >ppm_max)
        command_state=1;
    if(lon_us<ppm_min || lon_us >ppm_max)
        command_state=1;
    if(ped_us<ppm_min || ped_us >ppm_max)
        command_state=1;
    if(col_us<ppm_min || col_us >ppm_max)
        command_state=1;

    if(0 == command_state)
    {
        command_buffer[2] = (ped_us>>8) & 0xff;
        command_buffer[3] = ped_us & 0xff  ;

        command_buffer[4] = (col_us>>8) & 0xff;
        command_buffer[5] = col_us & 0xff  ;

        command_buffer[6] = (lon_us>>8) & 0xff;
        command_buffer[7] = lon_us & 0xff  ;

        command_buffer[8] = (lat_us>>8) & 0xff;
        command_buffer[9] = lat_us & 0xff  ;


        unsigned short crc = crcFast(&command_buffer[2], 8);

        command_buffer[10] =  crc & 0xff;
        command_buffer[11] =  (crc>>8) & 0xff;

        fcci_serial->write((uint8_t*)&command_buffer, 12);
    }
}

void fcci_module::getChannels(uint16_t *ped_us, uint16_t *col_us, uint16_t *lon_us,
                              uint16_t *lat_us, uint16_t *aux_us, uint16_t *mod_us)
{
    *ped_us = fcci_in_channel->getChannel(rud_1);
    *col_us = fcci_in_channel->getChannel(thr_2);
    *lon_us = fcci_in_channel->getChannel(ele_3);
    *lat_us = fcci_in_channel->getChannel(ail_4);
    *aux_us = fcci_in_channel->getChannel(tbd_5);
    *mod_us = fcci_in_channel->getChannel(mod_6);
}


fcci_module::~fcci_module()
{
    fccilog.end();
    if(NULL != fcci_in_channel)
        delete(fcci_in_channel);

    delete(fcci_serial);
    fcci_serial = NULL;
}
