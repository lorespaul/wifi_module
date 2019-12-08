package com.lorenzodaneo.cnc.listeners;

import java.awt.event.KeyEvent;

public enum KeyEnum {
    ArrowLeft(37),
    ArrowTop(38),
    ArrowRight(39),
    ArrowDown(40),
    ZChar(90);

    public int value;

    KeyEnum(int value){
        this.value = value;
    }

    public static KeyEnum getEnum(int value){
        for(KeyEnum keyEnum : KeyEnum.values()){
            if(keyEnum.value == value)
                return keyEnum;
        }
        return null;
    }

}
