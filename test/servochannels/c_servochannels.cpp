#include "c_servochannels.h"

cServoChannels::cServoChannels(size_t size):
  ppm_min(900),
  ppm_max(2100)
{
  p_channel = NULL;
  if(0 < size)
  {
    p_channel = (unsigned int*)malloc(size*sizeof(unsigned int));
    m_size = size;
  }

}
cServoChannels & cServoChannels::operator=(cServoChannels const &other)
{ 
  free(p_channel); 
  m_size = other.m_size; 
  p_channel = (unsigned int *)malloc(m_size*sizeof(unsigned int)); 
  memcpy(p_channel, other.p_channel, m_size*sizeof(cServoChannels)); 
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
    if(   !(p_channel[ii] > ppm_min) 
       || !(p_channel[ii] < ppm_max))
      valid |= 1;
  }
  if(valid)
    return true;
  else
    return false;
}     
     
cServoChannels::~cServoChannels()
{
  if(NULL != p_channel)
    free(p_channel);
}
