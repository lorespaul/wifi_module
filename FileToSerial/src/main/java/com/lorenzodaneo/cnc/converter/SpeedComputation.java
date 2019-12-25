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

    void addComputingCommand(ComputingCommand command){
        this.commands.addLast(command);
    }

    LinkedList<ComputingCommand> getAllCommands(){
        return commands;
    }

    LinkedList<ComputingCommand> poolAllCommands(){
        LinkedList<ComputingCommand> all = new LinkedList<>(commands);
        commands.clear();
        return all;
    }

    public LinkedList<ComputingCommand> poolNextCommands(boolean flush){
        if(flush){
            return poolAllCommands();
        } else {
            LinkedList<ComputingCommand> list = new LinkedList<>();
            if(commands.size() >= MAX_LIST_SIZE){
                list.add(commands.getFirst());
                commands.removeFirst();
            }
            return list;
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

            PhysicalVector currentVector = command.computeVectorAverage();
            List<Character> axisWithInverseDirection = currentVector.detectInverseDirection(previousCommand.computeVectorAverage());

            BigDecimal adjustedSpeed, targetSpeed;
            if(axisWithInverseDirection != null && !lastCommands){ //TODO: direction inversion is not the only reason to recalculate speed

                final BigDecimal cosThetaAngle = PhysicalVector.computeCosThetaAngleByDotProduct(currentVector, previousCommand.computeVectorAverage());
                final BigDecimal sinHalfThetaAngle = BigDecimalUtils.bigSqrt(BigDecimal.ONE.subtract(cosThetaAngle).divide(BigDecimal.valueOf(2), RoundingMode.HALF_EVEN)).setScale(20, RoundingMode.HALF_EVEN);
                final BigDecimal radiusBetweenVectors = JUNCTION_DEVIATION.multiply(sinHalfThetaAngle.divide(BigDecimal.ONE.subtract(sinHalfThetaAngle), RoundingMode.HALF_EVEN));
                adjustedSpeed = BigDecimalUtils.bigSqrt(maxAcceleration.multiply(radiusBetweenVectors)).setScale(20, RoundingMode.HALF_EVEN);
                adjustedSpeed = adjustedSpeed.min(previousCommand.getLastMmPerSecSpeed().min(command.getFirstMmPerSecSpeed()));

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

            previousCommand = command;
        }
    }

}
