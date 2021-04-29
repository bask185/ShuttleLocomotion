#include "src/basics/timers.h"
#include "src/basics/io.h"
#include "src/modules/XpressNet.h"
#include "src/modules/Weistra.h"
#include "src/modules/debounceClass.h"
#include "src/modules/shortCircuit.h"
#include "recorder.h"

const int nChannels = 4 ;
Recorder recorder[] =
{
    Recorder( 0x50 ), 
    Recorder( 0x51 ), 
    Recorder( 0x52 ), 
    Recorder( 0x53 )
} ;
uint8_t channel = 0;

struct {
    uint8_t speed ;
    uint8_t function ;
    uint8_t accessory ;
    uint8_t locospeed ;
    uint8_t state ;
    uint8_t newSpeed ;
    uint8_t newFunction ;
    uint8_t newAccessory ;
    uint8_t newLocospeed ;
    uint8_t newState ;
} event[nChannels] ;

void clearEvent()
{
    event.speed = 0 ;
    event.function = 0 ;
    event.accessory = 0 ;
    event.locospeed = 0 ;
    event.state = 0 ;
}


// constructors
//Xnet
// Weista weistra( power ) ;
//shortCircuit.
Debounce on_off( onSwitch ) ;
Debounce record( recordSwitch ) ;


// SENSORS 
const inst nSensors = 4 ;
typedef struct {
    uint8_t pin : 4;
    uint8_t state  : 1;
    uint8_t statePrev : 1 ;
    uint8_t *timer ;
} Sensors ;
Sensors sens[ nSensors ] ;

void initSensors()
{
    sens[1].pin = sens1 ;
    sens[2].pin = sens2 ;
    sens[3].pin = sens3 ;
    sens[4].pin = sens4 ;
    
    sens[1].timer = &sens1T ;
    sens[2].timer = &sens2T ;
    sens[3].timer = &sens3T ;
    sens[4].timer = &sens4T ;
}

void readSensors()
{
    for( int i = 0 ; i < nSensors ; i++ )
    {
        if( digitalRead( sens[i].pin ) == LOW )
        {
            sens[i].timer = 200 ;
            sens[i].state = 1 ;
        }
        if( !sens[i].timer )
        {
            sens[i].state = 0 ;
        }
    }
}
////////////////////////////////

// READ SWITCHES
uint8_t on_off_state ;
uint8_t record_state ;

void readSwitches()
{
    REPEAT_MS( 20 ) ;
    on_off.debounceInputs() ;
    record.debounceInputs() ;
    END_REPEAT
    
    on_off_state = on_off.readInput() ;
    record_state = record.readInput() ;
}

// blink led code
uint8_t greenLedOffTime[]   = {100, 200 } ; // enter more number here is groups of 2
uint8_t greenLedOnTime[]    = {100, 200 } ;

uint8_t redLedOffTime[]     = {100, 200 } ;
uint8_t redLedOnTime[]      = {100, 200 } ;

void blinkLeds()
{
    if( !redLedT )
    {
        if( digitalRead( redLed ) )
        {
            digitalWrite( redLed, LOW ) ;
            redLedT = redLedOffTime[ blinkCodeR ] ;
        }
        else
        {
            if( blinkCodeR ) digitalWrite( redLed, HIGH ) ;
            redLedT = redLedOnTime[ blinkCodeR ] ;
        }
    }
    if( !greenLedT )
    {
        if( digitalRead( greenLedT ) )
        {
            digitalWrite( greenLedT, LOW ) ;
            greenLedT = greenLedOffTime[ blinkCodeG ] ;
        }
        else
        {
            if( blinkCodeG ) digitalWrite( redLed, HIGH ) ;
            greenLedT = greenLedOnTime[ blinkCodeG ] ;
        }
    }
}

void selectChannel()
{
    REPEAT_MS( 100 ) ;
    channel = analogRead( channelSelectPin ) / 0x100 ;
    END_REPEAT
}

