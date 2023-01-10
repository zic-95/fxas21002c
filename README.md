# FXAS21002C Driver
Driver for NXP FXAS21002C Gyroscope. Tested on Teensy 4.0 board and Adafruit breakout  board (https://learn.adafruit.com/nxp-precision-9dof-breakout)


To use the FXAS21002C driver, initialize it (it uses the default I2C bus, but another bus can be forwarded at initialization):
```c++
    fxas21002c_gyro sensor(FXAS2002C_DEFAUTLT_I2C_ADDR);
    bool init(TwoWire &inBus = Wire);
```

FXAS21002C has a lot of options that can be setup, like high pass and low pass filter cut-off frequency. Output data rate and Full-scale range can be setup as well. Before changing these parameters, the sensor needs to be in Ready or Standby working mode. That part is handled by the library.
```c++
    bool setLPF_CF(FXAS21002C_gyro_lpf_cf setLPF_CF);
    bool setHPF_CF(FXAS21002C_gyro_hpf_cf setHPF_CF);
    // etc...
```

To read temperature, use **readTemp()**, it reads from temperature from sensor, saves it to driver internal variable, and fills  retTemp variable that is passed to **readTemp()** by reference. To get last saved internal value for temperautre, use **getTemp()**.
```c++
    bool readTemp(int8_t *retTemp);
    int8_t getTemp(void);
```

To read gyroscope data, use **readGyroData()**, it reads from gyro from sensor, saves it to driver internal struct, and fills  gyro_ret_data struct that is passed to **readGyroData()** by reference. To get last saved internal value for temperautre, use **getGyroData()**.
```c++
    bool readGyroData(FXAS21002C_gyro_data *gyro_ret_data);
    FXAS21002C_gyro_data getGyroData(void);
```

There is one short example on how to init sensor and read Temperature as well as Gyroscope data.
