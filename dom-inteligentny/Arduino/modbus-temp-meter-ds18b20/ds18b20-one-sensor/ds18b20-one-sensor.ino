#include <OneWire.h>
#include <DS18B20.h>
#include <ModbusRtu.h>

#define ID   101
// Numer pinu cyfrowego do którego podłaczyłęś czujniki
const byte ONEWIRE_PIN = 8;

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensor(&onewire);

Modbus slave;
byte sensora1[8];

void setup()
{
  pinMode(7, INPUT_PULLUP);
  delay(100);
  discover();
  slave = Modbus(ID, Serial, 2);
  slave.begin(9600);
  sensor.begin();
  sensor.request();
}

uint16_t au16data[2];

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
}

float temperature1 = 100;


void getTemperature()
{
  if (sensor.available())
  {
    temperature1 = sensor.readTemperature(sensora1);
  }

  sensor.request();
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

    au16data[0] = round(temperature1*100);
    au16data[1] = slave.getInCnt();

  }
  state = slave.poll( au16data, 2 );
 
}
