#include <Arduino.h>
#include "fxas21002c.h"

#define GYRO_READ_HZ 100 // Hz
#define TEMP_READ_HZ 1   // Hz

static uint32_t tTime[10];

fxas21002c_gyro sensor(FXAS2002C_DEFAUTLT_I2C_ADDR);
int8_t temp;
FXAS21002C_gyro_data gyro_data = {0.0};

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  while (!Serial)
    ;
  if (!sensor.init())
  {
    while (true)
    {
      Serial.println("Error in GYRO init!");
      delay(100);
    }
  }
  delay(200);
}
void loop()
{
  uint32_t t = millis();
  if ((t - tTime[0]) >= (1000 / TEMP_READ_HZ))
  {
    if (!sensor.readTemp(&temp))
      Serial.println("Error reading temp!");
    else
      Serial.println(temp);
    tTime[0] = t;
  }
  if ((t - tTime[1]) >= (1000 / GYRO_READ_HZ))
  {
    if (!sensor.readGyroData(&gyro_data))
    {
      Serial.println("Error reading Gyro Data!");
    }
    else
    {
      Serial.print(gyro_data.x);
      Serial.print("\t");
      Serial.print(gyro_data.y);
      Serial.print("\t");
      Serial.println(gyro_data.z);
    }
    tTime[1] = t;
  }
}