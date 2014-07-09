#ifndef IBARO_H
#define IBARO_H

#include <boost/signal.hpp>

class iBARO
{
    public:
        iBARO(){;};
        virtual ~iBARO(){};

        virtual void getPressure_pa(float *p) = 0;
        virtual void getTemperature_deg(float *T) = 0;

        boost::signal<void (void)>  signal_barodata;

    protected:
    private:
};

#endif // IBARO_H
