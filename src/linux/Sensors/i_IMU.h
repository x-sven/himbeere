#ifndef __IMU_H__
#define __IMU_H__


class iIMU
{
    public:
        iIMU();
        virtual ~iIMU();

        void getAccel_mss(float *ax, float *ay, float *az);
        void getRates_rads(float *ax, float *ay, float *az);

    protected:
    private:
};

#endif // __IMU_H__
