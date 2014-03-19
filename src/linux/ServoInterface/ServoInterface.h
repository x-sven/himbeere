#ifndef CSERVOINTERFACECLASS_H
#define CSERVOINTERFACECLASS_H

#include "Stream.h"

class ServoInterface
{
    public:
        ServoInterface(Stream *stream = NULL);
        void begin(Stream *stream);
        bool update();
        uint16_t get_channel(uint8_t channel);

        virtual ~ServoInterface();
        const uint8_t num_channel_max;
    protected:
    private:
        Stream *m_stream;
        uint8_t reciever_state;
        uint8_t reciever_couter;
        uint16_t *rx_channel;

// TODO (sven#1#): Failsafe values?

};

#endif // CSERVOINTERFACECLASS_H
