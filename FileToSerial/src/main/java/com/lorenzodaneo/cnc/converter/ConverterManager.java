package com.lorenzodaneo.cnc.converter;

import com.lorenzodaneo.cnc.physics.PhysicalVector;
import com.lorenzodaneo.cnc.utils.BigDecimalUtils;
import org.apache.log4j.Logger;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.*;
import java.util.stream.Collectors;

public class ConverterManager {


    private static Logger logger = Logger.getLogger(ConverterManager.class);

    private static final BigDecimal MICROS_CONVERSION = BigDecimal.valueOf(1000000);
    private static final BigDecimal STD_SPEED_SLOW = BigDecimal.valueOf(30);
    private static final BigDecimal STD_SPEED_FAST = BigDecimal.valueOf(40);

    private static final BigDecimal ACCELERATION = BigDecimal.valueOf(10.0);

    private static BigDecimal cachedFCommand = null;
    private BigDecimal currentSpeed = BigDecimal.valueOf(0.0);

    private ComputingJunction computingJunction = new ComputingJunction();

    private List<SingleAxisConverterMirroring> mirroredAxisConverters = Arrays.asList(
            new SingleAxisConverterMirroring(CommandSectionEnum.XAxis.value, true),
            new SingleAxisConverterMirroring(CommandSectionEnum.YAxis.value, true),
            new SingleAxisConverterMirroring(CommandSectionEnum.ZAxis.value, false)
    );


    private List<SingleAxisConverter> getAxisConvertersOnly(){
        return this.mirroredAxisConverters.stream().map(SingleAxisConverterMirroring::getConverter).collect(Collectors.toList());
    }


    private List<SingleAxisConverter> getAxisMirrorsOnly(){
        return this.mirroredAxisConverters.stream().map(SingleAxisConverterMirroring::getPreComputingAxis).collect(Collectors.toList());
    }


    public ConverterManager(String initPosition){
        if(initPosition != null && !initPosition.isEmpty()){
            for(SingleAxisConverterMirroring converter : this.mirroredAxisConverters){
                String section = getSection(initPosition, CommandSectionEnum.getEnum(converter.getAxisId()));
                if(section != null && !section.isEmpty())
                    converter.setLastPosition(section);
            }
        }
    }


    public String getCurrentPositions(boolean appendUserSpeed){
        StringBuilder result = new StringBuilder();
        for(SingleAxisConverterMirroring axisConverterMirroring : this.mirroredAxisConverters)
            result.append(axisConverterMirroring.getAxisId())
                    .append(new BigDecimal(axisConverterMirroring.getLastPosition()
                            .toString()).setScale(4, RoundingMode.HALF_EVEN).toString())
                    .append(" ");
        if(cachedFCommand != null && appendUserSpeed)
            result.append(GCodeEnum.F.value.get(0)).append(cachedFCommand.toString());
        return result.toString().trim();
    }



    public void trySetSpeed(String command){
        String fCommandString = getSection(command, CommandSectionEnum.FCommand);
        if(fCommandString != null && !fCommandString.isEmpty()) {
            fCommandString = fCommandString.replace("F", "");
            cachedFCommand = new BigDecimal(fCommandString);
            if(cachedFCommand.compareTo(BigDecimal.TEN) < 0 || cachedFCommand.compareTo(STD_SPEED_FAST) > 0)
                cachedFCommand = null;
        }
    }


    public void setHomePosition(CommandSectionEnum... sectionEnums){
        for(CommandSectionEnum sectionEnum : sectionEnums){
            for(SingleAxisConverterMirroring axisConverterMirroring : this.mirroredAxisConverters){
                if(axisConverterMirroring.getAxisId() == sectionEnum.value)
                    axisConverterMirroring.setLastPosition("0.0");
            }
        }
    }


