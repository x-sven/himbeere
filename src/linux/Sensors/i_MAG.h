#ifndef __MAG_H__
#define __MAG_H__

#include <boost/signals2.hpp>

class iMAG
{
    public:
        iMAG(){;};
        virtual ~iMAG(){};

        virtual void getField_Gauss(float *mx, float *my, float *mz) = 0;

        boost::signals2::signal<void (void)>  signal_magdata;

    protected:
    private:
};

#endif // __MAG_H__
