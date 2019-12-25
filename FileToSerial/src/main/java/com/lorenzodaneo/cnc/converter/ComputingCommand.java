package com.lorenzodaneo.cnc.converter;

import com.lorenzodaneo.cnc.physics.PhysicalVector;
import com.lorenzodaneo.cnc.utils.BigDecimalUtils;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.*;

public class ComputingCommand {

    private static final BigDecimal MAX_POINT_TO_POINT_DISTANCE = BigDecimal.valueOf(0.1);
    private static final Map<Character, String> closingSection = new HashMap<>();

    static {
        closingSection.put(CommandSectionEnum.XAxis.value, "X0.0");
        closingSection.put(CommandSectionEnum.YAxis.value, "Y0.0");
        closingSection.put(CommandSectionEnum.ZAxis.value, "Z0.0");
    }

    private String command;// = UUID.randomUUID().toString();

    private GCodeEnum commandType;
//    private BigDecimal linearDistance;
    private LinkedList<BigDecimal> splitLinearDistance;
    private LinkedList<BigDecimal> mmPerSecSpeeds;
    private BigDecimal flatSpeed;
    private Map<Character, String> sections;

    List<PhysicalVector> vectors;
    PhysicalVector vectorAverage = null;

    ComputingCommand(String command, GCodeEnum commandType, BigDecimal linearDistance, BigDecimal previousMmPerSecSpeed, BigDecimal mmPerSecSpeed, Map<Character, String> sections){
        this.command = command;
        this.commandType = commandType;
//        this.linearDistance = linearDistance;
        this.flatSpeed = mmPerSecSpeed;
        this.sections = sections;
        this.vectors = new ArrayList<>();
        this.splitLinearDistance = computeSplitLinearDistance(linearDistance);
        preComputeSpeedBasedOnPrevious(previousMmPerSecSpeed, mmPerSecSpeed);
    }

    public static ComputingCommand getClosingCommand(){
        return new ComputingCommand(GCodeEnum.M02.value.get(0), GCodeEnum.M02, BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO, closingSection);
    }

    public BigDecimal getFlatSpeed(){
        return flatSpeed;
    }

    public String getCommand(){
        return command;
    }

    public GCodeEnum getCommandType(){
        return this.commandType;
    }

    public List<BigDecimal> getSplitLinearDistance(){
        return this.splitLinearDistance;
    }

    public BigDecimal getMmPerSecSpeed(int splittingPosition){
        if(commandType == GCodeEnum.G28)
            return BigDecimal.valueOf(40);
        return this.mmPerSecSpeeds.get(splittingPosition);
    }

    public BigDecimal getFirstMmPerSecSpeed(){
        return this.mmPerSecSpeeds.getFirst();
    }

    public BigDecimal getLastMmPerSecSpeed(){
        return this.mmPerSecSpeeds.getLast();
    }

//    public void setFirstMmPerSecSpeed(BigDecimal firstMmPerSecSpeed){
//        this.mmPerSecSpeeds.removeFirst();
//        this.mmPerSecSpeeds.addFirst(firstMmPerSecSpeed);
//    }
//
//    public void setLastMmPerSecSpeed(BigDecimal lastMmPerSecSpeed){
//        this.mmPerSecSpeeds.removeLast();
//        this.mmPerSecSpeeds.addLast(lastMmPerSecSpeed);
//    }
//
//    public void setMmPerSecSpeed(BigDecimal mmPerSecSpeed){
//        this.mmPerSecSpeed = mmPerSecSpeed;
//    }

    public void addVector(PhysicalVector vector){
        vectors.add(vector);
    }

//    public BigDecimal getJunctionSpeed(){
//        return this.junctionManagement.junctionSpeed;
//    }

    private LinkedList<BigDecimal> computeSplitLinearDistance(BigDecimal linearDistance){
       LinkedList<BigDecimal> splitLinearDistance = new LinkedList<>();
//        if(linearDistance.compareTo(MAX_POINT_TO_POINT_DISTANCE) > 0){
        BigDecimal[] splitting = linearDistance.divideAndRemainder(MAX_POINT_TO_POINT_DISTANCE);
        for(BigDecimal i = BigDecimal.ZERO; i.compareTo(splitting[0]) < 0; i = i.add(BigDecimal.ONE)){
            splitLinearDistance.add(MAX_POINT_TO_POINT_DISTANCE);
        }
        splitLinearDistance.add(splitting[1]);
        return splitLinearDistance;
//        } else {
//            this.splitLinearDistance.add(linearDistance);
//        }
    }

