#include "c_simulation.h"
#include "tricopter_simulation_rtw_capi.h"

#include <iostream>
#include <algorithm>    // std::min_element
#include <stdint.h>

c_simulation::c_simulation():
    simulation_rtw_object(NULL),
    m_model_running(false)
{
    simulation_rtw_object = (tricopter_simulation_rtwModelClass*)new tricopter_simulation_rtwModelClass();
}

c_simulation::~c_simulation()
{
    delete(simulation_rtw_object);
    simulation_rtw_object = NULL;
}

void c_simulation::update(void)
{
    if(NULL != simulation_rtw_object)
    {
        /* Step the model */
        simulation_rtw_object->step();
    }
    else
        m_model_running = false;
}

void c_simulation::begin(void)
{
    /* Initialize model */
    if(NULL != simulation_rtw_object)
    {
        m_model_running = true;
        simulation_rtw_object->initialize();
        #if defined(DEBUG)
        std::cout << "Simulation: model with sample period of "
                  << get_sample_time() << "s"
                  << std::endl;
        #endif //DEBUG
    }
    else
        m_model_running = false;

}

void c_simulation::end(void)
{
    /* Terminate model */
    if(NULL != simulation_rtw_object)
    {
        simulation_rtw_object->terminate();
    }
    m_model_running = false;
}

void c_simulation::set_input(float lat_us, float lon_us, float ped_us, float col_us)
{
 if(lat_us < 2100 && lat_us > 900)
    simulation_rtw_object->tricopter_simulation_rtw_U.deflections.lat_us = lat_us;
 if(lon_us < 2100 && lon_us > 900)
    simulation_rtw_object->tricopter_simulation_rtw_U.deflections.lon_us = lon_us;
 if(ped_us < 2100 && ped_us > 900)
    simulation_rtw_object->tricopter_simulation_rtw_U.deflections.ped_us = ped_us;
 if(col_us < 2100 && col_us > 900)
    simulation_rtw_object->tricopter_simulation_rtw_U.deflections.col_us = col_us;
}

ExternalOutputs_tricopter_simul* c_simulation::get_output()
{
    return(&(simulation_rtw_object->tricopter_simulation_rtw_Y));
}

float c_simulation::get_sample_time()
{
    float dt[NUMST];

    if(NULL != simulation_rtw_object)
    {
        for(uint8_t ii=0; ii < NUMST; ii++)
        {
            rtwCAPI_ModelMappingInfo* capiMap =  &(simulation_rtw_object->getRTM()->DataMapInfo.mmi);
            const rtwCAPI_SampleTimeMap* stm = rtwCAPI_GetSampleTimeMap(capiMap);
            dt[ii] = rtwCAPI_GetSamplePeriod(stm,0);
        }
    }
    return(*std::min_element(dt, dt+NUMST));
}
