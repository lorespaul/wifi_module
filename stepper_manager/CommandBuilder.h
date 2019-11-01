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

#define GG 'G'    // imposta tipo comando
#define MM 'M'    // imposta tipo comando
#define FF 'F'    // imposta velocità (mm/sec)
#define XX 'X'    // coordinata asse x
#define YY 'Y'    // coordinata asse y
#define ZZ 'Z'    // coordinata asse z
#define II 'I'    // cerchio -> offset dal centro rispetto al punto di partenza su asse x
#define JJ 'J'    // cerchio -> offset dal centro rispetto al punto di partenza su asse y

#define G00 "G00" // movimento veloce
#define G01 "G01" // movimento lento o a velocità prestabilita (vedi fMode 'F')
#define G02 "G02" // movimento insenso orario
#define G03 "G03" // movimento insenso antiorario
#define G28 "G28" // vai alla posizione home
#define M09 "M09" // vai alla posizione home
#define M10 "M10" // vai alla posizione home

#define F_DEFAULT 20
#define MODE_CHARS 3

#define GO_AHEAD LOW
#define GO_BACK HIGH

#define STD_F_FAST 60.00  // mm/sec
#define STD_F_SLOW 30.00  // mm/sec
#define STD_F_HOME 40.00  // mm/sec


namespace stepper_motor {
    
    class CommandBuilder {

        private:
            char gMode[MODE_CHARS];
            char mMode[MODE_CHARS];
            float fMode;
            double xLastPos;
            double yLastPos;
            double zLastPos;
            double xPos;
            double yPos;
            double zPos;
            double iOffset;
            double jOffset;
            void prepareContext(char stringCommand[]);
            bool modeEq(const char mode[], const char comparison[]);

            int computeSpeedMillis(float mmPerSec, double millimeters);
            
            double computeStartEndPosDistanceLinear(double startPos, double endPos);
            int computeSpeedMillisLinear(float mmPerSec, double xEndPos, double yEndPos, double zEndPos);
            void buildSingleLinear(StepperCommand &command, double *startPos, double endPos, int speedMillis);

            double computeStartEndPosDistanceCircularProjection(double startPos, double endPos, double offset, bool clockwise, bool startUp, bool endUp);
            int computeSpeedMillisCircular(float mmPerSec, double xEndPos, double yEndPos, bool clockwise, bool yStartUp, bool yEndUp);
            void buildSingleCircular(StepperCommand &command, double *startPos, double endPos, double offset, bool clockwise, bool startUp, bool endUp, int speedMillis);

            double computeRadius();
            
        public:
            CommandBuilder();
            ~CommandBuilder();
            int build(char stringCommand[], StepperCommand &xCommand, StepperCommand &yCommand, StepperCommand &zCommand);
        
    };
    
}

#endif /* CommandBuilder_h */
