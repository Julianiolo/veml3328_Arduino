#ifndef __VEML3328_H__
#define __VEML3328_H__

#include <Arduino.h>
#include <Wire.h>

// https://www.vishay.com/docs/84968/veml3328.pdf
// https://www.vishay.com/docs/80010/designingveml3328.pdf

#define VEML3328_VERSION "1.0.0"
#define VEML3328_VERSION_MAJOR 1
#define VEML3328_VERSION_MINOR 0
#define VEML3328_VERSION_PATCH 0

#define VEML3328_DEF_ADDR 0x10

class VEML3328 {
private:
    uint8_t device_addr = 0; // i2c address of the device
    TwoWire* wire = NULL;   // i2c interface
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
        DG_4x = 2
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

    /*
        meas_ctx_t is a value that stores all the context for a measurement to be able to calculate a lux value from the raw counts; see VEML3328::toUniversalUnit()
        Bits:
                7: Error
            6-5: DG
            4-3: Gain
                2: Sens
            1-0: IT
    */
    typedef uint8_t meas_ctx_t;


    // this class allows writing a complete config to the device at once (since everything is in the same register)
    // a config only ever changes the values set by a set* function, all other values are left unchanged
    class Config {
    public:
        uint16_t value = 0; // dont use unless you know what you're doing
        uint16_t mask  = 0; // dont use unless you know what you're doing

        /*
            all get* functions return (uint8_t)-1 (so 255) if no value is present (== mask is not set for this value)
        */

        // Shutdown/Awake
        void setSD(VEML3328::SD_t value);
        VEML3328::SD_t getSD(void) const;

        // Channel Selection (named "SD_ALS only" in datasheet)
        void setCS(VEML3328::CS_t value);
        VEML3328::CS_t getCS(void) const;

        // Differential Gain
        void setDG(VEML3328::DG_t value);
        VEML3328::DG_t getDG(void) const;

        // Gain
        void setGain(VEML3328::Gain_t value);
        VEML3328::Gain_t getGain(void) const;

        // Sensitivity
        void setSens(VEML3328::Sens_t value);
        VEML3328::Sens_t getSens(void) const;

        // Integration time
        void setIT(VEML3328::IT_t value);
        VEML3328::IT_t getIT(void) const;

        // Mode
        void setMode(VEML3328::Mode_t value);
        VEML3328::Mode_t getMode(void) const;

        // Trigger
        //   setting it to Trig_Trigger triggers a measurement in AF mode, the bit gets reset (=Trig_NoTrigger) when the measurement is complete 
        //   (so setting it to Trig_NoTrigger doesnt really make sense)
        void setTrig(VEML3328::Trig_t value);
        VEML3328::Trig_t getTrig(void) const;

        meas_ctx_t toCtx(void) const;
        static Config fromCtx(meas_ctx_t ctx);

    protected:
        // helpers
        void setX(uint8_t value, uint16_t mask, uint8_t shift);
        uint8_t getX(uint16_t mask, uint8_t shift) const;
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
    uint16_t getChannelValue(VEML3328::channel_t channel, uint8_t* error = NULL);

    // this converts a measurement value and the used settings for that measurement into a universal value independent of settings used
    // equivalent to (theoredical) value at settings DG=4x GAIN=4x SENS=1x IT=400ms
    // multiply by 0.003 lux to get lux value (or just use toFloat)
    static uint32_t toUniversalUnit(VEML3328::meas_ctx_t ctx, uint16_t value);
    static uint32_t toUniversalUnit(VEML3328::Config config, uint16_t value);

    // this needs the value calculated by toUniversalUnit as input
    static float toFloat(uint32_t universal_unit_value);



    /*
        Get a fitting config (meas_ctx_t) from a value which was caputred at the one level before the minimum sensitivity level
    */
    VEML3328::meas_ctx_t getFittingConfig(uint16_t value, float threshold = 0.85f);

    /*
        Automatically determine a config by taking a single measurement

        returns same errors as setConfig() and getChannelValue() offset by 16
    */
    VEML3328::meas_ctx_t autoConfig(VEML3328::channel_t channel = CHANNEL_CLEAR, uint8_t* error = NULL);
    
    struct CONF{
        uint16_t sens; // normalized sensitivity (relative to lowest sensitivity)
        meas_ctx_t ctx;
    };

    // see scripts/sensitivity.py
    static constexpr CONF auto_steps[16] = {{1, 0x4}, {2, 0xc}, {3, 0x0}, {4, 0x14}, {6, 0x8}, {8, 0x1c}, {12, 0x10}, {16, 0x1d}, {24, 0x18}, {32, 0x1e}, {48, 0x38}, {64, 0x1f}, {96, 0x58}, {192, 0x59}, {384, 0x5a}, {768, 0x5b}};
private:

    uint16_t read16(const uint8_t register_address, uint8_t* error = NULL);
    uint8_t write16(const uint8_t register_address, const uint16_t data, uint16_t mask = 0xffff);
    uint8_t write16Confirm(const uint8_t register_address, const uint16_t data, uint16_t mask = 0xffff);
};

#endif