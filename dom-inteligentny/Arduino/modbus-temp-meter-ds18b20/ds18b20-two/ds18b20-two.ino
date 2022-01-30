#include <OneWire.h>
#include <DS18B20.h>
#include <EEPROM.h>
#include <ModbusRtu.h>
#define ID   15
// Numer pinu cyfrowego do którego podłaczyłęś czujniki
const byte ONEWIRE_PIN = 5;
const byte ONEWIRE_PIN2 = 8;

OneWire onewire(ONEWIRE_PIN);
OneWire onewire2(ONEWIRE_PIN2);
DS18B20 sensor(&onewire);
DS18B20 sensor2(&onewire2);

Modbus slave;
byte sensora1[8];
byte sensora2[8];

void setup()
{
  pinMode(7, INPUT_PULLUP);
  delay(100);
  discover();
  slave = Modbus(ID, Serial, 2);
  slave.begin(9600);
  sensor.begin();
  sensor.request();
  sensor2.begin();
  sensor2.request();
}

uint16_t au16data[3];

void discover()
{
  byte address[8];
  onewire.reset_search();
  while(onewire.search(address))
  {
    if (address[0] != 0x28)
      continue;

    if (OneWire::crc8(address, 7) != address[7])
    {
      break;
    }

    for (byte i=0; i<8; i++)
    {
      sensora1[i] = address[i];
    }
  }
  onewire2.reset_search();
  while(onewire2.search(address))
  {
    if (address[0] != 0x28)
      continue;

    if (OneWire::crc8(address, 7) != address[7])
    {
      break;
    }

    for (byte i=0; i<8; i++)
    {
      sensora2[i] = address[i];
    }
  }
}

float temperature1 = 100;
float temperature2 = 100;

void getTemperature()
{
  if (sensor.available())
  {
    temperature1 = sensor.readTemperature(sensora1);
  }
  if (sensor2.available())
  {
    temperature2 = sensor2.readTemperature(sensora2);
  }

  sensor.request();
  sensor2.request();
}

static const unsigned long REFRESH_INTERVAL = 1000; // ms
static unsigned long lastRefreshTime = 0;
int8_t state = 0;
  
void loop()
{
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
    getTemperature();
  }

  au16data[0] = round(temperature1*100);
  au16data[1] = round(temperature2*100);
  au16data[2] = slave.getInCnt();
  
  state = slave.poll( au16data, 3 );
}
