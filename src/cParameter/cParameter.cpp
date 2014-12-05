#include "cParameter.h"

cParameter::cParameter(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    sys_id = _sys_id;
    comp_id = _comp_id;
    value=_value;
    name=_name;
    get_instances()->insert(this);
    //ctor
}

cParameter* cParameter::create_instance(float _value, std::string _name)
{
    return create_instance(_value, _name, 0, 0);
}

cParameter* cParameter::create_instance(float _value, std::string _name, uint16_t _comp_id)
{
    return create_instance(_value, _name, _comp_id, 0);
}

cParameter* cParameter::create_instance(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    cParameter* new_parameter(new cParameter(_value, _name, _comp_id, _sys_id));
    {
        get_instances()->insert(new_parameter);
    }
    return new_parameter;
}

cParameter::~cParameter()
{
    get_instances()->erase(this);
}