    void preComputeSpeedBasedOnPrevious(BigDecimal previousMmPerSecSpeed, BigDecimal mmPerSecTargetSpeed){
        BigDecimal nextSpeed;
        LinkedList<BigDecimal> speeds = new LinkedList<>();
        if(mmPerSecTargetSpeed.compareTo(previousMmPerSecSpeed) != 0){
            for(BigDecimal currentDistance : splitLinearDistance){

                BigDecimal speedAfterAcceleration = BigDecimal.valueOf(2).multiply(ConverterManager.ACCELERATION).multiply(currentDistance);
                if(mmPerSecTargetSpeed.compareTo(previousMmPerSecSpeed) >= 0){
                    nextSpeed = BigDecimalUtils.bigSqrt(previousMmPerSecSpeed.pow(2).add(speedAfterAcceleration));
                    if(nextSpeed.compareTo(mmPerSecTargetSpeed) > 0)
                        nextSpeed = mmPerSecTargetSpeed;
                } else {
                    if(speedAfterAcceleration.compareTo(previousMmPerSecSpeed.pow(2)) > 0)
                        nextSpeed = BigDecimal.valueOf(0.1);
                    else
                        nextSpeed = BigDecimalUtils.bigSqrt(previousMmPerSecSpeed.pow(2).subtract(speedAfterAcceleration));
                }
                nextSpeed = nextSpeed.setScale(20, RoundingMode.HALF_EVEN);
                speeds.add(nextSpeed);
                previousMmPerSecSpeed = nextSpeed;
            }
        } else {
            for(int i = 0; i < splitLinearDistance.size(); i++){
                speeds.add(mmPerSecTargetSpeed);
            }
        }
        this.mmPerSecSpeeds = speeds;
    }

    boolean computeSpeedBasedOnPrevious(BigDecimal previousMmPerSecSpeed, BigDecimal mmPerSecTargetSpeed){
        BigDecimal nextSpeed;
//        LinkedList<BigDecimal> speeds = new LinkedList<>();
        boolean computingFinished = false;
        if(mmPerSecTargetSpeed.compareTo(previousMmPerSecSpeed) != 0){
            for(int i = 0; i < splitLinearDistance.size(); i++){
//                if(!computingFinished){
                BigDecimal currentDistance = splitLinearDistance.get(i);

                BigDecimal speedAfterAcceleration = BigDecimal.valueOf(2).multiply(ConverterManager.ACCELERATION).multiply(currentDistance);
                if(mmPerSecTargetSpeed.compareTo(previousMmPerSecSpeed) >= 0){
                    nextSpeed = BigDecimalUtils.bigSqrt(previousMmPerSecSpeed.pow(2).add(speedAfterAcceleration));
                    if(nextSpeed.compareTo(mmPerSecTargetSpeed) > 0)
                        nextSpeed = mmPerSecTargetSpeed;
                } else {
                    if(speedAfterAcceleration.compareTo(previousMmPerSecSpeed.pow(2)) > 0)
                        nextSpeed = BigDecimal.valueOf(0.1);
                    else
                        nextSpeed = BigDecimalUtils.bigSqrt(previousMmPerSecSpeed.pow(2).subtract(speedAfterAcceleration));
                }
                nextSpeed = nextSpeed.setScale(20, RoundingMode.HALF_EVEN);
                BigDecimal presentSpeed = this.mmPerSecSpeeds.get(i);
                this.mmPerSecSpeeds.remove(i);
                this.mmPerSecSpeeds.add(i, nextSpeed);
                previousMmPerSecSpeed = nextSpeed;
//                }
                if(presentSpeed.compareTo(nextSpeed) <= 0){
//                    if(!computingFinished)
//                        speeds.removeFirst();
//                    speeds.addFirst(presentSpeed);
                    computingFinished = true;
                    break;
                }
            }
        } else {
            computingFinished = true;
            this.mmPerSecSpeeds = new LinkedList<>();
            for(int i = 0; i < splitLinearDistance.size(); i++){
                this.mmPerSecSpeeds.add(mmPerSecTargetSpeed);
            }
        }
        return computingFinished;
    }