void recordPrograms()
{
    uint8_t mode = recorder[ channel ].getMode() ;
    if( record_state == FALLING )              // IF BUTTON IS PRESSED
    {
        if(      mode == idle       ) recorder[ channel ].startRecording() ;
        else if( mode == recording  ) recorder[ channel ].stopRecording() ;
        else
        {
            // mode must be playing, so an error blink will suffice
        }
    }
    if( mode == recording )
    {
        if( event.sensor )       recorder[ channel ].SensorEvent(        event.sensor    , event.state ) ;
        if( event.locoSpeed )    recorder[ channel ].LocoSpeedEvent(     event.loco      , event.speed ) ;
        if( event.function )     recorder[ channel ].LocoFunctionEvent(  event.loco      , event.function | (event.state << 7) ) ;
        if( event.accessory )    recorder[ channel ].AccessoryEvent(     event.accessory , event.state ) ;
        
        clearEvent() ;
    }
}

void playPrograms()
{
    uint8_t mode = recorder[ channel ].getMode() ;
    if( on_off_state == FALLING )              // IF BUTTON IS PRESSED
    {
        if( mode == idle ) recorder[ channel ].StartPlaying() ;
        else
        {
            // recorder is either already playing or recording, error blink needed
        }
    }
    
    for( int i = 0 ; i < nChannels ; channel ++ )
    {
        if( recorder[i].getMode() == playing )
        {
            uint8_t arg1, arg2, arg3 ;
            event.new = recorder[ channel ].GetNextEvent( &arg1, &arg2, &arg3 ) ;
            
            switch( event[i].new )
            {
                case sensorEvent:       event[i].newSensor    = arg1 & 0x7F ; event[i].newState    = arg2 >> 7 ; break ;
                case locoSpeedEvent:    event[i].newLoco      = arg1 ;        event[i].newSpeed    = arg2 ;      break ;
                case locoFunctionEvent: event[i].newLoco      = arg1 ;        event[i].newFunction = arg2 ;      break ;
                case accessoryEvent:    event[i].newAccessory = arg1 ;        event[i].newState    = arg2 ;      break ;
                case timeExpireEvent:   event[i].newTime   = (arg3 << 16) | (arg2 << 8) | arg1 ;                 break ;
                case stopEvent:     /* to be filled in */                                                        break ;
            }
        }
    }
} 

// analog train controls
const int nSamples = 4 ;
void speedControl()
{
    if( speedKnobT == 0 )
    {   speedKnobT = 10 ;   // 10x per second
        
        static uint8_t counter   = 0 ;
        static uint8_t speedPrev = 0 ;
        uint16_t average = 0;
        
        int sample[ counter ] = analogRead( speedKnob ) ;
        if( ++counter == nSamples ) counter = 0 ;
        
        for( int i = 0 ; i < nSamples ; i ++ ) 
        {
            average += sample[ i ] ;
        }
        
        average /= nSamples ;
        speed = map( average, 0, 1023, -100, 100 ) ;
        
        if( speed < 0 ) { digitalWrite( dirLeft, HIGH ) ; digitalWrite( dirRight,  LOW ) ; }
        if( speed > 0 ) { digitalWrite( dirLeft,  LOW ) ; digitalWrite( dirRight, HIGH ) ; }
        
        if( speed != speedPrev )
        {   speedPrev  = speed ;
            weistra.setSpeed( speed ) ;
        }
    }
}




void setup()
{
    initTimers() ;
    initIO() ;
    initSensors() ;
    
    XpressNet.start(XNetAddress, 3);
    weistra.begin() ;
    // shortCircuit.begin() ;
}

void loop() {
// handle XpressNet Bus
    Xpressnet.receive() ;            // handles XpressNet bus communication
    // decodeCommands() ;                // process XpressNet commands to monitor updates for trains and turnouts.
    
// readInput switches
    // readSwitches() ;                    // reads and debounces switches as well as I2C inputs
    readSensors() ;                    // reads and debounces sensors
    
// run programs
    recordPrograms () ;                // record programs
    playPrograms() ;                    // play programs
    blinkLeds() ;
    
// analog train controls
    speedControl() ;                // reads and debounces speed for pwm
    weistra.update() ;                 // handles track PWM
    shortCircuit() ;                    // monitors current draw and regulates power pin

}