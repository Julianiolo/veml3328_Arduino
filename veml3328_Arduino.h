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
    uint8_t begin(TwoWire* wire = &Wire, const uint8_t device_addr = VEML3328_DEF_ADDR, bool wake_up = false);

    // returns same errors as WireUtils::write16Confirm
    uint8_t wakeUp(void);
    // returns same errors as WireUtils::write16Confirm
    uint8_t shutdown(void);

    // returns same errors as WireUtils::read16
    bool isShutdown(uint8_t* error = NULL);

    // returns same errors as WireUtils::read16
    uint8_t getDeviceId(uint8_t* error = NULL);

    // Shutdown/Awake
    enum {
        SD_Awake    = 0, 
        SD_Shutdown = 1,  // (default)
    };
    typedef uint8_t SD_t;

    // Channel Selection (named "SD_ALS only" in datasheet)
    enum {
        CS_All  = 0, // all channels active (default)
        CS_NoRB = 1, // R and B chanells deactivated (G, Clear, IR still active)
    };
    typedef uint8_t CS_t;

    // Differential Gain
    enum {
        DG_1x = 0,  // (default)
        DG_2x = 1,
        DG_3x = 2
    };
    typedef uint8_t DG_t;

    // Gain
    enum {
        Gain_0_5x = 0, // x 0.5
        Gain_1x   = 1, // x   1 (default)
        Gain_2x   = 2, // x   2
        Gain_4x   = 3  // x   4
    };
    typedef uint8_t Gain_t;

    // Sensitivity
    enum {
        Sens_HIGH = 0, // high sensitivity (default)
        Sens_LOW  = 1, // low  sensitivity (1/3 the sensitivity of high?)
    };
    typedef uint8_t Sens_t;

    // Integration time
    enum {
        IT_50ms  = 0, // 50 ms  (default)
        IT_100ms = 1, // 100 ms
        IT_200ms = 2, // 200 ms
        IT_400ms = 3, // 400 ms
    };
    typedef uint8_t IT_t;

    // Mode
    enum {
        Mode_AUTO = 0, // auto mode (default)
        Mode_AF   = 1, // active force mode
    };
    typedef uint8_t Mode_t;

    // Trigger
    enum {
        Trig_NoTrigger = 0, // no trigger (default)
        Trig_Trigger   = 1, // trigger
    };
    typedef uint8_t Trig_t;

    // this class allows writing a complete config to the device at once (since everything is in the same register)
    class Config {
    public:
        uint16_t value; // dont use unless you know what you're doing
        uint16_t mask;  // dont use unless you know what you're doing

        /*
            all get* functions return (uint8_t)-1 (so 255) if no value is present (mask is not set for this value)
        */

        // Shutdown/Awake
        void setSD(VEML3328::SD_t value);
        VEML3328::SD_t getSD(void);

        // Channel Selection (named "SD_ALS only" in datasheet)
        void setCS(VEML3328::CS_t value);
        VEML3328::CS_t getCS(void);

        // Differential Gain
        void setDG(VEML3328::DG_t value);
        VEML3328::DG_t getDG(void);

        // Gain
        void setGain(VEML3328::Gain_t value);
        VEML3328::Gain_t getGain(void);

        // Sensitivity
        void setSens(VEML3328::Sens_t value);
        VEML3328::Sens_t getSens(void);

        // Integration time
        void setIT(VEML3328::IT_t value);
        VEML3328::IT_t getIT(void);

        // Mode
        void setMode(VEML3328::Mode_t value);
        VEML3328::Mode_t getMode(void);

        // Trigger
        //   setting it to Trig_Trigger triggers a measurement in AF mode, the bit gets reset (=Trig_NoTrigger) when the measurement is complete 
        //   (so setting it to Trig_NoTrigger doesnt really make sense)
        void setTrig(VEML3328::Trig_t value);
        VEML3328::Trig_t getTrig(void);

    protected:
        // helpers
        void setX(uint8_t value, uint16_t mask, uint8_t shift);
        uint8_t getX(uint16_t mask, uint8_t shift);
    };

    // returns the same errors as WireUtils::write16Confirm
    uint8_t setConfig(VEML3328::Config value);
    // writes the same errors as WireUtils::read16 to the error parameter
    VEML3328::Config getConfig(uint8_t* error = NULL);

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