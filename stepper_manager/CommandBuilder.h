//
//  CommandBuilder.h
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef CommandBuilder_h
#define CommandBuilder_h

#include <Arduino.h>
#include "StepperCommand.h"

#define SPACE " "

#define G 'G'    // imposta tipo comando
#define F 'F'    // imposta velocità (mm/sec)
#define X 'X'    // coordinata asse x
#define Y 'Y'    // coordinata asse y
#define Z 'Z'    // coordinata asse z
#define I 'I'    // cerchio -> offset dal centro rispetto al punto di partenza su asse x
#define J 'J'    // cerchio -> offset dal centro rispetto al punto di partenza su asse y

#define G00 "00" // movimento veloce
#define G01 "01" // movimento lento o a velocità prestabilita (vedi fMode 'F')
#define G02 "02" // movimento insenso orario
#define G03 "03" // movimento insenso antiorario
#define G28 "28" // vai alla posizione home

#define F_DEFAULT 20
#define G_MODE_CHARS 2

#define GO_AHEAD LOW
#define GO_BACK HIGH

#define STD_F_FAST 60.00  // mm/sec
#define STD_F_SLOW 30.00  // mm/sec


namespace stepper_motor {
    
    class CommandBuilder {

        private:
            char gMode[G_MODE_CHARS];
            float fMode;
            int xLastPos;
            int yLastPos;
            int zLastPos;
            int xPos;
            int yPos;
            int zPos;
            int iOffset;
            int jOffset;
            void prepareContext(char stringCommand[]);
            bool gModeEq(char comparison[]);
            
            int computeStartEndPosDistanceLinear(int startPos, int endPos);
            int computeSpeedMillisLinear(float mmPerSec, int xEndPos, int yEndPos, int zEndPos);
            void buildSingleLinear(StepperCommand &command, int *startPos, int endPos, int speedMillis);

            int computeStartEndPosDistanceCircularProjection(int startPos, int endPos, int offset, bool clockwise, bool startUp, bool endUp);
            int computeSpeedMillisCircular(float mmPerSec, int xEndPos, int yEndPos, bool clockwise, bool yStartUp, bool yEndUp);
            void buildSingleCircular(StepperCommand &command, int *startPos, int endPos, int offset, bool clockwise, bool startUp, bool endUp, int speedMillis);

            double computeRadius();
            
        public:
            CommandBuilder();
            ~CommandBuilder();
            int build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand);
        
    };
    
}

#endif /* CommandBuilder_h */
