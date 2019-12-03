package com.lorenzodaneo.cnc.converter;

import java.math.BigDecimal;
import java.math.RoundingMode;

class SingleAxisConverter {

    private enum Direction {
        Back(1),
        Ahead(0);

        int value;

        Direction(int value){
            this.value = value;
        }
    }

    static final String RECALCULATES = "RECALCULATES";

    private static final BigDecimal REVOLUTION_STEPS = BigDecimal.valueOf(400);
    private static final BigDecimal MM_PER_REVOLUTION = BigDecimal.valueOf(8);
    private static final BigDecimal MIN_HALF_INTERVAL = BigDecimal.valueOf(310);
    private static final BigDecimal MIN_DISTANCE = BigDecimal.valueOf(0.02);
    private static final BigDecimal TWO = BigDecimal.valueOf(2);

    private final String axisId;
    private BigDecimal lastPosition = BigDecimal.ZERO;
    private BigDecimal nextPosition = null;

    SingleAxisConverter(char axisId){
        this.axisId = String.valueOf(axisId);
    }


    char getAxisId(){
        return axisId.charAt(0);
    }



    String convert(BigDecimal speedMicros, boolean infinite){
        if(infinite){
            this.lastPosition = BigDecimal.ZERO;
            this.nextPosition = null;
            return axisId + ",i0,s" + REVOLUTION_STEPS.multiply(REVOLUTION_STEPS) + ",d" + Direction.Back.value;
        }

        BigDecimal startToEndDistance = computeStartToEndDistance();
        if(startToEndDistance.compareTo(MIN_DISTANCE) < 0 || this.nextPosition == null){
            this.nextPosition = null;
            return null;
        }

        BigDecimal stepsToExecute = REVOLUTION_STEPS.divide(MM_PER_REVOLUTION, RoundingMode.HALF_EVEN).multiply(startToEndDistance).setScale(0, RoundingMode.HALF_DOWN);
        if(stepsToExecute.compareTo(BigDecimal.ONE) < 0){
            System.out.println("No steps to do.");
            this.nextPosition = null;
            return null;
        }

        BigDecimal halfStepInterval = speedMicros.setScale(100, RoundingMode.HALF_EVEN).divide(stepsToExecute, RoundingMode.HALF_EVEN).divide(TWO, RoundingMode.HALF_EVEN);
        if(halfStepInterval.compareTo(MIN_HALF_INTERVAL) < 0){
            return RECALCULATES;
        }

        Direction direction = getDirection();
        return axisId + ",i" + halfStepInterval.longValue() + ",s" + stepsToExecute.longValue() + ",d" + direction.value;
    }


    void completeConversion(){
        if(this.nextPosition != null)
            this.lastPosition = this.nextPosition;
        this.nextPosition = null;
    }


    void putNextPosition(String axisValueString){
        axisValueString = axisValueString.replace(axisId, "");
        this.nextPosition = new BigDecimal(axisValueString).setScale(100, RoundingMode.HALF_EVEN);
    }


    BigDecimal computeStartToEndDistance(){
        if(nextPosition == null)
            return BigDecimal.ZERO;
        return nextPosition.subtract(lastPosition).abs();
    }


    private Direction getDirection(){
        if(nextPosition.compareTo(lastPosition) >= 0)
            return Direction.Ahead;
        return Direction.Back;
    }


}