    boolean computeSpeedBasedOnNext(BigDecimal nextMmPerSecSpeed, BigDecimal mmPerSecTargetSpeed){
        BigDecimal previousSpeed;
//        LinkedList<BigDecimal> speeds = new LinkedList<>();
        boolean computingFinished = false;
        if(mmPerSecTargetSpeed.compareTo(nextMmPerSecSpeed) != 0){
            for(int i = splitLinearDistance.size() - 1; i >= 0; i--){
//                if(!computingFinished){
                BigDecimal currentDistance = splitLinearDistance.get(i);

                BigDecimal speedAfterAcceleration = BigDecimal.valueOf(2).multiply(ConverterManager.ACCELERATION).multiply(currentDistance);
                if(mmPerSecTargetSpeed.compareTo(nextMmPerSecSpeed) >= 0){
                    previousSpeed = BigDecimalUtils.bigSqrt(nextMmPerSecSpeed.pow(2).add(speedAfterAcceleration));
                    if(previousSpeed.compareTo(mmPerSecTargetSpeed) > 0)
                        previousSpeed = mmPerSecTargetSpeed;
                } else {
                    if(speedAfterAcceleration.compareTo(nextMmPerSecSpeed.pow(2)) > 0)
                        previousSpeed = BigDecimal.valueOf(0.1);
                    else
                        previousSpeed = BigDecimalUtils.bigSqrt(nextMmPerSecSpeed.pow(2).subtract(speedAfterAcceleration));
                }
                previousSpeed = previousSpeed.setScale(20, RoundingMode.HALF_EVEN);
                BigDecimal presentSpeed = this.mmPerSecSpeeds.get(i);
                this.mmPerSecSpeeds.remove(i);
                this.mmPerSecSpeeds.add(i, previousSpeed);
                nextMmPerSecSpeed = previousSpeed;
//                }
                if(presentSpeed.compareTo(previousSpeed) <= 0){
//                    if(!computingFinished)
//                        speeds.removeFirst();
//                    speeds.addFirst(presentSpeed);
                    computingFinished = true;
                    break;
                }
            }
        } else {
            computingFinished = true;
            this.mmPerSecSpeeds = new LinkedList<>();
            for(int i = 0; i < splitLinearDistance.size(); i++){
                this.mmPerSecSpeeds.add(mmPerSecTargetSpeed);
            }
        }
        return computingFinished;
    }


    public String getSection(char axisId){
        return sections.get(axisId);
    }

    public List<Character> getSections(){
        return new ArrayList<>(sections.keySet());
    }

    public PhysicalVector computeVectorAverage(){
        if(vectorAverage == null){

            Map<Character, List<BigDecimal>> dimensions = new HashMap<>();
            for(PhysicalVector vector : vectors){
                for(Map.Entry<Character, BigDecimal> entry : vector.getVectorDimensions().entrySet()){
                    if(!dimensions.containsKey(entry.getKey()))
                        dimensions.put(entry.getKey(), new ArrayList<>());
                    dimensions.get(entry.getKey()).add(entry.getValue());
                }
            }

            PhysicalVector average = new PhysicalVector();
            for(Map.Entry<Character, List<BigDecimal>> entry : dimensions.entrySet()){
                BigDecimal sum = BigDecimal.valueOf(0.0);
                for (BigDecimal val : entry.getValue()){
                    sum = sum.add(val);
                }
                average.addDimension(entry.getKey(), sum.divide(BigDecimal.valueOf(entry.getValue().size()), RoundingMode.HALF_EVEN));
            }
            vectorAverage = average;

        }
        return vectorAverage;
    }

    @Override
    public boolean equals(Object other){
        return other instanceof ComputingCommand && command.equals(((ComputingCommand) other).getCommand());
    }

}
