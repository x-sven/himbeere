#ifndef _C_SIMULATION_H_
#define _C_SIMULATION_H_

#include "tricopter_simulation_rtw.h"  /* Model's header file */

class c_simulation
{
    public:
        /** Default constructor */
        c_simulation();
        bool is_running() {return(m_model_running);}
        void begin();
        void update();
        void end();

        void set_input(float lat_us, float lon_us, float ped_us, float col_us);
        ExternalOutputs_tricopter_simul* get_output(void);
        float get_sample_time();

        /** Default destructor */
        virtual ~c_simulation();
    protected:
    private:
        tricopter_simulation_rtwModelClass* simulation_rtw_object;
        bool m_model_running;

};

#endif // _C_SIMULATION_H_
