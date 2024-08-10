#include "veml3328_Arduino.h"

#include "WireUtils.h"

#define VEML3328_CONFIG_REG_ADDR 0x00
#define VEML3328_DEVICEID_REG_ADDR 0x0c

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

VEML3328::meas_ctx_t VEML3328::Config::toCtx(void) const {
    const uint8_t dg = getDG();
    if(dg == (uint8_t)-1)   return 0b10000000;
    const uint8_t gain = getGain();
    if(gain == (uint8_t)-1) return 0b10000001;
    const uint8_t sens = getSens();
    if(sens == (uint8_t)-1) return 0b10000010;
    const uint8_t it = getIT();
    if(it == (uint8_t)-1)   return 0b10000011;
    return (dg << 5) | (gain << 3) | (sens << 2) | it;
}
VEML3328::Config VEML3328::Config::fromCtx(meas_ctx_t ctx) {
    if(ctx & 0b10000000) return VEML3328::Config();

    Config config;
    config.setDG((ctx >> 5) & 0b11);
    config.setGain((ctx >> 3) & 0b11);
    config.setSens((ctx >> 2) & 0b1);
    config.setIT(ctx & 0b11);

    return config;
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

VEML3328::meas_ctx_t VEML3328::getFittingConfig(uint16_t value, float threshold) {
    const uint32_t threshold_value = (uint32_t)(0xffff*threshold);

    if(value > threshold_value)
        return auto_steps[0].ctx; // if already basically maxed out when using one above minimum sensitivity, then we use minimum sensitivity

    const uint32_t universal_value = toUniversalUnit(auto_steps[1].ctx, value);

    uint8_t i = 0;
    while(i+1 < 16) {
        const uint32_t v_s = (universal_value * auto_steps[i+1].sens)/auto_steps[15].sens; // simulated value for next sensitivity step
        if(v_s > threshold_value) { // check if value would overflow for next sensitivity step
            break;
        }
        i++;
    }
    return auto_steps[i].ctx;
}

VEML3328::meas_ctx_t VEML3328::autoConfig(VEML3328::channel_t channel, uint8_t* error) {
    {
        Config config = Config::fromCtx(auto_steps[1].ctx);

        uint8_t ret = setConfig(config);
        if(ret != 0) {
            if(error != NULL) *error = ret;
            return -1;
        }
    }

    uint8_t err = 0;
    const uint16_t v = getChannelValue(channel, &err);
    if(err != 0) {
        if(error != NULL) *error = err + 16;
        return -1;
    }

    return getFittingConfig(v);
}

uint32_t VEML3328::toUniversalUnit(VEML3328::meas_ctx_t ctx, uint16_t value) {
    return toUniversalUnit(Config::fromCtx(ctx), value);
}
uint32_t VEML3328::toUniversalUnit(VEML3328::Config config, uint16_t value) {
    const uint8_t dg = config.getDG();
    if(dg == (uint8_t)-1)   return -1;
    const uint8_t gain = config.getGain();
    if(gain == (uint8_t)-1) return -2;
    const uint8_t sens = config.getSens();
    if(sens == (uint8_t)-1) return -3;
    const uint8_t it = config.getIT();
    if(it == (uint8_t)-1)   return -4;
    
    const uint8_t mult_dg[] = {4,2,1};
    const uint8_t mult_gain[] = {8,4,2,1};
    const uint8_t mult_sens[] = {3,1};
    const uint8_t mult_it[] = {8,4,2,1};
    
    return (uint32_t)value * (uint32_t)mult_dg[dg] * (uint32_t)mult_gain[gain] * (uint32_t)mult_sens[sens] * (uint32_t)mult_it[it];
}

float VEML3328::toFloat(uint32_t universal_unit_value) {
    return (float)universal_unit_value * 0.003;
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