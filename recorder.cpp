#include "log.h"
#include <Wire.h>


    

// only use 0x50 <> 0x53
Recorder:Recorder( uint8_t _I2Caddress ) 
{
    I2Caddress = _I2Caddress ;
    
    state = getEvent ;
}

uint8_t Recorder::StartRecording() 
{
    if( mode == idle )
    {
        eeAdress = 0 ;
        mode = recording ;
        return 1 ;
    }
    return 0 ;                                              // return 0 if device is playing
}


uint8_t Recorder::StopRecording( )
{
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;
    
    Wire.write( stopEvent ) ;       // eeAdress ++ ; no need to increment address for a stop
    Wire.endTransmission() ;
    
    recording = false ;
    return 1 ;
}



// RECORDING FUNCTIONS
void Recorder::LogTime( )
{
    uint32_t currentTime = millis() ;
    uint32_t time = lastTime - millis() ;
    
    lastTime = currentTime ;
    
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;                                // set register
    
    Wire.write( logTime )    ; eeAdress ++ ;                // enter instruction followed by time stamp of 3 bytes
    Wire.write( time >> 16 ) ; eeAdress ++ ;                // we use 3 bytes for time.
    Wire.write( time >>  8 ) ; eeAdress ++ ; 
    Wire.write( time       ) ; eeAdress ++ ; 
    
    Wire.endTransmission() ;
}

void Recorder::resetTime() 
{
    lastTime = millis() ;
}

void Recorder::SensorEvent(  uint8_t sensor )
{    
    resetTime() ;
    
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;                                // set register
    
    Wire.write( sensorEvent ) ; eeAdress ++ ; 
    Wire.write( sensor ) ;      eeAdress ++ ;               // take not that bit 7 is used for the state.
    
    Wire.endTransmission() ;
}

void Recorder::LocoSpeedEvent( uint8_t DCC_address, uint8_t newSpeed )
{
    logTime() ;
    
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;                                // set register
    
    Wire.write( locoSpeedEvent ) ;  eeAdress ++ ; 
    Wire.write( DCC_address ) ;     eeAdress ++ ; 
    Wire.write( newSpeed ) ;        eeAdress ++ ; 
    
    Wire.endTransmission() ;        
}

void Recorder::LocoFunctionEvent( uint8_t DCC_address, uint8_t functions )
{
    logTime() ;
    
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;                                // set register
    
    Wire.write( locoFunctionEvent ) ;   eeAdress ++ ; 
    Wire.write( DCC_address ) ;         eeAdress ++ ; 
    Wire.write( functions ) ;           eeAdress ++ ;       // bit 7 is state of function, bit 0-5 are functions

    Wire.endTransmission() ;
}

uint8_t Recorder::AccessoryEvent( uint8_t address, uint8_t state )
{
    logTime() ;
    
    Wire.beginTransmission( I2Caddress ) ;   // channel is 1-4
    Wire.write( eeAdress ) ;                                // set register
    
    Wire.write( accessoryEvent ) ;  eeAdress ++ ; 
    Wire.write( DCC_address ) ;     eeAdress ++ ; 
    Wire.write( state ) ;           eeAdress ++ ;       // bit 7 is state of function, bit 0-5 are functions

    Wire.endTransmission() ;
}

uint8_t Recorder::getMode()
{
    return mode ;
}

int16_t Recorder::GetProgramSize( )
{
    uint16_t tempAddress = 0 ;
    for( tempAddress = 0 ; tempAddress < 0x7FFF ; tempAddress ++ )
    {
        Wire.beginTransmission( I2Caddress ) ;
        Wire.write( tempAddress ) ;
        Wire.endTransmission() ;
        
        Wire.requestFrom( baseAddress + channel - 1, 1 ) ;  // read one byte per time
        uint8_t b = Wire.read() ;
        if( b == stopEvent ) return tempAddress ;
    }
    return -1 ;
}

