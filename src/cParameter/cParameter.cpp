#include "cParameter.h"

#include <iostream>
#include <stdio.h>
#include <string.h>


cParameter::cParameter(float* _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    sys_id = _sys_id;
    comp_id = _comp_id;
    value=_value;
    name=_name;
}

cParameter* cParameter::set(float _value, std::string _name,float* _parameter_ptr)
{
    return set(_value, _name, _parameter_ptr, 0, 0);
}

cParameter* cParameter::set(float _value, std::string _name,float* _parameter_ptr, uint16_t _comp_id)
{
    return set(_value, _name, _parameter_ptr, _comp_id, 0);
}

cParameter* cParameter::set(float _value, std::string _name,float* _parameter_ptr, uint16_t _comp_id, uint16_t _sys_id)
{
    if(NULL != _parameter_ptr){
        cParameter* p_new_parameter(new cParameter(_parameter_ptr, _name, _comp_id, _sys_id));
        {
            std::vector<cParameter*>::iterator it = exist(_name);
            if(cParameter::get_instances()->end() == it) // parameter does not exists
            {
                get_instances()->push_back(p_new_parameter);
            }
            else                                        //parameter exists at "index"-pos and will be replaced!
            {
                get_instances()->insert(it, p_new_parameter);
            }
            p_new_parameter->set_value(_value);
        }
        return p_new_parameter;
    }
    return NULL;
}

std::vector<cParameter*>::iterator cParameter::exist(std::string name)
{
    std::vector<cParameter*>::iterator it = cParameter::get_instances()->begin();

    if(0 < cParameter::get_instances()->size())
    {
        while(  it != cParameter::get_instances()->end()
                && 0 != name.compare( (*it)->get_name()) ) // order importend, otherwise seg.fault!
        {
            it++;
        }
    }
    return(it);
}

cParameter::~cParameter()
{
    //get_instances()->erase(this);
}
