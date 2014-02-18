#ifndef C_JOYSTICKMANAGER_HPP
#define C_JOYSTICKMANAGER_HPP

#include "c_joystick.hpp"

class cJoystickManager
{
    public:
        /** Default constructor */
        cJoystickManager();
        /** Default destructor */
        virtual ~cJoystickManager();

        float getRollValue();
        float getPitchValue();
        float getYawValue();
        float getColValue();

        float getModeValue();

        void setRollProberty(uint8_t ch, float scale, float upper, float offset, float lower);
        void setPitchProberty(uint8_t ch, float scale, float upper, float offset, float lower);
        void setYawProberty(uint8_t ch, float scale, float upper, float offset, float lower);
        void setColProberty(uint8_t ch, float scale, float upper, float offset, float lower);
        void setModeProberty(uint8_t ch, float scale, float upper, float offset, float lower);

        uint8_t getRollChannel(void);
        uint8_t getPitchChannel(void);
        uint8_t getYawChannel(void);
        uint8_t getColChannel(void);
        uint8_t getModeChannel(void);

        float getRollScale(void);
        float getPitchScale(void);
        float getYawScale(void);
        float getColScale(void);
        float getModeScale(void);

        float getRollUpperLimit(void);
        float getPitchUpperLimit(void);
        float getYawUpperLimit(void);
        float getColUpperLimit(void);
        float getModeUpperLimit(void);

        float getRollOffset(void);
        float getPitchOffset(void);
        float getYawOffset(void);
        float getColOffset(void);
        float getModeOffset(void);

        float getRollLowerLimit(void);
        float getPitchLowerLimit(void);
        float getYawLowerLimit(void);
        float getColLowerLimit(void);
        float getModeLowerLimit(void);

    protected:
    private:
        cJoystick* joy;

};

#endif // C_JOYSTICKMANAGER_HPP
