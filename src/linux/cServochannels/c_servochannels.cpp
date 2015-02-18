#include "c_servochannels.h"

cServoChannels::cServoChannels(size_t num):
    ppm_min(900),
    ppm_max(2100)
{
    p_channel = new uint16_t[num];
    for(size_t ii=0; ii<num; ii++)
        p_channel[ii] = 0;
    m_size = num;
}

cServoChannels & cServoChannels::operator=(cServoChannels const &other)
{
    if (this != &other)
    {
        delete [] p_channel;
        m_size = other.m_size;
        p_channel = new uint16_t[m_size];
        memcpy(p_channel, other.p_channel, m_size*sizeof(uint16_t));
    }
    return *this;
}

void cServoChannels::setChannel(size_t no, uint16_t value)
{
    p_channel[no] = value;
}

uint16_t cServoChannels::getChannel(size_t no)
{
    return(p_channel[no]);
}


boolean cServoChannels::valid()
{
    uint8_t valid = 0;
    for(uint8_t ii=0; ii<m_size; ii++)
    {
        if(    (p_channel[ii] < ppm_min)
                || (p_channel[ii] > ppm_max))
            valid = valid || 1;
    }
    if(0 == valid)
        return true;
    else
        return false;
}

size_t cServoChannels::size(void)
{
    return(m_size);
}

cServoChannels::~cServoChannels()
{
    m_size = 0;
    delete [] p_channel;
}
