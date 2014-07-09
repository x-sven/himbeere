#include "c_signal_register.h"

c_signal_register::c_signal_register()
{
    //ctor
}

void register_func(boost::signal<void (void)> source, boost::function<void (void)> target)
{
    source.connect(target );
}


c_signal_register::~c_signal_register()
{
    //dtor
}
