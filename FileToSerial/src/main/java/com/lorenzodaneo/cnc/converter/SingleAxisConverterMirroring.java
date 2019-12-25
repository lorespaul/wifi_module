package com.lorenzodaneo.cnc.converter;

import java.math.BigDecimal;
import java.util.List;

public class SingleAxisConverterMirroring {

    private SingleAxisConverter preComputingAxis;
    private SingleAxisConverter converter;


    SingleAxisConverterMirroring(char axisId, boolean canBeInfinite){
        this.preComputingAxis = new SingleAxisConverter(axisId, canBeInfinite);
        this.converter = new SingleAxisConverter(axisId, canBeInfinite);
    }


    SingleAxisConverter getPreComputingAxis(){
        return this.preComputingAxis;
    }


    SingleAxisConverter getConverter(){
        return this.converter;
    }


    char getAxisId(){
        return this.converter.getAxisId();
    }

    BigDecimal getLastPosition(){
        return this.converter.getLastPosition();
    }

    void setLastPosition(String axisValueString){
        setConverterLastPosition(axisValueString);
        setMirroringLastPosition(axisValueString);
    }

    void setConverterLastPosition(String axisValueString){
        this.converter.setLastPosition(axisValueString);
    }

    void setMirroringLastPosition(String axisValueString){
        this.preComputingAxis.setLastPosition(axisValueString);
    }


    void putConverterNextPosition(String axisValueString){
        this.converter.putNextPosition(axisValueString);
    }

    void putMirroringNextPosition(String axisValueString){
        this.preComputingAxis.putNextPosition(axisValueString);
    }

//    void completeOnMirroring(){
//        String nextPosition = this.preComputingAxis.getNextPosition();
//        if(nextPosition != null)
//            this.preComputingAxis.setLastPosition(nextPosition);
//    }

    boolean checkAlignment(){
        return this.preComputingAxis.getLastPosition().compareTo(this.converter.getLastPosition()) == 0;
    }

    // return axisSpeed
    BigDecimal preComputing(BigDecimal deltaTMicros, List<BigDecimal> splitting, int splittingPosition, boolean infinite){
        String result = this.preComputingAxis.convert(deltaTMicros, splitting, splittingPosition, infinite);
        if(result == null || this.preComputingAxis.getNextPosition() == null)
            return BigDecimal.ZERO;
        BigDecimal speed = this.preComputingAxis.getSpeed();
        if(speed.compareTo(BigDecimal.valueOf(-1)) == 0)
            return BigDecimal.ZERO;
        if(this.preComputingAxis.getDirection() == SingleAxisConverter.Direction.Back)
            speed = speed.negate();
        return speed;
    }

    String convert(BigDecimal deltaTMicros, List<BigDecimal> splitting, int splittingPosition, boolean infinite){
        if(infinite && this.preComputingAxis.canBeInfinite()){
            this.preComputingAxis.setLastPosition("0.0");
        }
        return this.converter.convert(deltaTMicros, splitting, splittingPosition, infinite);
    }

}
