#ifndef C_SIGNAL_REGISTER_H
#define C_SIGNAL_REGISTER_H

#include<boost/signals2.hpp>

class c_signal_register
{
    public:
        /** Default constructor */
        c_signal_register();

        void register_func(boost::signals2::signal<void (void)> source, boost::function<void (void)> target);
        /** Default destructor */
        virtual ~c_signal_register();
    protected:
    private:
};

#endif // C_SIGNAL_REGISTER_H
