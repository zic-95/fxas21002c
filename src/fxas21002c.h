#ifndef FXAS21002C_H
#define FXAS21002C_H

#include <Arduino.h>
#include <Wire.h>
#include "fxas21002c_reg.h"

#define DEBUG 0

class fxas21002c_gyro
{
public:
    fxas21002c_gyro(uint8_t address = FXAS2002C_DEFAUTLT_I2C_ADDR);
    ~fxas21002c_gyro();
    bool init(TwoWire &inBus = Wire);

    bool readTemp(int8_t *retTemp);
    int8_t getTemp(void);

    bool readGyroData(FXAS21002C_gyro_data *gyro_ret_data);
    FXAS21002C_gyro_data getGyroData(void);
    // bool getXYZ;;;;

    bool getStatus(FXAS21002C_dr_status *STATUS);
    bool whoAmI(void);
    bool softReset(void);
    bool enableSelfTest(void);
    bool setWorkingMode(FXAS21002C_working_mode setWM); // Set Gyro Working mode
    FXAS21002C_working_mode getWorkingMode();
    bool setODR(FXAS21002C_gyro_odr setWantedODR); // Set ODR (data output rate)
    FXAS21002C_gyro_odr getCurrentODR();
    bool wraptoone(bool autoIncrementOn); // Set auto-increment read address pointer roll-over behaviour
    bool isWrapToOneOn();
    bool setFStoDouble(bool toDoubleFS); // Full-scale range expansion enable
    bool isSetFStoDouble();
    bool setLPF_CF(FXAS21002C_gyro_lpf_cf setLPF_CF); // Set cut-off freq of DLPF
    FXAS21002C_gyro_lpf_cf get_lpf_cf();
    bool setHPF_CF(FXAS21002C_gyro_hpf_cf setHPF_CF); // Set HPF cutt-off freq
    FXAS21002C_gyro_hpf_cf get_hpf_cf();
    bool enableHPF(bool enableHPF); // Enable HPF (default disabled)
    bool isHPFenabled();
    bool setFSR(FXAS21002C_gyro_fsr setWantedFSR); // Set FSR (full scale range)
    FXAS21002C_gyro_fsr getCurrentFSR();
    float setFSR_factor(FXAS21002C_gyro_fsr setWantedFSR); // Set FSR factor

    void setBus(TwoWire *inBus) { this->_i2c_bus = inBus; }
    TwoWire *getBus() { return _i2c_bus; }

private:
    TwoWire *_i2c_bus;

    bool read8(uint8_t reg, uint8_t *retVal);
    bool readN(uint8_t reg, uint8_t nBytes, uint8_t *retBuff);
    bool write8(uint8_t reg, uint8_t data);

    uint8_t _address;
    int8_t _temperature = 0;
    FXAS21002C_gyro_data _gyro_data = {0.0};

    FXAS21002C_working_mode currWorkingMode;
    FXAS21002C_gyro_odr currODR;
    FXAS21002C_gyro_fsr currFSR;
    float currFSR_sens = 0.0;

    FXAS21002C_gyro_lpf_cf currLPF_CF;
    FXAS21002C_gyro_hpf_cf currHPF_CF;
    uint8_t currWrapToOne;  // Default The auto-increment read address pointer rolls over to address 0x00 (STATUS) after the Z-axis LSB is read
    uint8_t currFSdouble;   // Default not in full scale double mode
    uint8_t currHPF_Enable; // Defualt HPF disabled
};
#endif