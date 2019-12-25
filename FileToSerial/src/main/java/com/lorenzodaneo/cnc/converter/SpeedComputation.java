package com.lorenzodaneo.cnc.converter;

import com.lorenzodaneo.cnc.physics.PhysicalVector;
import com.lorenzodaneo.cnc.utils.BigDecimalUtils;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.LinkedList;
import java.util.List;

public class SpeedComputation {

    public static final int MAX_LIST_SIZE = 5;
    private static final BigDecimal JUNCTION_DEVIATION = BigDecimal.valueOf(0.10);

    private LinkedList<ComputingCommand> commands = new LinkedList<>();

    void addComputingCommandsList(LinkedList<ComputingCommand> commands){
        for (ComputingCommand command : commands){
            this.commands.addLast(command);
        }
    }

    LinkedList<ComputingCommand> getFirsts(int quantity){
        LinkedList<ComputingCommand> firsts = new LinkedList<>();
        for (int i = 0; i < quantity && i < commands.size(); i++){
            firsts.addLast(commands.getFirst());
            commands.removeFirst();
        }
        return firsts;
    }

    LinkedList<ComputingCommand> getAll(){
        LinkedList<ComputingCommand> all = new LinkedList<>(commands);
        commands.clear();
        return all;
    }

    boolean isFull(){
        return commands.size() >= MAX_LIST_SIZE;
    }

    public LinkedList<ComputingCommand> getNextCommands(boolean lastCommand){
        if(lastCommand){
            return getAll();
        } else if(isFull()){
            return getFirsts(SpeedComputation.MAX_LIST_SIZE);
        } else {
            return getFirsts(3);
        }
    }

    public void computeSpeedBackwards(BigDecimal maxAcceleration, boolean lastCommands){
        ComputingCommand previousCommand = null;

        for(int i = 0; i < commands.size(); i++){
            ComputingCommand command = commands.get(i);

            if(previousCommand == null){
                previousCommand = command;
                continue;
            }

            if(command.getCommand().equals("M02")){
                System.out.println("ju");
            }

            PhysicalVector currentVector = command.computeVectorAverage();
            List<Character> axisWithInverseDirection = currentVector.detectInverseDirection(previousCommand.computeVectorAverage());
//            try{ // compute junction speed
            BigDecimal adjustedSpeed, targetSpeed;
            if(axisWithInverseDirection != null && !lastCommands){// && axisWithInverseDirection.size() > 0){ // TODO: direction inversion is not the only reason to recalculate speed

                final BigDecimal cosThetaAngle = PhysicalVector.computeCosThetaAngleByDotProduct(currentVector, previousCommand.computeVectorAverage());
                final BigDecimal sinHalfThetaAngle = BigDecimalUtils.bigSqrt(BigDecimal.ONE.subtract(cosThetaAngle).divide(BigDecimal.valueOf(2), RoundingMode.HALF_EVEN)).setScale(20, RoundingMode.HALF_EVEN);
                final BigDecimal radiusBetweenVectors = JUNCTION_DEVIATION.multiply(sinHalfThetaAngle.divide(BigDecimal.ONE.subtract(sinHalfThetaAngle), RoundingMode.HALF_EVEN));
                adjustedSpeed = BigDecimalUtils.bigSqrt(maxAcceleration.multiply(radiusBetweenVectors)).setScale(20, RoundingMode.HALF_EVEN);
                adjustedSpeed = adjustedSpeed.min(previousCommand.getLastMmPerSecSpeed().min(command.getFirstMmPerSecSpeed()));
//                    previousCommand.setLastMmPerSecSpeed(finalSpeed);
//                    command.setFirstMmPerSecSpeed(finalSpeed);
//            } else if(axisWithInverseDirection != null){
//
                targetSpeed = previousCommand.getLastMmPerSecSpeed();
            } else {
                adjustedSpeed = BigDecimal.valueOf(2.0);
                targetSpeed = previousCommand.getFlatSpeed();
            }

            int previousCommandIndex = commands.indexOf(previousCommand);
            for(int n = previousCommandIndex; n >= 0; n--){
                previousCommand = commands.get(n);
                if(previousCommand.computeSpeedBasedOnNext(adjustedSpeed, targetSpeed))
                    break;
                adjustedSpeed = previousCommand.getFirstMmPerSecSpeed();
            }
            ComputingCommand nextCommand;
            for(int n = i; n < commands.size(); n++){
                nextCommand = commands.get(n);
                if(nextCommand.computeSpeedBasedOnPrevious(adjustedSpeed, targetSpeed))
                    break;
                adjustedSpeed = previousCommand.getFirstMmPerSecSpeed();
            }
//            } catch (Throwable e){
//                throw e;
//            }

            previousCommand = command;
        }
    }

}
