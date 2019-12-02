package com.lorenzodaneo.cnc.converter;

import java.math.BigDecimal;
import java.math.RoundingMode;

class SingleAxisConverter {

    private enum Direction {
        Back(1),
        Forward(0);

        int value;

        Direction(int value){
            this.value = value;
        }
    }

    private final static BigDecimal REVOLUTION_STEPS = BigDecimal.valueOf(400);
    private final static BigDecimal MM_PER_REVOLUTION = BigDecimal.valueOf(8);
    private final static BigDecimal MIN_HALF_INTERVAL = BigDecimal.valueOf(310);
    private final static BigDecimal MIN_DISTANCE = BigDecimal.valueOf(0.02);
    private final static BigDecimal TWO = BigDecimal.valueOf(2);

    private final String axisId;
    private BigDecimal lastPosition = BigDecimal.ZERO;
    private BigDecimal nextPosition = null;

    SingleAxisConverter(char axisId){
        this.axisId = String.valueOf(axisId);
    }


    char getAxisId(){
        return axisId.charAt(0);
    }



    String convert(BigDecimal speedMicros){
        BigDecimal startToEndDistance = computeStartToEndDistance();
        if(startToEndDistance.compareTo(MIN_DISTANCE) < 0 || this.nextPosition == null){
            this.nextPosition = null;
            return null;
        }

        BigDecimal stepsToExecute = REVOLUTION_STEPS.divide(MM_PER_REVOLUTION, RoundingMode.HALF_EVEN).multiply(startToEndDistance).setScale(0, RoundingMode.HALF_DOWN);

        BigDecimal temp = this.nextPosition;
        this.nextPosition = null;
        if(stepsToExecute.equals(BigDecimal.ZERO))
            return null;

        BigDecimal halfStepInterval = speedMicros.divide(stepsToExecute, RoundingMode.HALF_EVEN).divide(TWO, RoundingMode.HALF_EVEN);
        if(halfStepInterval.compareTo(MIN_HALF_INTERVAL) < 0){
            System.out.println("Warning: half step interval was too little.");
            halfStepInterval = MIN_HALF_INTERVAL;
        }

        Direction direction = getDirection(temp);
        this.lastPosition = temp;
        return axisId + ",i" + halfStepInterval.longValue() + ",s" + stepsToExecute.intValue() + ",d" + direction.value;
    }



    void putNextPosition(String axisValueString){
        axisValueString = axisValueString.replace(axisId, "");
        this.nextPosition = new BigDecimal(axisValueString);
    }


    BigDecimal computeStartToEndDistance(){
        if(nextPosition == null)
            return BigDecimal.ZERO;
        return nextPosition.subtract(lastPosition).abs();
    }


    private Direction getDirection(BigDecimal nextPosition){
        if(nextPosition.compareTo(lastPosition) >= 0)
            return Direction.Forward;
        return Direction.Back;
    }


}
