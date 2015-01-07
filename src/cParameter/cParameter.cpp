#include "cParameter.h"

cParameter::cParameter(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{
    sys_id = _sys_id;
    comp_id = _comp_id;
    value=_value;
    name=_name;
    get_instances()->insert(std::make_pair(name,this));
    //ctor
}

cParameter* cParameter::set(float _value, std::string _name)
{
    return set(_value, _name, 0, 0);
}

cParameter* cParameter::set(float _value, std::string _name, uint16_t _comp_id)
{
    return set(_value, _name, _comp_id, 0);
}

cParameter* cParameter::set(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id)
{

    cParameter* new_parameter(new cParameter(_value, _name, _comp_id, _sys_id));
    {
        std::pair<std::map<std::string,cParameter*>::iterator,bool> ret;
        ret = get_instances()->insert(std::pair<std::string, cParameter*>(new_parameter->get_name(), new_parameter));
        if(false == ret.second)
        {
            get_instances()->erase(ret.first);
            get_instances()->insert(std::pair<std::string, cParameter*>(new_parameter->get_name(), new_parameter));
        }
    }
    return new_parameter;
}

cParameter::~cParameter()
{
    get_instances()->erase(this->get_name());
}
