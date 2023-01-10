#include "fxas21002c.h"

fxas21002c_gyro::fxas21002c_gyro(uint8_t address) : _address(address)
{
    currWorkingMode = STANDBY_WORKING_MODE;
    currODR = GYRO_ODR_100HZ;
    currFSR = GYRO_FSR_500DPS;
    currFSR_sens = GYRO_SENS_500;
    currLPF_CF = GYRO_LPF_CF_1;
    currHPF_CF = GYRO_HPF_CF_1;
    currWrapToOne = 0x00;
    currFSdouble = 0x00;
    currHPF_Enable = 0x00;
}

fxas21002c_gyro::~fxas21002c_gyro()
{
}

bool fxas21002c_gyro::init(TwoWire &inBus)
{
    _i2c_bus = &inBus;
    /*
    ToDo:
    - setup LPF
    - setup HPF
    - enable HPF
    - set WrapToOne
    */
    if (!whoAmI())
        return false;
    if (!setWorkingMode(READY_WORKING_MODE))
        return false;
    if (!setODR(GYRO_ODR_100HZ))
        return false;
    if (!setFSR(GYRO_FSR_500DPS))
        return false;
    if (!setWorkingMode(ACTIVE_WORKING_MODE))
        return false;
    return true;
}

// The content of this register is reset upon a transition from Standby to Active or from Ready to Active modes
bool fxas21002c_gyro::getStatus(FXAS21002C_dr_status *STATUS)
{
    if (!read8(REG_DR_STATUS, (uint8_t *)STATUS))
        return false;
    else
    {
        return true;
#if DEBUG == 1
        if (((*STATUS) & 0x01) == X_AXIS_DATA_READY)
            Serial.println("X_AXIS_DATA_READY");
        if (((*STATUS) & 0x02) == Y_AXIS_DATA_READY)
            Serial.println("Y_AXIS_DATA_READY");
        if (((*STATUS) & 0x04) == Z_AXIS_DATA_READY)
            Serial.println("Z_AXIS_DATA_READY");
        if (((*STATUS) & 0x08) == XYZ_AXIS_DATA_READY)
            Serial.println("XYZ_AXIS_DATA_READY");
        if (((*STATUS) & 0x10) == X_AXIS_DATA_OVERWRITE)
            Serial.println("X_AXIS_DATA_OVERWRITE");
        if (((*STATUS) & 0x20) == Y_AXIS_DATA_OVERWRITE)
            Serial.println("Y_AXIS_DATA_OVERWRITE");
        if (((*STATUS) & 0x40) == Z_AXIS_DATA_OVERWRITE)
            Serial.println("Z_AXIS_DATA_OVERWRITE");
        if (((*STATUS) & 0x80) == XYZ_AXIS_DATA_OVERWRITE)
            Serial.println("XYZ_AXIS_DATA_OVERWRITE");
#endif
    }
}

// Reads current temperature to internal variable and sends the same data to the caller
bool fxas21002c_gyro::readTemp(int8_t *retTemp)
{
    uint8_t retVal;
    if (!read8(REG_TEMP, &retVal))
        return false;
    else
    {
        *retTemp = (int8_t)retVal;
        _temperature = *retTemp;
        return true;
    }
}

// Returns current temperature value that is in the internal variable
int8_t fxas21002c_gyro::getTemp(void)
{
    return _temperature;
}

// Reads current Gyro data to internal structure and sends the same data to the caller
bool fxas21002c_gyro::readGyroData(FXAS21002C_gyro_data *gyro_ret_data)
{
    uint8_t retBuffer[6];
    if (!readN(REG_OUT_X_MSB, 6, retBuffer))
        return false;
    else
    {
        _gyro_data.x = (float)((int16_t)(((int16_t)retBuffer[0]) << 8 | ((int16_t)retBuffer[1]))) * currFSR_sens;
        _gyro_data.y = (float)((int16_t)(((int16_t)retBuffer[2]) << 8 | ((int16_t)retBuffer[3]))) * currFSR_sens;
        _gyro_data.z = (float)((int16_t)(((int16_t)retBuffer[4]) << 8 | ((int16_t)retBuffer[5]))) * currFSR_sens;
        if (gyro_ret_data != nullptr)
        {
            gyro_ret_data->x = _gyro_data.x;
            gyro_ret_data->y = _gyro_data.y;
            gyro_ret_data->z = _gyro_data.z;
        }
        return true;
    }
}

