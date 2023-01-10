#ifndef FXAS21002C_REG_H
#define FXAS21002C_REG_H

#define FXAS2002C_DEFAUTLT_I2C_ADDR 0x21 // with SA0 = 0, or 0x21 with SA0 = 1
#define FXAS2002C_WHO_AM_I_ID 0xD7

// Gyro  msr sensitivity settings
#define GYRO_SENS_250 (0.0078125F) // 250dps sensitivity
#define GYRO_SENS_500 (0.015625F)  // 500dps sensitivity
#define GYRO_SENS_1000 (0.03125F)  // 1000dps sensitivity
#define GYRO_SENS_2000 (0.0625F)   // 2000dps sensitivity

typedef struct
{
    float x;
    float y;
    float z;
}FXAS21002C_gyro_data;

// Gyro FS factor:
typedef enum
{
    GYRO_FSR_250DPS = 0x00,  // 250dps range
    GYRO_FSR_500DPS = 0x01,  // 500dps range
    GYRO_FSR_1000DPS = 0x02, // 1000dps range
    GYRO_FSR_2000DPS = 0x03, // 2000dps range
} FXAS21002C_gyro_fsr;

// Gyro ODR:
typedef enum
{
    GYRO_ODR_12_5HZ = 0x18, // 12.5Hz ODR
    GYRO_ODR_25HZ = 0x14,   // 25Hz ODR
    GYRO_ODR_50HZ = 0x10,   // 50Hz ODR
    GYRO_ODR_100HZ = 0x0c,  // 100Hz ODR
    GYRO_ODR_200HZ = 0x08,  // 200Hz ODR
    GYRO_ODR_400HZ = 0x04,  // 400Hz ODR
    GYRO_ODR_800HZ = 0x00,  // 800Hz ODR

} FXAS21002C_gyro_odr;

// Gyro LPF Cut-Off frequency:
typedef enum
{                         // 800 400 200 100  50 25 12.5    HZ
    GYRO_LPF_CF_1 = 0x00, // 256 128  64  32  16  8    4
    GYRO_LPF_CF_2 = 0x01, // 128  64  32  16   8  4    —
    GYRO_LPF_CF_3 = 0x10, //  64  32  16   8   4  —    —
} FXAS21002C_gyro_lpf_cf;

// HPF HPF Cut-Off frequency:
typedef enum
{                         // 800    400   200    100    50     25     12.5    HZ
    GYRO_HPF_CF_1 = 0x00, // 15     7.5   3.75   1.875  0.937  0.468  0.234
    GYRO_HPF_CF_2 = 0x08, //  7.7   3.85  1.925  0.963  0.481  0.241  0.120
    GYRO_HPF_CF_3 = 0x10, //  3.9   1.95  0.975  0.488  0.244  0.122  0.061
    GYRO_HPF_CF_4 = 0x18, //  1.98  0.99  0.495  0.248  0.124  0.062  0.031
} FXAS21002C_gyro_hpf_cf;

// Device registers:
typedef enum
{
    REG_STATUS = 0x00,
    REG_OUT_X_MSB = 0x01,
    REG_OUT_X_LSB = 0x02,
    REG_OUT_Y_MSB = 0x03,
    REG_OUT_Y_LSB = 0x04,
    REG_OUT_Z_MSB = 0x05,
    REG_OUT_Z_LSB = 0x06,
    REG_DR_STATUS = 0x07,
    REG_F_STATUS = 0x08,
    REG_F_SETUP = 0x09,
    REG_F_EVENT = 0x0A,
    REG_INT_SOURCE_FLAG = 0x0B,
    REG_WHO_AM_I = 0x0C,
    REG_CTRL_REG0 = 0x0D,
    REG_RT_CFG = 0x0E,
    REG_RT_SRC = 0x0F,
    REG_RT_THS = 0x10,
    REG_RT_COUNT = 0x11,
    REG_TEMP = 0x12,
    REG_CTRL_REG1 = 0x13,
    REG_CTRL_REG2 = 0x14,
    REG_CTRL_REG3 = 0x15,

} FXAS21002C_regAddr;

typedef enum
{
    X_AXIS_DATA_READY = 0,       // Cleared anytime the OUT_X_MSB register is read
    Y_AXIS_DATA_READY = 1,       // Cleared anytime the OUT_Y_MSB register is read
    Z_AXIS_DATA_READY = 2,       // Cleared anytime the OUT_Z_MSB register is read
    XYZ_AXIS_DATA_READY = 3,     // Cleared when the high-bytes of the data of all channels (OUT_X_MSB, OUT_Y_MSB, OUT_Z_MSB) are read
    X_AXIS_DATA_OVERWRITE = 4,   // Cleared anytime the OUT_Z_MSB (and respectively OUT_Y_MSB, OUT_X_MSB) register is read
    Y_AXIS_DATA_OVERWRITE = 5,   // Cleared anytime the OUT_Z_MSB (and respectively OUT_Y_MSB, OUT_X_MSB) register is read
    Z_AXIS_DATA_OVERWRITE = 6,   // Cleared anytime the OUT_Z_MSB (and respectively OUT_Y_MSB, OUT_X_MSB) register is read
    XYZ_AXIS_DATA_OVERWRITE = 7, // Cleared after the high-bytes of the data of all channels (OUT_X_MSB, OUT_Y_MSB, OUT_Z_MSB) are read
} FXAS21002C_dr_status;

typedef enum
{
    STANDBY_WORKING_MODE = 0x00, // Default at power-up
    READY_WORKING_MODE = 0x01,
    ACTIVE_WORKING_MODE = 0x02, // Higher priority Ready working mode
} FXAS21002C_working_mode;
#endif