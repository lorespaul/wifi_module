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

    static final int SCALE = 10;

    private static final BigDecimal REVOLUTION_STEPS = BigDecimal.valueOf(400);
    private static final BigDecimal MM_PER_REVOLUTION = BigDecimal.valueOf(8);
    private static final BigDecimal MIN_HALF_INTERVAL = BigDecimal.valueOf(312);
    private static final BigDecimal MIN_DISTANCE = BigDecimal.valueOf(0.02);
    private static final BigDecimal TWO = BigDecimal.valueOf(2);

    private final String axisId;
    private BigDecimal lastPosition = BigDecimal.valueOf(0.0);
    private BigDecimal nextPosition = null;

    private BigDecimal stepsToExecute;
    private Direction direction;

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

        BigDecimal halfStepInterval = speedMicros.setScale(SCALE, RoundingMode.HALF_EVEN).divide(stepsToExecute, RoundingMode.HALF_EVEN).divide(TWO, RoundingMode.HALF_EVEN);
        if(halfStepInterval.compareTo(MIN_HALF_INTERVAL) < 0){
            return RECALCULATES;
        }

        Direction direction = getDirection();
        this.stepsToExecute = stepsToExecute;
        this.direction = direction;
        return axisId + ",i" + halfStepInterval.longValue() + ",s" + stepsToExecute.longValue() + ",d" + direction.value;
    }


    void completeConversion(){
        if(this.nextPosition != null){
            checkConversionValidity();
//            this.lastPosition = this.nextPosition;
            BigDecimal distance = this.stepsToExecute.setScale(SCALE, RoundingMode.HALF_EVEN).multiply(MIN_DISTANCE);
            this.lastPosition = direction == Direction.Ahead ? this.lastPosition.add(distance) : this.lastPosition.subtract(distance);
        }
        this.nextPosition = null;
    }


    void putNextPosition(String axisValueString){
        axisValueString = axisValueString.replace(axisId, "");
        this.nextPosition = new BigDecimal(axisValueString).setScale(SCALE, RoundingMode.HALF_EVEN);
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


    private void checkConversionValidity(){
        // 400steps : 8mm = stepsToExecute : x
        BigDecimal testDistance = stepsToExecute.setScale(nextPosition.scale(), RoundingMode.HALF_EVEN).multiply(MIN_DISTANCE);
        BigDecimal testLastPosition = direction == Direction.Ahead ? this.nextPosition.subtract(testDistance).setScale(SCALE, RoundingMode.HALF_EVEN) : this.nextPosition.add(testDistance).setScale(SCALE, RoundingMode.HALF_EVEN);
        if(testLastPosition.compareTo(this.lastPosition) == 0)
            return;

        BigDecimal superiorMargin = lastPosition.setScale(SCALE, RoundingMode.HALF_EVEN).add(MIN_DISTANCE);
        BigDecimal inferiorMargin = lastPosition.setScale(SCALE, RoundingMode.HALF_EVEN).subtract(MIN_DISTANCE);
        if(testLastPosition.compareTo(superiorMargin) < 0 || testDistance.compareTo(inferiorMargin) > 0)
            return;

        System.out.println("Test position precision failed.");
    }


}
