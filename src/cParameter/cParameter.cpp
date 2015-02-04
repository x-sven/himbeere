#include "cParameter.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

cParameter::cParameter(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    sys_id = _sys_id;
    comp_id = _comp_id;
    value=_value;
    name=_name;
}

cParameter* cParameter::set(float _value, std::string _name)
{
    return set(_value, _name, 0, 0);
}

cParameter* cParameter::set(float _value, std::string _name, uint16_t _comp_id)
{
    return set(_value, _name, _comp_id, 0);
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

cParameter* cParameter::set(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    cParameter* p_new_parameter(new cParameter(_value, _name, _comp_id, _sys_id));
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
    }
    return p_new_parameter;
}

cParameter::~cParameter()
{
    //get_instances()->erase(this);
}
