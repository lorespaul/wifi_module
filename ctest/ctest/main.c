//
//  main.c
//  ctest
//
//  Created by Lorenzo Daneo on 31/01/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <stdio.h>
#include "interface.h"

char _charAt(char* string, int index){
    return *(string + index);
}

int main(int argc, const char * argv[]) {
    char test[] = "testone";
    _charAt(test, 3);
//    while(1){
//        loop();
//    }
}


void loop(){
    printf("ciao");
}
