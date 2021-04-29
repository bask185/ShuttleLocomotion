#include <Wire.h>

enum states {
    waitTime ,
    getEvent
    waitSensor ,
    setAccessory ,
    setLocoSpeed ,
    setLocoFunction ,
    stopPlaying ,
} ;

// STATE MACHINE FUNCTIONS
bool Player::getEventF()
{
    Wire.beginTransmission( I2Caddress ) ;
    Wire.write( eeAddress++ ) ;
    Wire.endTransmission() ;
    
    Wire.requestFrom( I2Caddress, 1; ) ;
    uint8_t nextEvent = Wire.read() ;                       // fetch next event
    uint8_t nArguments = 0 ;
    switch( nextEvent )
    {
        case timeExpireEvent:   nArguments = 3 ; break ;    // time + time + time
        case locoFunctionEvent:                             // address + state|function
        case locoSpeedEvent:                                // address + speed
        case accessoryEvent:    nArguments = 2 ; break ;    // address + state
        case sensorEvent:       nArguments = 1 ; break ;    // state|sensor
        case stopEvent:         nArguments = 0 ; break ;    // N/A
    }
    
    if( nArguments )                                        // if atleast one argument, initiate the request
    {
        Wire.beginTransmission( I2Caddress ) ;
        Wire.write( eeAddress ) ;
        Wire.endTransmission() ;
        Wire.requestFrom( I2Caddress, nArguments; ) ;
        
        if( nArguments > 0 ) { retval[1] = Wire.read() ; eeAddress ++ ; } // always true...
        if( nArguments > 1 ) { retval[2] = Wire.read() ; eeAddress ++ ; }
        if( nArguments > 2 ) { retval[3] = Wire.read() ; eeAddress ++ ; }
    }
    return true ;
}

bool Player::waitTimeF()
{
    if( millis() - lastTime >= timeToRun) return true ;
    return false ;
}

bool Player::waitSensorF()
{
    if( sens[1].state == newState ) return true ;
    return false ;
}

bool Player::setAccessoryF()
{
    XpressNet.setTrntPos( high(newAccessory) , 
                          low(newAccessory) , 
                          newState ) ;
    return true ;
}

bool Player::setLocoSpeedF()
{
    XpressNet.setLocoDrive( high( newLoco ) , 
                            low( newLoco) , 
                            28/*ovb*/ , 
                            newSpeed ) ;
    return true ;
}

bool Player::setLocoFunctionF()
{
	XpressNet.setLocoFunc( high (newLoco ) , 
                           low( newLoco ) , 
                           1/*what type goes here?*/ , 
                           newFunction ) ;
    return true ;
}

bool Player::stopPlayingF()
{
    // do something
   return true ;
}


// STATE MACHINE
#define State(x) break; case if(x##F())
bool Player::stateMachine()
{
    switch( state ) {

        State(waitTime) {
            state = getEvent ; }

        State(waitSensor) {
            state = getEvent ; }

    #define nextState(x) if( nextEvent == x ) state = x ;
        State(getEvent) {
            nextState( waitSensor ) ; 
            nextState( waitTime ) ; 
            nextState( setAccessory ) ;  
            nextState( setLocoSpeed ) ;  
            nextState( setLocoFunction ) ;
            nextState( stopPlaying ) ; }

        State(setAccessory) {
            state = getEvent ; }

        State(setLocoSpeed) {
            state = getEvent ; }

        State(setLocoFunction) {
            state = getEvent ; }

        State(stopPlaying) {
            state = getEvent ; }

        break ;
    }
}
#undef State
