package com.lorenzodaneo.cnc.converter;

public enum GCodeEnum {
    G00("G00"),     // go fast
    G01("G01"),     // go slow
    G28("G28"),     // go to home
    G92("G92"),     // set home position
    M02("M2"),     // end command
    M114("M114"),   // print current position
    F("F"),
    NONE("");

    public final String value;

    GCodeEnum(String value){
        this.value = value;
    }

    public boolean isMovementCommand(){
        return this == G00 || this == G01 || this == G28;
    }

    public static GCodeEnum getEnum(String code){
        for(GCodeEnum codeEnum : GCodeEnum.values()){
            if(code.startsWith(codeEnum.value))
                return codeEnum;
        }
        return NONE;
    }

    public static boolean isValidGCode(String code){
        return getEnum(code) != NONE;
    }

}
