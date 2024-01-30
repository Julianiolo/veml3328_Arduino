/*
    This sketch tries all measurement related settings and takes measurements with them. Useful for seeing what each setting does.
    It tries:
        DG (Differential Gain)
        Gain
        Sens (Sensitivity)
        IT (Integration Time)
*/

#include "veml3328_Arduino.h"

VEML3328 veml3328;

static void set_config(VEML3328::Config config) {
    uint8_t ret = veml3328.setConfig(config);
    if(ret != 0) {
        Serial.printf("Error: could not set config of veml3328: %d\n", ret);
    }
}

static void set_default_config(void) {
    VEML3328::Config config;
    config.setSD(VEML3328::SD_Awake); // wake up
    config.setMode(VEML3328::Mode_AF); // manual trigger
    config.setCS(VEML3328::CS_All); // enable all channels

    config.setGain(VEML3328::Gain_1x);
    config.setDG(VEML3328::DG_1x);
    config.setIT(VEML3328::IT_50ms);
    config.setSens(VEML3328::Sens_HIGH);

    uint8_t ret = veml3328.setConfig(config);
    if(ret != 0) {
        Serial.printf("Error: could not set default config of veml3328: %d\n", ret);
    }
}

static void trigger_measurement_and_wait(void) {
    {
        VEML3328::Config config;
        config.setTrig(VEML3328::Trig_Trigger);
        set_config(config);
    }

    delay(50); // minimum measurement IT is 50ms, so it takes at least this long

    bool has_finished = false;
    // wait for maximum of 1s
    for(uint8_t i = 0; i < 10; i++) {
        uint8_t error = 0;
        VEML3328::Config config = veml3328.getConfig(&error);

        if(error != 0) {
            Serial.printf("Error on getConfig while waiting for measurement: %d\n", error);
        }else{
            if(config.getTrig() == VEML3328::Trig_NoTrigger) {
                has_finished = true;
                break;
            }
        }

        delay(100);
    }

    if(!has_finished) {
        Serial.println("Timeout while waiting for measurement");
    }
}

static void print_config(void) {
    uint8_t error = 0;
    const VEML3328::Config config = veml3328.getConfig(&error);
    if(error != 0) {
        Serial.printf("Error: could not get config of veml3328: %d\n", error);
        return;
    }

    // SD
    {
        const char* labels[] = {"Awake","Shutdown"};
        Serial.printf("%8s, ", labels[config.getSD()]);
    }

    // Mode
    {
        const char* labels[] = {"AUTO","AF"};
        Serial.printf("%4s, ", labels[config.getMode()]);
    }

    // CS
    {
        const char* labels[] = {"ALL","NoRB"};
        Serial.printf("%4s, ", labels[config.getCS()]);
    }

    // Trig
    {
        Serial.printf("TRG: %d, ", config.getTrig());
    }

    // DG
    {
        if(config.getDG() == 3) { // illegal reserved value
            Serial.print("DG:ILL, ");
        } else{
            const uint8_t values[] = { 1, 2, 3};
            Serial.printf("DG: %dx, ", values[config.getDG()]);
        }
    }

    // Gain
    {
        const float values[] = { 0.5, 1, 2, 4};
        Serial.printf("Gain: %3fx, ", values[config.getGain()]);
    }

    // Sens
    {
        const char* labels[] = {"Hi","Lo"};
        Serial.printf("Sens: %s, ", labels[config.getSens()]);
    }

    // IT
    {
        const uint16_t values[] = { 50, 100, 200, 400};
        Serial.printf("Sens: %3dms", values[config.getIT()]);
    }

    Serial.println();
}

static void print_measurement(void) {
    const uint8_t channels[5] = {VEML3328::CHANNEL_CLEAR, VEML3328::CHANNEL_IR, VEML3328::CHANNEL_RED, VEML3328::CHANNEL_GREEN, VEML3328::CHANNEL_BLUE};
    const char* names[5] = {"Clear", "IR", "Red", "Green", "Blue"};

    for(uint8_t i = 0; i < 5; i++) {
        Serial.print(names[i]);
        Serial.print(": ");
        
        uint8_t error = 0;
        uint16_t value = veml3328.getChannelValue(channels[i], &error);

        if(error != 0) {
            Serial.printf("ERR:%d, ", error);
        }else{
            Serial.printf("%5d, ", value);
        }
    }

    Serial.println();
}

static void take_measurement(VEML3328::Config config) {
    set_config(config);
    trigger_measurement_and_wait();
    print_config();
    print_measurement();
}

void setup() {
    Serial.begin(115200);
    
    uint8_t ret = veml3328.begin(&Wire, VEML3328_DEF_ADDR, false); // dont wake up yet
    if(ret != 0) {
        Serial.printf("Error: could not initialize veml3328: %d\n", ret);
        return;
    }
    
    set_default_config();
    Serial.println("Default:");
    trigger_measurement_and_wait();
    print_config();
    print_measurement();
    Serial.println();

    Serial.println("DG:");
    for(uint8_t i = 0; i < 3; i++) {
        VEML3328::Config config;
        config.setDG(i);
        take_measurement(config);
    }
    Serial.println();

    set_default_config();

    Serial.println("Gain:");
    for(uint8_t i = 0; i < 4; i++) {
        VEML3328::Config config;
        config.setGain(i);
        take_measurement(config);
    }
    Serial.println();

    set_default_config();

    Serial.println("Sens:");
    for(uint8_t i = 0; i < 2; i++) {
        VEML3328::Config config;
        config.setSens(i);
        take_measurement(config);
    }
    Serial.println();

    set_default_config();

    Serial.println("IT:");
    for(uint8_t i = 0; i < 4; i++) {
        VEML3328::Config config;
        config.setIT(i);
        take_measurement(config);
    }
    Serial.println();

    Serial.println("Done :)");
}

void loop() {

}