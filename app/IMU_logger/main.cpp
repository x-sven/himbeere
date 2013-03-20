#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

using namespace std;

#include <I2Cdev.h>
#include <MPU6050/MPU6050.h>
#include <HMC5883L/HMC5883L.h>
#include <MS561101/MS561101BA.h>

#include <Delay.h>

#define DEBUG(s) cout<<(s)

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69

MPU6050 imu(MPU6050_ADDRESS_AD0_HIGH);
HMC5883L mag(0x1E);
MS561101BA baro(0x77);

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
int32_t pressure, temp;

void setup() {
    // initialize device
    DEBUG("Initializing I2C devices...\n");

    imu.initialize();
    imu.setI2CBypassEnabled(true);
    imu.setI2CMasterModeEnabled(false);

    // master & bypass
    DEBUG("Testing device configuration...\n");
    if (1 == imu.getI2CMasterModeEnabled())
      DEBUG("MPU6050 master mode enabled\n");
    else
      DEBUG("MPU6050 master mode disabled\n");

    if (1 == imu.getI2CBypassEnabled())
      DEBUG("MPU6050 bypass enabled\n");
    else
      DEBUG("MPU6050 bypass disabled\n");

    mag.initialize();
    baro.initialize();

    // verify connection
    DEBUG("Testing device connections...\n");
    DEBUG(imu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");DEBUG("\n");
    DEBUG(baro.testConnection() ? "MS561101BA connection successful" : "MS561101BA connection failed");DEBUG("\n");
    DEBUG(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");DEBUG("\n");
}

void loop() {

    // read raw measurements from device
    mag.getHeading(&mx, &my, &mz);
    baro.read(&pressure, &temp);
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // display values
    DEBUG("a/g/m/p/T:\t");
    DEBUG(ax); DEBUG("\t");
    DEBUG(ay); DEBUG("\t");
    DEBUG(az); DEBUG("\t");
    DEBUG(gx); DEBUG("\t");
    DEBUG(gy); DEBUG("\t");
    DEBUG(gz); DEBUG("\t");
    DEBUG(mx); DEBUG("\t");
    DEBUG(my); DEBUG("\t");
    DEBUG(mz); DEBUG("\t");
    DEBUG((float)pressure/100.); DEBUG("\t");
    DEBUG((float)temp/100.); DEBUG("\t");
    DEBUG("\n");

    delay(100);

}

int main()
{
    setup();
    for (;;)
        loop();
}

