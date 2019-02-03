//
//  cpplink.cpp
//  cpptest
//
//  Created by Lorenzo Daneo on 02/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <string>
#include "cpplink.h"
#include "interface.h"

using namespace std;

bool test(){
    _charAt((char*)"ciao", 2);
    testStatic[0] = 'm';
    testStatic[1] = 'a';
    testStatic[2] = 'i';
    testStatic[3] = 'o';
    return true;
    //return check();
}

void referenceTest(string& output){
    //printf("%s", output);
    cout << output << endl;
}
