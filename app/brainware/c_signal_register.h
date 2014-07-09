#ifndef C_SIGNAL_REGISTER_H
#define C_SIGNAL_REGISTER_H

#include<boost/signal.hpp>

class c_signal_register
{
    public:
        /** Default constructor */
        c_signal_register();

        void register_func(boost::signal<void (void)> source, boost::function<void (void)> target);
        /** Default destructor */
        virtual ~c_signal_register();
    protected:
    private:
};

#endif // C_SIGNAL_REGISTER_H
