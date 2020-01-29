// -----------------------------------------------------------------------------
// fsm example - MPF 01/2020
// -----------------------------------------------------------------------------

// includes
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "fsm.h"


// enumerate states
enum states_enum {
    STATE_STATE1,
    STATE_STATE2,
    STATE_STATE3,
    STATE_STOP,
};

// fsm structure
fsm_t fsm;
#define FSM_STRCT fsm

// prototypes
void fsm_init(void);
void fsm_step(void);

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int i,j;
int stop;
int main(void){
    stop=0;
    fsm_init();
    for (j=0; j<50; j++) {
        if (j==40) stop=1;
        fsm_step();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// sm init
// -----------------------------------------------------------------------------
void fsm_init(void){
    FSM_INIT(STATE_STATE1);
}

// -----------------------------------------------------------------------------
// sm step
// -----------------------------------------------------------------------------
void fsm_step(void){

    BEGIN_EVENT(stop,STATE_STOP);
        // stop all
        printf("Stop all\n");
    END_EVENT;
    
    BEGIN_FSM;

        BEGIN_STATE(STATE_STATE1);

            BEGIN_ENTRY;
                // do nothing
                printf("Entry STATE_STATE1 i=%u\n",i);
            END_ENTRY;

            BEGIN_DURING;
                // increment i
                i++;
                printf("During STATE_STATE1 i=%u\n",i);
            END_DURING;
            
            BEGIN_EVENT(i==4,STATE_STATE2);
                // state 1 event statements
                printf("Event STATE_STATE1 i=%u\n",i);
            END_EVENT;
            
            BEGIN_EXIT;
                // set i = 0
                printf("Exit STATE_STATE1 i=%u\n",i);
                i = 0;
            END_EXIT;

        END_STATE;

        BEGIN_STATE(STATE_STATE2);
            
            BEGIN_ENTRY;
                // do nothing
                printf("Entry STATE_STATE2 i=%u\n",i);
            END_ENTRY;
            
            BEGIN_DURING;
                // increment i
                i++;
                printf("During STATE_STATE2 i=%u\n",i);
            END_DURING;
            
            BEGIN_EVENT(i==5,STATE_STATE3);
                // state 2 event 1 statements
                printf("Event STATE_STATE2 i=%u\n",i);
            END_EVENT;

            BEGIN_EXIT;
                // set i = 0
                i=0;
                printf("Exit STATE_STATE2 i=%u\n",i);
            END_EXIT;

        END_STATE;

        BEGIN_STATE(STATE_STATE3);
            
            BEGIN_ENTRY;
                // do nothing
                printf("Entry STATE_STATE3 i=%u\n",i);
            END_ENTRY;
            
            BEGIN_DURING;
                // increment i
                i++;
                printf("During STATE_STATE3 i=%u\n",i);
            END_DURING;
            
            BEGIN_EVENT(i==6,STATE_STATE1);
                // state 3 event 1 statements
                printf("Event STATE_STATE3 i=%u\n",i);
            END_EVENT;
            
            BEGIN_EXIT;
                // set i = 0
                i=0;
                printf("Exit STATE_STATE3 i=%u\n",i);
            END_EXIT;

        END_STATE;

        BEGIN_STATE(STATE_STOP);
            BEGIN_ENTRY;
                // set stop = 0
                stop = 0;
            END_ENTRY;
            BEGIN_EVENT(start,STATE_STATE1);
                // restart the machine
                printf("Restarting the machine\n");
            END_EVENT;
        END_STATE;

    END_FSM;
}