    public List<String> convertCommands(List<String> commands, boolean flush){

        List<String> result = new ArrayList<>();
        LinkedList<ComputingCommand> computingCommandList = new LinkedList<>();

        for(String command : commands){
            trySetSpeed(command);

            String commandTypeString = getSection(command, CommandSectionEnum.GCommand);
            GCodeEnum commandType = commandTypeString != null ? GCodeEnum.getEnum(commandTypeString) : null;
            if(commandTypeString == null || !commandType.isMovementCommand()){
                logger.warn("Command must have a type: " + command);
                result.add("");
                continue;
            }

            Map<Character, String> sections = new HashMap<>();
            for(SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
                String section = getSection(command, CommandSectionEnum.getEnum(mirroring.getAxisId()));
                if(section != null && !section.isEmpty()){
                    section = mirroring.getAxisId() + new BigDecimal(section.substring(1)).setScale(2, RoundingMode.HALF_EVEN).toString(); // rounding to machine precision
                    mirroring.putMirroringNextPosition(section);
                    sections.put(mirroring.getAxisId(), section);
                }
            }

            BigDecimal mmPerSecSpeed = commandType == GCodeEnum.G00 ? STD_SPEED_FAST : cachedFCommand != null ? cachedFCommand :  STD_SPEED_SLOW;

            List<SingleAxisConverter> involvedMotors = new ArrayList<>();
            BigDecimal linearDistance = computeLinearDistance(getAxisMirrorsOnly(), involvedMotors);
            if(linearDistance.compareTo(BigDecimal.ZERO) == 0 && commandType != GCodeEnum.G28){
                result.add("");
                continue;
            }

            ComputingCommand computingCommand = new ComputingCommand(commandType, linearDistance, involvedMotors.size(), mmPerSecSpeed, sections);

            for (int splittingPosition = 0; splittingPosition < computingCommand.getSplitLinearDistance().size(); splittingPosition++){
                BigDecimal distance = computingCommand.getSplitLinearDistance().get(splittingPosition);

                PhysicalVector vector = new PhysicalVector();
                external:for(BigDecimal checkSpeed = computingCommand.getMmPerSecSpeed(); checkSpeed.compareTo(BigDecimal.ZERO) > 0; checkSpeed = checkSpeed.subtract(BigDecimal.ONE)){

                    if(checkSpeed.compareTo(BigDecimal.ONE) == 0){
                        logger.warn("Speed not adjustable.");
                        return Collections.emptyList();
                    }

                    BigDecimal deltaTMicros = computeDeltaTMicrosOfCommand(distance, checkSpeed);
                    if(deltaTMicros.compareTo(BigDecimal.ZERO) == 0)
                        logger.warn("Speed is zero with command: " + command);

                    for(SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
                        BigDecimal axisSpeed = mirroring.preComputing(deltaTMicros, computingCommand.getSplitLinearDistance(), splittingPosition, computingCommand.getCommandType() == GCodeEnum.G28);
                        if(getSection(command, CommandSectionEnum.getEnum(mirroring.getAxisId())) != null && axisSpeed.compareTo(BigDecimal.valueOf(-1)) == 0){
                            vector.clear();
                            continue external;
                        }
                        vector.addDimension(mirroring.getAxisId(), axisSpeed);
                    }

                    break;
                }

                computingCommand.addVector(vector);

                for(SingleAxisConverter converter : getAxisMirrorsOnly())
                    converter.completeConversion(computingCommand.getSplitLinearDistance(), splittingPosition);
            }

            computingCommandList.addLast(computingCommand);

        }

        this.computingJunction.addComputingCommandsList(computingCommandList);
        this.computingJunction.computeJunctionSpeed(ACCELERATION, flush);
        List<ComputingCommand> executing;
        if(flush){
            executing = this.computingJunction.getAll();
        } else if(this.computingJunction.isFull()){
            executing = this.computingJunction.getFirsts(ComputingJunction.MAX_LIST_SIZE - 5);
        } else {
            executing = this.computingJunction.getFirsts(1);
        }

        for (ComputingCommand computingCommand : executing){
            result.addAll(convertComputingCommand(computingCommand));
        }

        for(SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
            if(!mirroring.checkAlignment()){
                System.out.println("Axis mirroring alignment checking failed!");
            }
        }

        return result;
    }


