#ifndef _ST_EULER_H_
#define _ST_EULER_H_

struct stEuler_t
{
	float roll, pitch, yaw;
	stEuler_t(void) : roll(0.), pitch(0.), yaw(0.) {};

	void reset(void)
	{
		roll = pitch = yaw = 0.;
	};

	stEuler_t toDeg( void)
	{
	    stEuler_t Euler;
	    Euler.pitch = this->pitch*57.2957795131; //*180/pi
	    Euler.roll = this->roll*57.2957795131;
	    Euler.yaw = this->yaw*57.2957795131;
		return(Euler);
	};
} ;

#endif //_ST_EULER_H_
