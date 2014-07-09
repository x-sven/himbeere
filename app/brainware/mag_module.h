#ifndef MAG_MODULE_H
#define MAG_MODULE_H

#include "Drotek10dof/Drotek10dof.h"
#include "Sensors/i_MAG.h"

class mag_module
{
    public:
        /** Default constructor */
        mag_module();
        /** Default destructor */
        virtual ~mag_module();
        iMAG* get_mag_ptr(void );
    protected:
    private:
};

#endif // MAG_MODULE_H
