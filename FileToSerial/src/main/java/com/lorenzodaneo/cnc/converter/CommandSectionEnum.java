package com.lorenzodaneo.cnc.converter;

public enum CommandSectionEnum {
    GCommand('G'),
    FCommand('F'),
    XAxis('X'),
    YAxis('Y'),
    ZAxis('Z'),
    None(' ');

    public final char value;

    CommandSectionEnum(char value){
        this.value = value;
    }

    static CommandSectionEnum getEnum(char value){
        for(CommandSectionEnum sectionEnum : CommandSectionEnum.values()){
            if(sectionEnum.value == value)
                return sectionEnum;
        }
        return None;
    }
}
