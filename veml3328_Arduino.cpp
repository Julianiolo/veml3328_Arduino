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
#define VEML3328_CONFIG_ALL_MASK      0b1111110001111101  // all bits set except for reserved ones


#define VEML3328_CONFIG_CS_SHIFT   14
#define VEML3328_CONFIG_DG_SHIFT   12
#define VEML3328_CONFIG_GAIN_SHIFT 10
#define VEML3328_CONFIG_SENS_SHIFT  6
#define VEML3328_CONFIG_IT_SHIFT    4
#define VEML3328_CONFIG_AF_SHIFT    3
#define VEML3328_CONFIG_TRIG_SHIFT  2

VEML3328::VEML3328() {
    
}

uint8_t VEML3328::begin(TwoWire* wire_, const uint8_t device_addr_, bool wake_up) {
    device_addr = device_addr_;
    wire = wire_;

    if(wake_up)
        return this->wakeUp();

    return 0;
}

uint8_t VEML3328::wakeUp(void) {
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, 0x0, VEML3328_CONFIG_SHUTDOWN_MASK);
}
uint8_t VEML3328::shutdown(void) {
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, 0b1000000000000001, VEML3328_CONFIG_SHUTDOWN_MASK);
}
bool VEML3328::isShutdown(uint8_t* error) {
    uint16_t val = this->read16(VEML3328_CONFIG_REG_ADDR, error) & VEML3328_CONFIG_SHUTDOWN_MASK;
    return val & 0b1;  // we just look at the last bit, technically both bits need to be 1, but just one being on is UB (i think?) so yeah...
}


uint8_t VEML3328::getDeviceId(uint8_t* error) {
    return this->read16(VEML3328_DEVICEID_REG_ADDR, error);
}


void VEML3328::Config::setX(uint8_t value, uint16_t mask, uint8_t shift) {
    this->value &= ~mask;  // clear out old value
    this->value |= ((uint16_t)value << shift) & mask;
    this->mask |= mask;
}
uint8_t VEML3328::Config::getX(uint16_t mask, uint8_t shift) const {
    if((this->mask & mask) != mask) return -1;  // no value present

    return (this->value & mask) >> shift;
}

void VEML3328::Config::setSD(VEML3328::SD_t value) {
    this->setX(value ? VEML3328_CONFIG_SHUTDOWN_MASK : 0, VEML3328_CONFIG_SHUTDOWN_MASK, 0);
}
VEML3328::SD_t VEML3328::Config::getSD(void) const {
    return this->getX(VEML3328_CONFIG_SHUTDOWN_MASK, 0) & 0b1;
}

void VEML3328::Config::setCS(VEML3328::CS_t value) {
    this->setX(value, VEML3328_CONFIG_CS_MASK, VEML3328_CONFIG_CS_SHIFT);
}
VEML3328::CS_t VEML3328::Config::getCS(void) const {
    return this->getX(VEML3328_CONFIG_CS_MASK, VEML3328_CONFIG_CS_SHIFT);
}

void VEML3328::Config::setDG(VEML3328::DG_t value) {
    this->setX(value, VEML3328_CONFIG_DG_MASK, VEML3328_CONFIG_DG_SHIFT);
}
VEML3328::DG_t VEML3328::Config::getDG(void) const {
    return this->getX(VEML3328_CONFIG_DG_MASK, VEML3328_CONFIG_DG_SHIFT);
}

void VEML3328::Config::setGain(VEML3328::Gain_t value) {
    this->setX(value, VEML3328_CONFIG_GAIN_MASK, VEML3328_CONFIG_GAIN_SHIFT);
}
VEML3328::Gain_t VEML3328::Config::getGain(void) const {
    return this->getX(VEML3328_CONFIG_GAIN_MASK, VEML3328_CONFIG_GAIN_SHIFT);
}

void VEML3328::Config::setSens(VEML3328::Sens_t value) {
    this->setX(value, VEML3328_CONFIG_SENS_MASK, VEML3328_CONFIG_SENS_SHIFT);
}
VEML3328::Sens_t VEML3328::Config::getSens(void) const {
    return this->getX(VEML3328_CONFIG_SENS_MASK, VEML3328_CONFIG_SENS_SHIFT);
}

void VEML3328::Config::setIT(VEML3328::IT_t value) {
    this->setX(value, VEML3328_CONFIG_IT_MASK, VEML3328_CONFIG_IT_SHIFT);
}
VEML3328::IT_t VEML3328::Config::getIT(void) const {
    return this->getX(VEML3328_CONFIG_IT_MASK, VEML3328_CONFIG_IT_SHIFT);
}

void VEML3328::Config::setMode(VEML3328::Mode_t value) {
    this->setX(value, VEML3328_CONFIG_AF_MASK, VEML3328_CONFIG_AF_SHIFT);
}
VEML3328::Mode_t VEML3328::Config::getMode(void) const {
    return this->getX(VEML3328_CONFIG_AF_MASK, VEML3328_CONFIG_AF_SHIFT);
}

void VEML3328::Config::setTrig(VEML3328::Trig_t value) {
    this->setX(value, VEML3328_CONFIG_TRIG_MASK, VEML3328_CONFIG_TRIG_SHIFT);
}
VEML3328::Trig_t VEML3328::Config::getTrig(void) const {
    return this->getX(VEML3328_CONFIG_TRIG_MASK, VEML3328_CONFIG_TRIG_SHIFT);
}


uint8_t VEML3328::setConfig(VEML3328::Config value) {
    return this->write16Confirm(VEML3328_CONFIG_REG_ADDR, value.value, value.mask);
}
VEML3328::Config VEML3328::getConfig(uint8_t* error) {
    VEML3328::Config conf;
    conf.value = this->read16(VEML3328_CONFIG_REG_ADDR, error);
    conf.mask = VEML3328_CONFIG_ALL_MASK;
    return conf;
}


uint16_t VEML3328::getChannelValue(VEML3328::channel_t channel, uint8_t* error) {
    if(channel < CHANNEL_CLEAR || channel > CHANNEL_IR) {
        if(error != NULL) *error = 8;  // invalid channel number error
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