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

void Player::fetchArguments( uint8_t arguments )
{
    if( arguments )                                        // if atleast one argument, initiate the request
    {
        Wire.beginTransmission( I2Caddress ) ;
        Wire.write( eeAddress ) ;
        Wire.endTransmission() ;
        Wire.requestFrom( I2Caddress, arguments; ) ;
        
        if( arguments > 0 ) { retval[1] = Wire.read() ; eeAddress ++ ; } // always true...
        if( arguments > 1 ) { retval[2] = Wire.read() ; eeAddress ++ ; }
        if( arguments > 2 ) { retval[3] = Wire.read() ; eeAddress ++ ; }
    }
}

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
    case timeExpireEvent:                                   // time + time + time
        fetchArguments(3) ;
        newTime = ( retval[0]<<16 | retval[1]<<8 | retval[0] ) + millis() ;
        break ;

    case locoFunctionEvent:                                 // address + state|function
        fetchArguments(2) ;
        newLoco     = retval[ 0 ] ;
        newFunction = retval[ 1 ] & 0x7F ;
        newState    = retval[ 1 ] >> 7 ;
        break ;
 
    case locoSpeedEvent:                                    // address + speed
        fetchArguments(2) ;
        newLoco     = retval[ 0 ] ;
        newSpeed    = retval[ 1 ] ; // N.B. SPEED AND DIRECTION STEPS NEED TO BE SOLVED
        //newDir      = 
        break ;

    case accessoryEvent:    
        fetchArguments(2) ;
        newAccessory = retval[ 0 ] ;
        newState     = retval[ 1 ] ;
        break ;
            // address + state
    case sensorEvent:       
        fetchArguments(1) ;
        newSensor = retval[ 0 ] & 0x7F;
        newState  = retval[ 1 ] >> 7 ;
        break ;
            // state|sensor
    case stopEvent:         
        fetchArguments(0) ;
         
        break ;
            // N/A
    }
    
    
    return true ;
}

bool Player::waitTimeF()
{
    if( millis() >= newTime ) return true ;
    return false ;
}

bool Player::waitSensorF()
{
    if( sens[ newSensor ].state == newState ) return true ;
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
   eeAdress = 0 ;
   if( allowed2run ) return true ;
   else              return false ;
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



uint8_t Player::StartPlaying( )
{
    if( mode == idle )
    {
        eeAddress = 0 ;
        mode = playing ;
        return 1 ;
    }
    return 0 ;
}

uint8_t Player::StopPlaying( )
{
    return 1 ;
}