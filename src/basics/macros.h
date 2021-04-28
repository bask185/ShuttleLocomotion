// MACROS
#ifndef MACROS
#define MACROS

#define stateFunction(x) static bool x##F(void)
#define entryState if(runOnce) 
#define onState runOnce = false; if(!runOnce)
#define exitState if(!exitFlag) return false; else
#define State(x) break; case x: if(runOnce) Serial.println(#x); if(x##F())
#define STATE_MACHINE_BEGIN(x)  if(!enabled) { \
                                    if(!x##T) enabled = true ; } \
                                else switch(state){\
                                    default: debug("unknown state executed, state is idle now"); \
                                    state = x##IDLE; \
                                    case x##IDLE: \
                                    return true;
#define STATE_MACHINE_END break;}return false;

#define REPEAT_EVERY_MS(x)  { \
                            static uint32_t previousTime ;\
                                   uint32_t currentTime = millis() ;\
                            if( currentTime  - previousTime >= x ) {\
                                previousTime = currentTime ;
                                // code to be repeated goes between these 2 macros
#define END_REPEAT          }









#endif