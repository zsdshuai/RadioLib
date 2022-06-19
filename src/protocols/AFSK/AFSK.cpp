#include "AFSK.h"
#if !defined(RADIOLIB_EXCLUDE_AFSK)

AFSKClient::AFSKClient(PhysicalLayer* phy, RADIOLIB_PIN_TYPE pin): _pin(pin) {
  _phy = phy;
}

int16_t AFSKClient::begin() {
  if(_pin == RADIOLIB_NC) {
    // set encoding to NRZ
    int16_t state = _phy->setEncoding(RADIOLIB_ENCODING_NRZ);
    RADIOLIB_ASSERT(state);

    // disable preamble
    state = _phy->setPreambleLength(0);
    RADIOLIB_ASSERT(state);

    // disable CRC
    state = _phy->setCrcFiltering(false);
    return(state);
  }
  return(_phy->startDirect());
}

int16_t AFSKClient::tone(uint16_t freq, bool autoStart) {
  if(freq == 0) {
    return(RADIOLIB_ERR_INVALID_FREQUENCY);
  }

  if(autoStart) {
    int16_t state = _phy->transmitDirect();
    RADIOLIB_ASSERT(state);
  }

  Module* mod = _phy->getMod();
  mod->tone(_pin, freq);
  return(RADIOLIB_ERR_NONE);
}

int16_t AFSKClient::noTone(bool keepOn) {
  Module* mod = _phy->getMod();
  mod->noTone(_pin);
  if(keepOn) {
    return(0);
  }

  return(_phy->standby());
}

int16_t AFSKClient::tones(float baseFreq, uint8_t* bits, size_t len) {
  // set fixed packet length mode
  int16_t state = _phy->fixedPacketLengthMode(len);
  RADIOLIB_ASSERT(state);

  float br = baseFreq;
  if((br < 1200)) {
    if(br < 1200/8) {
      // this is too low to achieve by a single octet
      // FIXME - if tones lower than 150 Hz are needed, multiple octets must be used
      return(RADIOLIB_ERR_INVALID_FREQUENCY);
    }
    // this tone frequency is lower than what most modules can do
    // but we can easily emulate tones in this range
    br *= 8.0;
  }

  // set the base frequency
  state = _phy->setBitRate(br / 1000.0);
  RADIOLIB_ASSERT(state);

  // transmit
  return(_phy->transmit(bits, len));
}

#endif
