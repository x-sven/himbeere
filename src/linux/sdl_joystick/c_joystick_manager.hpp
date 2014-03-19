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

        void setRollProberty (uint8_t axis, float scale, float upper, float offset, float lower, float failsafe);
        void setPitchProberty(uint8_t axis, float scale, float upper, float offset, float lower, float failsafe);
        void setYawProberty  (uint8_t axis, float scale, float upper, float offset, float lower, float failsafe);
        void setColProberty  (uint8_t axis, float scale, float upper, float offset, float lower, float failsafe);
        void setModeProberty (uint8_t axis, float scale, float upper, float offset, float lower, float failsafe);

        uint8_t getRollChannel(void);
        uint8_t getPitchChannel(void);
        uint8_t getYawChannel(void);
        uint8_t getColChannel(void);
        uint8_t getModeChannel(void);

//        float getRollScale(void);
//        float getPitchScale(void);
//        float getYawScale(void);
//        float getColScale(void);
//        float getModeScale(void);
//
//        float getRollUpperLimit(void);
//        float getPitchUpperLimit(void);
//        float getYawUpperLimit(void);
//        float getColUpperLimit(void);
//        float getModeUpperLimit(void);
//
//        float getRollOffset(void);
//        float getPitchOffset(void);
//        float getYawOffset(void);
//        float getColOffset(void);
//        float getModeOffset(void);
//
//        float getRollLowerLimit(void);
//        float getPitchLowerLimit(void);
//        float getYawLowerLimit(void);
//        float getColLowerLimit(void);
//        float getModeLowerLimit(void);

    protected:
    private:
        cJoystick* joy;

        enum eChannel {eRoll=0, ePitch, eYaw, eCol, eMode, eAux, _eCounter};

        struct m_proberties
        {
            uint8_t    axis;
            float      scale;
            float      upper;
            float      lower;
            float      offset;
            float      failsafe;
        };

        std::vector<m_proberties> m_channel;

        void setProberty(eChannel e_ch, uint8_t ch, float scale, float upper, float offset, float lower, float failsafe);
        float getScaledAxis(eChannel e_ch);

};

#endif // C_JOYSTICKMANAGER_HPP
