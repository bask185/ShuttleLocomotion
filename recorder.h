#include <Arduino.h>

enum events 
{
    sensorEvent = 1,
    locoSpeedEvent,
    locoFunctionEvent,
    timeExpireEvent,
    accessoryEvent,
    stopEvent = 255,
} ;

enum modes
{
    idle,
    playing,
    recording,
} ;

class Recorder() 
{
public:
    Recorder() ;
    uint8_t StartRecording( ) ;
    uint8_t StopRecording( );
    void SensorEvent( uint8_t ) ;
    void LocoSpeedEvent( uint8_t, uint8_t ) ;
    void LocoFunctionEvent( uint8_t, uint8_t ) ;
    void AccessoryEvent( uint8_t, uint8_t ) ;
    int16_t GetProgramSize( ) ;
    
    uint8_t GetNextEvent( uint8_t*, uint8_t*, uint8_t* )
    
    uint8_t GetMode( ) ;
    
    void StartPlaying( ) ;
    void StopPlaying( ) ;
    
private:
    uint16_t eeAdress ;
    uint32_t lastTime ;
    uint8_t mode ;
    uint8_t I2Caddress ;
    
    void logTime() ;
    void resetTime() ;
} ;