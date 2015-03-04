#ifndef I_CTRL_H_INCLUDED
#define I_CTRL_H_INCLUDED

#include <boost/signals2.hpp>

class iCTRL
{
    public:
        iCTRL(){;};
        virtual ~iCTRL(){};

        virtual float getControl(float cmd, float state, float offset) = 0;

        boost::signals2::signal<void (void)>  signal_newdata;

    protected:
    private:
};


#endif // I_CTRL_H_INCLUDED
