#ifndef INFORMER_H
#define  INFORMER_H

class Informer{
  // will flash a led infinitely a number of times

  protected:
    const uint8_t   pin;
    static const uint16_t  flashTime = 200,  // millisecs
                           pauseTime = 1000; // millisecs

  public:
    Informer(uint8_t p);
    void informForever (uint8_t nbFlashes) const;
    void informOnce (uint8_t nbFlashes) const;
};

#endif
