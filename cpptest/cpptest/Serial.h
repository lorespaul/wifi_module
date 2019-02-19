//
//  Serial.hpp
//  cpptest
//
//  Created by Lorenzo Daneo on 18/02/2019.
//  Copyright © 2019 lore. All rights reserved.
//

#ifndef Serial_h
#define Serial_h

#include <iostream>

namespace serial {

    class FakeSerial {
        
        FakeSerial(){
            std::cout << "New FakeSerial" << std::endl;
        }
        ~FakeSerial(){
            std::cout << "Destroy FakeSerial" << std::endl;
        }
        
    public:
        static void println(std::string toPrint){
            std::cout << toPrint << std::endl;
        }
        
    };

}

extern serial::FakeSerial Serial;

#endif /* Serial_hpp */
