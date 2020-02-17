//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2016-10-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// 2019-05-24 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#include <SPI.h>
#define EI_NOTEXTERNAL
#include <EnableInterrupt.h>
#include <AskSinPP.h>
#include <Device.h>
#include <Register.h>

#include "Ssd1306.h"  // comment out if you use a 0.91" 128x32 OLED display

#ifdef SSD1306_H_
#define PEAK_THRESHOLD 10 // count the RSSI Polls for the peak threshold
#endif

#define RSSI_POLL_INTERVAL 100 //milliseconds
#define RSSI_SERIAL_FACTOR 5   // sends all RSSI_POLL_INTERVAL * RSSI_SERIAL_FACTOR  RSSI to Serial

// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
  {0xFF, 0xFF, 0xFF},     // Device ID
  "..........",           // Device Serial
  {0x00, 0x00},           // Device Model
  0x10,                   // Firmware Version
  as::DeviceType::Remote, // Device Type
  {0x00, 0x00}            // Info Bytes
};

typedef AskSin<StatusLed<4>, NoBattery, Radio<LibSPI<10>, 2>> HalType;

class SnifferDevice : public Device<HalType, DefList0>, Alarm {
  DefList0 l0;
  
  private: 
    uint8_t cnt;
    uint16_t rssi_mean;
    uint8_t rssi_mean_val; 
  public:
    typedef Device<HalType, DefList0> BaseDevice;
#ifdef SSD1306_H_
    DisplayType<PEAK_THRESHOLD, 0x3C> display;
#endif
    SnifferDevice (const DeviceInfo& i, uint16_t addr) : BaseDevice(i, addr, l0, 0), Alarm(0), l0(addr)  {}
    virtual ~SnifferDevice () {}

    virtual void trigger (__attribute__ ((unused)) AlarmClock& clock) {
      set(millis2ticks(RSSI_POLL_INTERVAL));
      clock.add(*this);
      this->radio().pollRSSI();
      cnt++;
      rssi_mean += this->radio().rssi();
      if (cnt >= RSSI_SERIAL_FACTOR) {
        rssi_mean_val = rssi_mean / cnt;
        DPRINT(":");DHEX(rssi_mean_val);DPRINTLN(";"); 
        cnt = 0;
        rssi_mean = 0;
      }
#ifdef SSD1306_H_
      display.printFull(this->radio().rssi());
#endif
    }

    virtual bool process(Message& msg) {
      DPRINT(F(":"));
      DHEX(radio().rssi());
      DHEX(msg.length());
      DHEX(msg.count());
      DHEX(msg.flags());
      DHEX(msg.type());
      msg.from().dump();
      msg.to().dump();
      for (uint8_t l = 0; l < msg.length() - 9; l++) DHEX(msg.buffer()[l + 9]);
      DPRINTLN(";");
      this->led().ledOn(millis2ticks(100));
      return true;
    }

    bool init (HalType& hal) {
      HMID id;
      this->setHal(hal);
      this->getDeviceID(id);
      hal.init(id);
      hal.config(this->getConfigArea());
      sysclock.add(*this);
      return false;
    }
};

HalType hal;
SnifferDevice sdev(devinfo, 0x20);

void setup () {
  DINIT(57600, ASKSIN_PLUS_PLUS_IDENTIFIER);
#ifdef SSD1306_H_
  sdev.display.init();
#endif
  sdev.init(hal);
}

void loop() {
  hal.runready();
  sdev.pollRadio();
}