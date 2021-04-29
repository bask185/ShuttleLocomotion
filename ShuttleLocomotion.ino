#include "src/basics/timers.h"
#include "src/basics/io.h"
#include "src/modules/XpressNet.h"
#include "src/modules/Weistra.h"
#include "src/modules/debounceClass.h"
#include "src/modules/shortCircuit.h"



// constructors
//Xnet
// Weista weistra( power ) ;
//shortCircuit.
Debounce on_off( onSwitch ) ;
Debounce record( recordSwitch ) ;

int8_t speed ;


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
    
    on_off_state = on_off.read() ;
    record_state = record.read() ;
}

// blink led code
uint8_t greenLedOffTime[]   = {100, 200, } ;
uint8_t greenLedOnTime[]    = {100, 200, } ;

uint8_t redLedOffTime[]     = {100, 200, } ;
uint8_t redLedOnTime[]      = {100, 200, } ;

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

void recordPrograms()
{
}
void playPrograms()
{
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
    // Xpressnet.init() ;
    // weistra.begin() ;
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