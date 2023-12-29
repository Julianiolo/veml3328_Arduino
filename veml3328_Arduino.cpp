#include "veml3328_Arduino.h"

#include "WireUtils.h"

#define VEML3328_CONFIG_REG_ADDR 0x0
#define VEML3328_DEVICEID_REG_ADDR 0x0

#define VEML3328_CONFIG_SHUTDOWN_MASK 0b1000000000000001
#define VEML3328_CONFIG_CS_MASK       0b0100000000000000
#define VEML3328_CONFIG_DG_MASK       0b0011000000000000
#define VEML3328_CONFIG_GAIN_MASK     0b0000110000000000
#define VEML3328_CONFIG_SENS_MASK     0b0000000001000000
#define VEML3328_CONFIG_IT_MASK       0b0000000000110000
#define VEML3328_CONFIG_AF_MASK       0b0000000000001000
#define VEML3328_CONFIG_TRIG_MASK     0b0000000000000100

#define VEML3328_CONFIG_CS_SHIFT   14
#define VEML3328_CONFIG_DG_SHIFT   12
#define VEML3328_CONFIG_GAIN_SHIFT 10
#define VEML3328_CONFIG_SENS_SHIFT  6
#define VEML3328_CONFIG_IT_SHIFT    4
#define VEML3328_CONFIG_AF_SHIFT    3
#define VEML3328_CONFIG_TRIG_SHIFT  2


uint8_t VEML3328::begin(TwoWire* wire_ = &Wire, const uint8_t device_addr_ = VEML3328_DEF_ADDR) {
    device_addr = device_addr_;
    wire = wire_;

    return this->wakeUp();
}

uint8_t VEML3328::wakeUp(void) {
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, 0x0, VEML3328_CONFIG_SHUTDOWN_MASK);
}
uint8_t VEML3328::shutdown(void) {
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, 0b1000000000000001, VEML3328_CONFIG_SHUTDOWN_MASK);
}
bool VEML3328::isShutdown(uint8_t* error = NULL) {
    uint16_t val = this->read16(VEML3328_CONFIG_REG_ADDR, error) & VEML3328_CONFIG_SHUTDOWN_MASK;
    return val == VEML3328_CONFIG_SHUTDOWN_MASK;
}


uint8_t VEML3328::getDeviceId(uint8_t* error) {
    return this->read16(VEML3328_DEVICEID_REG_ADDR, error);
}


uint8_t VEML3328::setCS(VEML3328::CS_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_CS_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_CS_MASK);
}
VEML3328::CS_t VEML3328::getCS(uint8_t* error = NULL) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_CS_SHIFT) & 0b1;
}

uint8_t VEML3328::setDG(VEML3328::DG_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_DG_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_DG_MASK);
}
VEML3328::DG_t VEML3328::getDG(uint8_t* error) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_DG_SHIFT) & 0b11;
}

uint8_t VEML3328::setGain(VEML3328::Gain_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_GAIN_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_GAIN_MASK);
}
VEML3328::Gain_t VEML3328::getGain(uint8_t* error) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_GAIN_SHIFT) & 0b11;
}

uint8_t VEML3328::setSens(VEML3328::Sens_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_SENS_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_SENS_MASK);
}
VEML3328::Sens_t VEML3328::getSens(uint8_t* error = NULL) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_SENS_SHIFT) & 0b1;
}

uint8_t VEML3328::setIT(VEML3328::IT_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_IT_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_IT_MASK);
}
VEML3328::IT_t VEML3328::getIT(uint8_t* error = NULL) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_IT_SHIFT) & 0b1;
}

uint8_t VEML3328::setMode(VEML3328::Mode_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_AF_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_AF_MASK);
}
VEML3328::Mode_t VEML3328::getMode(uint8_t* error = NULL) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_AF_SHIFT) & 0b1;
}

uint8_t VEML3328::setTrig(VEML3328::Trig_t value) {
    uint16_t actual_value = value << VEML3328_CONFIG_TRIG_SHIFT;
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, actual_value, VEML3328_CONFIG_TRIG_MASK);
}
VEML3328::Trig_t VEML3328::getTrig(uint8_t* error = NULL) {
    return (this->read16(VEML3328_CONFIG_REG_ADDR, error) >> VEML3328_CONFIG_TRIG_SHIFT) & 0b1;
}



uint16_t VEML3328::getChannelValue(VEML3328::channel_t channel, uint8_t* error) {
    if(channel < CHANNEL_CLEAR || channel > CHANNEL_IR) {
        if(error != NULL) *error = 8;
        return 0;
    }

    return this->read16(channel, error);
}

// helpers

uint16_t VEML3328::read16(const uint8_t register_address, uint8_t* error) {
    return WireUtils::read16(wire, device_addr, register_address, error);
}
uint8_t VEML3328::write16(const uint8_t register_address, const uint16_t data, uint16_t mask) {
    return WireUtils::write16(wire, device_addr, register_address, data, mask);
}
uint8_t VEML3328::write16Confirm(const uint8_t register_address, const uint16_t data, uint16_t mask) {
    return WireUtils::write16Confirm(wire, device_addr, register_address, data, mask);
}