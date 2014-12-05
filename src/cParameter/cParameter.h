#ifndef CPARAMETER_H
#define CPARAMETER_H

#include <stdint.h>
#include <string>
#include <set>




class cParameter
{
    public:
        uint16_t get_sys_id()   {return sys_id;};
        uint16_t get_comp_id()  {return comp_id;};
        std::string get_name()  {return name;};
        float get_value()       {return value;};
        void set_value(float _value) {value=_value;};
        static cParameter* create_instance(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id);
        static cParameter* create_instance(float _value, std::string _name, uint16_t _comp_id);
        static cParameter* create_instance(float _value, std::string _name);

        static std::set<cParameter*> *get_instances()
        {
            static std::set<cParameter*> list_parameter_instances;
            return(&list_parameter_instances);
        }


    protected:
    private:
        cParameter(float _value, std::string _name, uint16_t _comp_id, uint16_t _sys_id);
        virtual ~cParameter();
        uint16_t sys_id;
        uint16_t comp_id;
        float value;
        std::string name;


};




#endif // CPARAMETER_H