// Returns current raw Gyro data that is in the internal struct, copy format of struct, better to avoid
FXAS21002C_gyro_data fxas21002c_gyro::getGyroData(void)
{
    return _gyro_data;
}

bool fxas21002c_gyro::whoAmI(void)
{
    uint8_t retWhoAmI = 11;
    if (!read8(REG_WHO_AM_I, &retWhoAmI))
        return false;
    else
    {
        if (retWhoAmI == FXAS2002C_WHO_AM_I_ID)
            return true;
        else
            return false;
    }
}

// On reset, all registers except 0x01 through 0x06 revert to their default values
bool fxas21002c_gyro::softReset(void)
{
    if (!write8(REG_CTRL_REG1, 0x40))
        return false;
    else
        return true;
}

/*
When ST is set, a data output change will occur even if no angular rate is applied.
Enable it, than read data from xyz output registers
*/
bool fxas21002c_gyro::enableSelfTest(void)
{
    if (!write8(REG_CTRL_REG1, (0x20 | currODR | currWorkingMode))) // We must mask ODR and working mode registers with test enable
        return false;
    else
        return true;
}

bool fxas21002c_gyro::setWorkingMode(FXAS21002C_working_mode setWM)
{
    if (!write8(REG_CTRL_REG1, setWM | currODR)) // We must mask ODR and working mode registers
        return false;
    else
    {
        currWorkingMode = setWM;
        return true;
    }
}

FXAS21002C_working_mode fxas21002c_gyro::getWorkingMode(void)
{
    return currWorkingMode;
}

bool fxas21002c_gyro::setODR(FXAS21002C_gyro_odr setWantedODR)
{
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG1, setWantedODR | currWorkingMode)) // We must mask ODR and working mode registers
                return false;
            else
            {
                currODR = setWantedODR;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG1, setWantedODR | currWorkingMode)) // We must mask ODR and working mode registers
            return false;
        else
        {
            currODR = setWantedODR;
            return true;
        }
    }
}

FXAS21002C_gyro_odr fxas21002c_gyro::getCurrentODR()
{
    return currODR;
}

bool fxas21002c_gyro::wraptoone(bool autoIncrementOn)
{
    uint8_t setVar;
    if (autoIncrementOn)
        setVar = 0x08;
    else
        setVar = 0x00;
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG3, setVar | currFSdouble)) // We must mask WRAPTOONE and FS_DOUBLE mode registers
                return false;
            else
            {
                currWrapToOne = setVar;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG3, setVar | currFSdouble)) // We must mask WRAPTOONE and FS_DOUBLE mode registers
            return false;
        else
        {
            currWrapToOne = setVar;
            return true;
        }
    }
}

bool fxas21002c_gyro::isWrapToOneOn()
{
    if (currWrapToOne == 0x08)
        return true;
    else
        return false;
}
bool fxas21002c_gyro::setFStoDouble(bool toDoubleFS)
{
    uint8_t setVar;
    if (toDoubleFS)
        setVar = 0x01;
    else
        setVar = 0x00;
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG3, setVar | currWrapToOne)) // We must mask WRAPTOONE and FS_DOUBLE mode registers
                return false;
            else
            {
                currFSdouble = setVar;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG3, setVar | currWrapToOne)) // We must mask WRAPTOONE and FS_DOUBLE mode registers
            return false;
        else
        {
            currFSdouble = setVar;
            return true;
        }
    }
}

bool fxas21002c_gyro::isSetFStoDouble()
{
    if (currFSdouble == 0x01)
        return true;
    else
        return false;
}

bool fxas21002c_gyro::setLPF_CF(FXAS21002C_gyro_lpf_cf setLPF_CF)
{
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG0, setLPF_CF | currHPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
                return false;
            else
            {
                currLPF_CF = setLPF_CF;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG0, setLPF_CF | currHPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
            return false;
        else
        {
            currLPF_CF = setLPF_CF;
            return true;
        }
    }
}