    private List<String> convertComputingCommand(ComputingCommand command) {

        for (SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
            String section = command.getSection(mirroring.getAxisId());
            if(section != null && !section.isEmpty())
                mirroring.putConverterNextPosition(section);
        }

        List<String> resultCommands = new ArrayList<>();
        for (int splittingPosition = 0; splittingPosition < command.getSplitLinearDistance().size(); splittingPosition++){
            BigDecimal distance = command.getSplitLinearDistance().get(splittingPosition);

            List<String> actuatorsValues = new ArrayList<>();
            external:for(BigDecimal checkSpeed = command.getMmPerSecSpeed(); checkSpeed.compareTo(BigDecimal.ZERO) > 0; checkSpeed = checkSpeed.subtract(BigDecimal.ONE)){

                if(checkSpeed.compareTo(BigDecimal.ONE) == 0){
                    logger.warn("Speed not adjustable.");
                    return Collections.emptyList();
                }

                BigDecimal deltaTMicros = computeDeltaTMicrosOfCommand(distance, ACCELERATION, checkSpeed);
                if(deltaTMicros.compareTo(BigDecimal.ZERO) == 0)
                    logger.warn("Speed is zero with command: " + command);

                for(SingleAxisConverterMirroring converter : this.mirroredAxisConverters){
                    String value = converter.convert(deltaTMicros, command.getSplitLinearDistance(), splittingPosition, command.getCommandType() == GCodeEnum.G28);
                    if(value != null && value.equals(SingleAxisConverter.RECALCULATES)){
                        actuatorsValues.clear();
                        continue external;
                    }
                    actuatorsValues.add(value);
                }

                break;
            }

            for(SingleAxisConverter converter : getAxisConvertersOnly())
                converter.completeConversion(command.getSplitLinearDistance(), splittingPosition);

            StringBuilder builder = new StringBuilder();
            boolean valueFound = false;
            for(String val : actuatorsValues){
                if(val != null && !val.isEmpty()){
                    builder.append(val);
                    builder.append(" ");
                    valueFound = true;
                }
            }

            if(valueFound)
                resultCommands.add(builder.toString().trim());
        }

        return resultCommands;
    }


    private BigDecimal computeLinearDistance(List<SingleAxisConverter> axisConverters, List<SingleAxisConverter> involvedMotors){
        BigDecimal linearDistance = BigDecimal.ZERO;
        for (SingleAxisConverter converter : axisConverters){
            BigDecimal axisDistance = converter.computeStartToEndDistance();
            if(axisDistance.compareTo(BigDecimal.ZERO) > 0) {
                involvedMotors.add(converter);
                if(linearDistance.compareTo(BigDecimal.ZERO) == 0){
                    linearDistance = axisDistance;
                } else {
                    linearDistance = BigDecimalUtils.bigSqrt(linearDistance.pow(2).add(axisDistance.pow(2))).setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN);
                }
            }
        }
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN);
    }


    private BigDecimal computeDeltaTMicrosOfCommand(BigDecimal linearDistance, BigDecimal mmPerSec){
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSec, RoundingMode.HALF_EVEN).multiply(MICROS_CONVERSION);
    }

    private BigDecimal computeDeltaTMicrosOfCommand(BigDecimal linearDistance, BigDecimal acceleration, BigDecimal mmPerSecTargetSpeed){
//        // calculate delta t with target speed
//        BigDecimal deltaTOnTargetSpeed = linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSecTargetSpeed, RoundingMode.HALF_EVEN);
//        // calculate speed based on delta t on target speed and acceleration (acceleration * delta t on target speed)
//        BigDecimal instantaneousSpeed = acceleration.multiply(deltaTOnTargetSpeed);
//        // recalculate current speed adding instantaneous speed if target is bigger or subtracting instantaneous speed if target is smaller
//        this.currentSpeed = mmPerSecTargetSpeed.compareTo(currentSpeed) > 0 ? this.currentSpeed.add(instantaneousSpeed) : this.currentSpeed.subtract(instantaneousSpeed);
//        // calculate final delta t
        // Vf^2 = Vi^2 + 2*acceleration*distance
        if(mmPerSecTargetSpeed.compareTo(this.currentSpeed) != 0){
            BigDecimal signedAcceleration = mmPerSecTargetSpeed.compareTo(this.currentSpeed) >= 0 ? acceleration : acceleration.negate();
            this.currentSpeed = BigDecimalUtils.bigSqrt(this.currentSpeed.pow(2).add(BigDecimal.valueOf(2).multiply(signedAcceleration).multiply(linearDistance)));
        }
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(this.currentSpeed, RoundingMode.HALF_EVEN).multiply(MICROS_CONVERSION);
    }


    public static String getSection(String command, CommandSectionEnum sectionEnum){
        String[] splitting = command.split(" ");

        for (String section : splitting){
            if(section.length() > 0 && CommandSectionEnum.getEnum(section.charAt(0)) == sectionEnum){
                return  section;
            }
        }
        return null;
    }

}
