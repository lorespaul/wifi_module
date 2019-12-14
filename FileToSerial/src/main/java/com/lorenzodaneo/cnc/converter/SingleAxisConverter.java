package com.lorenzodaneo.cnc.converter;

import org.apache.log4j.Logger;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.List;

class SingleAxisConverter {

    private enum Direction {
        Back(1),
        Ahead(0);

        int value;

        Direction(int value){
            this.value = value;
        }
    }

    private enum ConvertedSectionEnum{
        Interval('i'),
        Steps('s'),
        Direction('d');

        char value;

        ConvertedSectionEnum(char value){
            this.value = value;
        }

        static String buildConvertedCommand(char axisId, long interval, long steps, Direction direction){
            return axisId + "," + Interval.value + interval + "," + Steps.value + steps + "," + Direction.value + direction.value;
        }
    }

    private static Logger logger = Logger.getLogger(ConverterManager.class);

    static final String RECALCULATES = "RECALCULATES";

    static final int SCALE = 10;

    private static final BigDecimal MM_PER_REVOLUTION = BigDecimal.valueOf(8);
    private static final BigDecimal REVOLUTION_STEPS = BigDecimal.valueOf(800);
    private static final BigDecimal MIN_HALF_INTERVAL = BigDecimal.valueOf(50); // quarter step mode
    private static final BigDecimal MIN_DISTANCE = BigDecimal.valueOf(0.01);
    private static final BigDecimal HALF_MIN_DISTANCE = BigDecimal.valueOf(0.005);
    private static final BigDecimal TWO = BigDecimal.valueOf(2);
    private static final long MAX_AXIS_LENGTH_STEPS = 90000;

    private final String axisId;
    private BigDecimal originalLastPosition = BigDecimal.valueOf(0.0);
    private BigDecimal lastPosition = BigDecimal.valueOf(0.0);
    private BigDecimal nextPosition = null;

    private BigDecimal incrementalStepsToExecute = BigDecimal.valueOf(0.0);
    private BigDecimal stepsToExecute;

    private boolean canBeInfinite;

    SingleAxisConverter(char axisId, boolean canBeInfinite){
        this.axisId = String.valueOf(axisId);
        this.canBeInfinite = canBeInfinite;
    }


    char getAxisId(){
        return axisId.charAt(0);
    }

    BigDecimal getLastPosition(){
        return lastPosition;
    }

    void setLastPosition(String axisValueString){
        axisValueString = axisValueString.replace(axisId, "");
        this.lastPosition = new BigDecimal(axisValueString).setScale(SCALE, RoundingMode.HALF_EVEN);
        this.originalLastPosition = new BigDecimal(this.lastPosition.toString());
    }

    String convert(BigDecimal speedMicros, List<BigDecimal> splitting, int splittingPosition, boolean infinite){
        if(infinite && canBeInfinite){
            this.incrementalStepsToExecute = BigDecimal.valueOf(0.0);
            this.originalLastPosition = BigDecimal.ZERO;
            this.lastPosition = BigDecimal.ZERO;
            this.nextPosition = null;
            return ConvertedSectionEnum.buildConvertedCommand(getAxisId(), 0, MAX_AXIS_LENGTH_STEPS, Direction.Back);
        }

        BigDecimal startToEndDistance = computeStartToEndDistance();
        if(startToEndDistance.compareTo(MIN_DISTANCE) < 0 || this.nextPosition == null){
            this.originalLastPosition = new BigDecimal(lastPosition.setScale(4, RoundingMode.HALF_EVEN).toString());
            this.incrementalStepsToExecute = BigDecimal.valueOf(0.0);
            this.nextPosition = null;
            return null;
        }

        BigDecimal proportionalDistance = computeProportionalDistance(startToEndDistance, splitting, splittingPosition);

        BigDecimal stepsToExecute = REVOLUTION_STEPS.divide(MM_PER_REVOLUTION, RoundingMode.HALF_EVEN).multiply(proportionalDistance).setScale(0, RoundingMode.HALF_DOWN);
        if(stepsToExecute.compareTo(BigDecimal.ONE) < 0){
            logger.warn(getAxisId() + ": No steps to do. (" + proportionalDistance.setScale(4, RoundingMode.HALF_EVEN).toString() + ")");
            this.originalLastPosition = new BigDecimal(lastPosition.setScale(4, RoundingMode.HALF_EVEN).toString());
            this.incrementalStepsToExecute = BigDecimal.valueOf(0.0);
            this.nextPosition = null;
            return null;
        }

        BigDecimal halfStepInterval = speedMicros.setScale(SCALE, RoundingMode.HALF_EVEN).divide(stepsToExecute, RoundingMode.HALF_EVEN).divide(TWO, RoundingMode.HALF_EVEN);
        if(halfStepInterval.compareTo(MIN_HALF_INTERVAL) < 0){
            return RECALCULATES;
        }

        this.stepsToExecute = stepsToExecute;
        Direction direction = getDirection();
        return ConvertedSectionEnum.buildConvertedCommand(getAxisId(), halfStepInterval.longValue(), stepsToExecute.longValue(), direction);
    }


