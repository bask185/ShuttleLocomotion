#include <Arduino.h>

class Player
{
public:
    Player() ;
    void StartPlaying( ) ;
    void StopPlaying( ) ;
    bool stateMachine() ;

private:
    bool getEventF() ;
    bool waitTimeF() ;
    bool waitSensorF() ;
    bool setAccessoryF() ;
    bool setLocoSpeedF() ;
    bool setLocoFunctionF() ; 
    bool stopPlayingF();
    
    uint8_t I2Caddress ;
    uint8_t retVal[3] ;
    uint16_t eeAdress ;
    uint32_t lastTime ;
    uint8_t state ;
} ;