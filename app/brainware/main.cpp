#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <signal.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp> //http://www.boost.org/doc/libs/1_52_0/libs/timer/doc/cpu_timers.html

using namespace std;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

// includes from: .
#include "gps_module.h"
#include "imu_module.h"
#include "ecf_module.h"
#include "ctrl_module.h"
#include "fcci_module.h"
#include "datalink_module.h"

#define DEBUGMSG(s) cout<<(s)

struct st_options
{
    string gcs_udp_addr;
    int logging_rate;
};
st_options options;


ctrl_module m_ctrl;
fcci_module m_fcci;
datalink_module m_datalink;



bool execute = true;
//***************************************
void trap(int signal)
{
    cout << "\n SIGINT received!" << endl;
    execute = false;
}

//***************************************
bool parse_options(st_options& opt, int argc, char **argv)
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("gcs-ip", po::value<string>(&opt.gcs_udp_addr), "IP address for sending data to.")
    ("lograte", po::value<int>(&opt.logging_rate), "Max. logging data rate in Hz. (set 0 or less to disable it).")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return false;
    }

    return true;
}

//***************************************
int main(int argc, char **argv )
//***************************************
{
   // register Ctrl-C signal
    signal(SIGINT, &trap);

//ToDo: servo channels vom fcci zur GCS schicken, => Problem: Wir bekommen nur 4 channels vom FCCI!?
// fcci communication testen + GCS channel Anzeige
//  MAG calibrierung testen bzw. Button in GCS suchen,
//  logging & timing testen
//Verbesserung: signals/ static register? Konzept???


//    gps_module m_gps;
//    imu_module m_imu;
//
//    //std::cout << m_imu.get_imu_ptr()->getIMUConfigString("# ");
//
//    ecf_module m_ecf(m_imu.get_imu_ptr(), m_imu.get_mag_ptr(), NULL);
//
//    // TODO (sven#1#): ctrl commands from DataLink or fcci!?
//    ctrl_module m_ctrl(m_ecf.get_sf_ptr());
//    fcci_module m_fcci(&m_ctrl); // Ihhhhh!

//    datalink_module m_datalink(m_imu.get_imu_ptr(),
//                               m_imu.get_mag_ptr(),
//                               m_imu.get_baro_ptr(),
//                               m_gps.get_gps_ptr(),
//                               m_ecf.get_sf_ptr());
    //datalink_module m_datalink(NULL,NULL,NULL,NULL,NULL);
//    datalink_module m_datalink;
    //m_ctrl.setDataLink(m_datalink.signal_mag_calibration);

//    m_datalink.signal_mag_calibration.connect( boost::bind(&imu_module::run_mag_calibration, &m_imu) );
    //GCSlink.signal_mag_calibration.connect( boost::bind(&Drotek10dof::run_mag_calibration, &imu10dof) );


    options.logging_rate = 1000; // max. default logging rate. // TODO (sven#2#): Nice but not used!
    options.gcs_udp_addr = "10.10.10.1"; // default GCS-IP address.

    if (!parse_options(options, argc, argv))
        return 1;

    if(!options.gcs_udp_addr.empty())
    {
        cout << "Trying to send to: "<< options.gcs_udp_addr << endl;
        m_datalink.connect(options.gcs_udp_addr);
    }
    if(0 < options.logging_rate)
    {
        cout << "Max. logging rate set to: "<< options.logging_rate << " Hz." << endl;
    }
    else
    {
        cout << "Logging disabled! "<< endl;
    }

    boost::timer::auto_cpu_timer t(1,"Program has within the %w s run time the CPU used for %t s (%p \%).\n\n");

//    // set properties of main thread
//    struct sched_param thread_param;
//    thread_param.sched_priority = 20; //sched_get_priority_max(SCHED_FIFO);
//    pthread_setschedparam(pthread_self(), SCHED_FIFO, &thread_param);

    //hmm, just wait here - or something to do?
    while(execute)
    {
        boost::this_thread::sleep(pt::seconds(1)); //arround 1 Hz
    }
    cout << "Bye bye!" << endl;
    return(0);
}

