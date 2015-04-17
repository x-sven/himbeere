#ifndef CPARAMETER_H
#define CPARAMETER_H

#include <stdint.h>
#include <string>
#include <vector>




class cParameter
{
    public:
        uint16_t get_sys_id()   {return sys_id;};
        uint16_t get_comp_id()  {return comp_id;};
        std::string get_name()  {return name;};
        float*    get_value()       {return value;};
        void     set_value(float _value) {(*value)=_value;}; // set_value for using the dataspace


//   void register_comp_callback(uint16_t comp_id, void* func) {
//
//
//   }
//   void    update_comp(uint16_t comp_id)
//   {
//       {
//           list <pair<uint16_t, void*> >
//
//
//
//           suche in liste comp id
//           rufe callback auf, wenn nicht nullptr
//       }
//
//   };

//Haben _value auf typ Pointer geändert, um die Member von den Modulen direkt zu ändern.
//Zugriffschutz? Keine Ahnung -> überbewertet, löst simon oder cgp, wenn es nötig wird.
//Brauchen Methoden, um an die Adressen der Parameter-Member zu kommen...

        static std::vector<cParameter*>::iterator exist(std::string);
        static cParameter* set(float _value, std::string _name,float* _parameter_ptr, uint16_t _comp_id, uint16_t _sys_id);
        static cParameter* set(float _value, std::string _name,float* _parameter_ptr, uint16_t _comp_id);
        static cParameter* set(float _value, std::string _name,float* _parameter_ptr);



        static std::vector<cParameter*>* get_instances()
        {
            static std::vector<cParameter*> list_parameter_instances;
            return(&list_parameter_instances);
        }


    protected:
    private:
        cParameter(float* _parameter_ptr, std::string _name, uint16_t _comp_id, uint16_t _sys_id);
        virtual ~cParameter();
        uint16_t sys_id;
        uint16_t comp_id;
        float* value;
        std::string name;
};




#endif // CPARAMETER_H