FXAS21002C_gyro_lpf_cf fxas21002c_gyro::get_lpf_cf()
{
    return currLPF_CF;
}

bool fxas21002c_gyro::setHPF_CF(FXAS21002C_gyro_hpf_cf setHPF_CF)
{
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG0, setHPF_CF | currLPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
                return false;
            else
            {
                currHPF_CF = setHPF_CF;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG0, setHPF_CF | currLPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
            return false;
        else
        {
            currHPF_CF = setHPF_CF;
            return true;
        }
    }
}

FXAS21002C_gyro_hpf_cf fxas21002c_gyro::get_hpf_cf()
{
    return currHPF_CF;
}

bool fxas21002c_gyro::enableHPF(bool enableHPF)
{
    uint8_t setVar;
    if (enableHPF)
        setVar = 0x04;
    else
        setVar = 0x00;
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG0, setVar | currLPF_CF | currHPF_CF | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
                return false;
            else
            {
                currHPF_Enable = setVar;
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG0, setVar | currLPF_CF | currHPF_CF | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
            return false;
        else
        {
            currHPF_Enable = setVar;
            return true;
        }
    }
}
bool fxas21002c_gyro::isHPFenabled()
{
    if (currHPF_Enable == 0x04)
        return true;
    else
        return false;
}

bool fxas21002c_gyro::setFSR(FXAS21002C_gyro_fsr setWantedFSR)
{
    // Check in what working mode is sensor
    if (currWorkingMode == ACTIVE_WORKING_MODE)
    {
        if (!setWorkingMode(READY_WORKING_MODE))
            return false;
        else
        {
            if (!write8(REG_CTRL_REG0, setWantedFSR | currLPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
                return false;
            else
            {
                currFSR = setWantedFSR;
                currFSR_sens = setFSR_factor(setWantedFSR);
                if (!setWorkingMode(ACTIVE_WORKING_MODE))
                    return false;
                else
                    return true;
            }
        }
    }
    else
    {
        if (!write8(REG_CTRL_REG0, setWantedFSR | currLPF_CF | currHPF_Enable | currFSR)) // We must mask LPF_CF, HPF_CF, HPF_CF_EN and FS control registers
            return false;
        else
        {
            currFSR = setWantedFSR;
            currFSR_sens = setFSR_factor(setWantedFSR);
            return true;
        }
    }
}

FXAS21002C_gyro_fsr fxas21002c_gyro::getCurrentFSR()
{
    return currFSR;
}

float fxas21002c_gyro::setFSR_factor(FXAS21002C_gyro_fsr setWantedFSR)
{
    switch (setWantedFSR)
    {
    case GYRO_FSR_250DPS:
        return GYRO_SENS_250;
        break;
    case GYRO_FSR_500DPS:
        return GYRO_SENS_500;
        break;
    case GYRO_FSR_1000DPS:
        return GYRO_SENS_1000;
        break;
    case GYRO_FSR_2000DPS:
        return GYRO_SENS_2000;
        break;
    default:
        return -1;
        break;
    }
}

bool fxas21002c_gyro::read8(uint8_t reg, uint8_t *retVal)
{
    _i2c_bus->beginTransmission(_address);
    _i2c_bus->write(reg);
    if (_i2c_bus->endTransmission(false) != 0)
        return false;
    else
    {
        _i2c_bus->requestFrom(_address, (uint8_t)1, true);
        *retVal = _i2c_bus->read();
        return true;
    }
}

bool fxas21002c_gyro::readN(uint8_t reg, uint8_t nBytes, uint8_t *retBuff)
{
    uint8_t i = 0;
    _i2c_bus->beginTransmission(_address);
    _i2c_bus->write(reg);
    if (_i2c_bus->endTransmission(false) != 0)
        return false;
    else
    {
        _i2c_bus->requestFrom(_address, (uint8_t)nBytes);
        while (_i2c_bus->available())
        {
            retBuff[i++] = _i2c_bus->read();
        }
        return true;
    }
}

bool fxas21002c_gyro::write8(uint8_t reg, uint8_t data)
{

    _i2c_bus->beginTransmission(_address);
    _i2c_bus->write(reg);
    _i2c_bus->write(data);

    if (_i2c_bus->endTransmission() != 0)
        return false;
    else
        return true;
}
