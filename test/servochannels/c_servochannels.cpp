#include "c_servochannels.h"

cServoChannels::cServoChannels(size_t num):
    ppm_min(900),
    ppm_max(2100)
{
    p_channel = NULL;
    if(0 < num)
    {
        p_channel = (unsigned int*)malloc(num*sizeof(unsigned int));
        if(NULL != p_channel)
        {
            for(size_t ii=0; ii < num; ii++)
                p_channel[ii] = 0;
            m_size = num;
        }
        else
        {
            m_size = 0;
        }
    }
}
cServoChannels & cServoChannels::operator=(cServoChannels const &other)
{
    if(NULL != p_channel)
        free(p_channel);
    m_size = other.m_size;
    p_channel = (unsigned int *)malloc(m_size*sizeof(unsigned int));
    memcpy(p_channel, other.p_channel, m_size*sizeof(unsigned int));
    return *this;
}

unsigned int & cServoChannels::operator[](size_t const &index)
{
    return(p_channel[index]);
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
    if(NULL != p_channel)
        free(p_channel);

    p_channel = NULL;
}
