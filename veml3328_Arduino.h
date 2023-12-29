#ifndef __VEML3328_H__
#define __VEML3328_H__

#include <Arduino.h>
#include <Wire.h>

// https://www.vishay.com/docs/84968/veml3328.pdf

#define VEML3328_DEF_ADDR 0x10

class VEML3328 {
private:
    uint8_t device_addr; // i2c address of the device
    TwoWire* wire;   // i2c interface
public:

    VEML3328();

    // returns same errors as wakeUp() function
    uint8_t begin(TwoWire* wire = &Wire, const uint8_t device_addr = VEML3328_DEF_ADDR);

    // returns same errors as WireUtils::write16Confirm
    uint8_t wakeUp(void);
    // returns same errors as WireUtils::write16Confirm
    uint8_t shutdown(void);

    // returns same errors as WireUtils::read16
    bool isShutdown(uint8_t* error = NULL);

    // returns same errors as WireUtils::read16
    uint8_t getDeviceId(uint8_t* error = NULL);

    // all set(CS | DG | Gain | Sens | IT | AF | TRIG) functions return the same errors as WireUtils::write16Confirm
    // all get(CS | DG | Gain | Sens | IT | AF | TRIG) functions write the same errors as WireUtils::read16 to the error parameter

    // Channel Selection (named "SD_ALS only" in datasheet)
    enum {
        CS_All = 0,  // all channels active (default)
        CS_NoRB = 1, // R and B chanells deactivated (G, Clear, IR still active)
    };
    typedef uint8_t CS_t;
    uint8_t setCS(VEML3328::CS_t value);
    VEML3328::CS_t getCS(uint8_t* error = NULL);

    // Differential Gain
    enum {
        DG_1x = 0,  // (default)
        DG_2x = 1,
        DG_3x = 2
    };
    typedef uint8_t DG_t;
    uint8_t setDG(VEML3328::DG_t value);
    VEML3328::DG_t getDG(uint8_t* error = NULL);

    // Gain
    enum {
        Gain_0_5x = 0, // x 0.5
        Gain_1x = 1,   // x   1 (default)
        Gain_2x = 2,   // x   2
        Gain_4x = 3    // x   4
    };
    typedef uint8_t Gain_t;
    uint8_t setGain(VEML3328::Gain_t value);
    VEML3328::Gain_t getGain(uint8_t* error = NULL);

    // Sensitivity
    enum {
        Sens_HIGH = 0, // high sensitivity (default)
        Sens_LOW  = 1, // low  sensitivity (1/3 the sensitivity of high?)
    };
    typedef uint8_t Sens_t;
    uint8_t setSens(VEML3328::Sens_t value);
    VEML3328::Sens_t getSens(uint8_t* error = NULL);

    // Integration time
    enum {
        IT_50ms  = 0, // 50 ms  (default)
        IT_100ms = 1, // 100 ms
        IT_200ms = 2, // 200 ms
        IT_400ms = 3, // 400 ms
    };
    typedef uint8_t IT_t;
    uint8_t setIT(VEML3328::IT_t value);
    VEML3328::IT_t getIT(uint8_t* error = NULL);

    // Mode
    enum {
        Mode_AUTO = 0, // auto mode (default)
        Mode_AF   = 1, // active force mode
    };
    typedef uint8_t Mode_t;
    uint8_t setMode(VEML3328::Mode_t value);
    VEML3328::Mode_t getMode(uint8_t* error = NULL);

    // Trigger
    enum {
        Trig_NoTrigger = 0, // no trigger (default)
        Trig_Trigger   = 1, // trigger
    };
    typedef uint8_t Trig_t;
    // setting it to Trig_Trigger triggers a measurement in AF mode, the bit gets reset (=Trig_NoTrigger) when the measurement is complete 
    // (so setting it to Trig_NoTrigger doesnt really make sense)
    uint8_t setTrig(VEML3328::Trig_t value);
    VEML3328::Trig_t getTrig(uint8_t* error = NULL);



    enum {
        CHANNEL_CLEAR = 0x4,
        CHANNEL_RED   = 0x5,
        CHANNEL_GREEN = 0x6,
        CHANNEL_BLUE  = 0x7,
        CHANNEL_IR    = 0x8,
    };
    typedef uint8_t channel_t;

    // returns same errors as WireUtils::read16 + 8: invalid channel number
    uint16_t getChannelValue(VEML3328::channel_t channel, uint8_t* error);

private:
    uint16_t read16(const uint8_t register_address, uint8_t* error = NULL);
    uint8_t write16(const uint8_t register_address, const uint16_t data, uint16_t mask = 0xffff);
    uint8_t write16Confirm(const uint8_t register_address, const uint16_t data, uint16_t mask = 0xffff);
};

#endif