#ifndef SENSORFUSION_H
#define SENSORFUSION_H

#include "ECF.h"
#include "BackgroundAction.h"

class SensorFusion: public ECFClass, public BackgroundAction
{
    public:
        SensorFusion();
        virtual ~SensorFusion();

        void update(void);
    protected:
    private:
};

#endif // SENSORFUSION_H