    void completeConversion(List<BigDecimal> splitting, int splittingPosition){
        if(this.nextPosition != null){
            incrementalStepsToExecute = incrementalStepsToExecute.add(stepsToExecute);
            BigDecimal distance = this.stepsToExecute.setScale(SCALE, RoundingMode.HALF_EVEN).multiply(MIN_DISTANCE);
            this.lastPosition = getDirection() == Direction.Ahead ? this.lastPosition.add(distance) : this.lastPosition.subtract(distance);

            if(splittingPosition == splitting.size() - 1){
                checkConversionValidity();
                originalLastPosition = new BigDecimal(lastPosition.setScale(4, RoundingMode.HALF_EVEN).toString());
                incrementalStepsToExecute = BigDecimal.valueOf(0.0);
                this.nextPosition = null;
            }
        }
    }


    private BigDecimal computeProportionalDistance(BigDecimal startToEndDistance, List<BigDecimal> splitting, int splittingPosition){
        if(splittingPosition == splitting.size() - 1) // at last position use startToEndDistance to not lose precision
            return startToEndDistance;
        BigDecimal remainingHypotenuseDistance = BigDecimal.valueOf(0.0);
        for(int i = splittingPosition; i < splitting.size(); i++){
            remainingHypotenuseDistance = remainingHypotenuseDistance.add(splitting.get(i));
        }
        // remainingHypotenuseDistance : startToEndDistance = splitting.get(i) : x
        BigDecimal result = startToEndDistance.multiply(splitting.get(splittingPosition)).divide(remainingHypotenuseDistance, RoundingMode.HALF_EVEN).setScale(SCALE, RoundingMode.HALF_EVEN);
        if(result.compareTo(MIN_DISTANCE) < 0)
            return startToEndDistance;
        return result;
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
        if(nextPosition.compareTo(originalLastPosition) >= 0)
            return Direction.Ahead;
        return Direction.Back;
    }


    private void checkConversionValidity(){
        // 400steps : 8mm = stepsToExecute : x
        BigDecimal testDistance = incrementalStepsToExecute.multiply(MIN_DISTANCE);
        BigDecimal testLastPosition = getDirection() == Direction.Ahead ? this.nextPosition.subtract(testDistance).setScale(SCALE, RoundingMode.HALF_EVEN) : this.nextPosition.add(testDistance).setScale(SCALE, RoundingMode.HALF_EVEN);
        if(testLastPosition.compareTo(originalLastPosition) == 0)
            return;

//        BigDecimal superiorMargin = originalLastPosition.setScale(SCALE, RoundingMode.HALF_EVEN).add(HALF_MIN_DISTANCE);
//        BigDecimal inferiorMargin = originalLastPosition.setScale(SCALE, RoundingMode.HALF_EVEN).subtract(HALF_MIN_DISTANCE);
//        if(testLastPosition.compareTo(superiorMargin) <= 0 || testDistance.compareTo(inferiorMargin) >= 0)
//            return;

        System.out.println("Test position precision failed.");
    }


}
