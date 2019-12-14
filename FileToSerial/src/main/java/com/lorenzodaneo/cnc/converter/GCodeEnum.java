package com.lorenzodaneo.cnc.converter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public enum GCodeEnum {
    G01("G01", "G1"),     // go slow
    G00("G00", "G0"),     // go fast
    G28("G28"),     // go to home
    G92("G92"),     // set home position
    M02("M02", "M2"),     // end command
    M114("M114"),   // print current position
    F("F"),
    NONE("");

    public final List<String> value;

    GCodeEnum(String... value){
        this.value = Arrays.asList(value);
    }

    public boolean isMovementCommand(){
        return this == G00 || this == G01 || this == G28;
    }

    public static GCodeEnum getEnum(String code){
        code = code.split(" ")[0];
        for(GCodeEnum codeEnum : GCodeEnum.values()){
            for(String enumValueInList : codeEnum.value){
                if(code.equals(enumValueInList))
                    return codeEnum;
            }
        }
        return NONE;
    }

    public static boolean isValidGCode(String code){
        return getEnum(code) != NONE;
    }

}
