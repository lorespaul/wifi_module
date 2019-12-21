package com.lorenzodaneo.cnc.converter;

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

    private static BigDecimal cachedFCommand = null;

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


    public List<String> convertCommands(List<String> commands){

        List<String> result = new ArrayList<>();
        List<ComputingCommand> computingCommandList = new ArrayList<>();

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

            computingCommandList.add(new ComputingCommand(commandType, linearDistance, involvedMotors.size(), mmPerSecSpeed, sections));

            for(SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
                mirroring.completeOnMirroring();
            }
        }

        for (ComputingCommand computingCommand : computingCommandList){
            result.addAll(convertComputingCommand(computingCommand));
            for(SingleAxisConverterMirroring mirroring : this.mirroredAxisConverters){
                if(!mirroring.checkAlignment()){
                    System.out.println("Axis mirroring alignment checking failed!");
                }
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
                }                                                                // mm/s^2       // s
//                BigDecimal deltaTMicros = computeDeltaTMicrosOfCommand(distance, BigDecimal.TEN, delta, false, checkSpeed);
                BigDecimal deltaTMicros = computeDeltaTMicrosOfCommand(distance, checkSpeed);
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


    private BigDecimal computeLinearDistance(List<SingleAxisConverter> axisConverters, List<SingleAxisConverter> implicatedMotors){
        BigDecimal linearDistance = BigDecimal.ZERO;
        for (SingleAxisConverter converter : axisConverters){
            BigDecimal axisDistance = converter.computeStartToEndDistance();
            if(axisDistance.compareTo(BigDecimal.ZERO) > 0) {
                implicatedMotors.add(converter);
                if(linearDistance.compareTo(BigDecimal.ZERO) == 0){
                    linearDistance = axisDistance;
                } else {
                    linearDistance = bigSqrt(linearDistance.pow(2).add(axisDistance.pow(2))).setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN);
                }
            }
        }
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN);
    }


    private BigDecimal computeDeltaTMicrosOfCommand(BigDecimal linearDistance, BigDecimal mmPerSec){
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSec, RoundingMode.HALF_EVEN).multiply(MICROS_CONVERSION);
    }

//    private BigDecimal computeDeltaTMicrosOfCommand(BigDecimal linearDistance, BigDecimal acceleration, BigDecimal deltaT, boolean isDeceleration, BigDecimal mmPerSecMaxSpeed){
//        BigDecimal mmPerSec = acceleration.multiply(deltaT); // compute speed based on acceleration and time delta
//        if(isDeceleration)
//            mmPerSec = mmPerSecMaxSpeed.subtract(mmPerSec); // if is deceleration subtract speed found to max speed
//        mmPerSec = mmPerSecMaxSpeed.compareTo(mmPerSec) > 0 ?  mmPerSec : mmPerSecMaxSpeed; // speed can't be bigger than max speed
//        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSec, RoundingMode.HALF_EVEN).multiply(MICROS_CONVERSION);
//    }


    public static String getSection(String command, CommandSectionEnum sectionEnum){
        String[] splitting = command.split(" ");

        for (String section : splitting){
            if(section.length() > 0 && CommandSectionEnum.getEnum(section.charAt(0)) == sectionEnum){
                return  section;
            }
        }
        return null;
    }





    private static final BigDecimal SQRT_DIG = new BigDecimal(150);
    private static final BigDecimal SQRT_PRE = new BigDecimal(10).pow(SQRT_DIG.intValue());

    /**
     * Private utility method used to compute the square root of a BigDecimal.
     *
     * @author Luciano Culacciatti
     * @url http://www.codeproject.com/Tips/257031/Implementing-SqrtRoot-in-BigDecimal
     */
    private static BigDecimal sqrtNewtonRaphson  (BigDecimal c, BigDecimal xn, BigDecimal precision){
        BigDecimal fx = xn.pow(2).add(c.negate());
        BigDecimal fpx = xn.multiply(new BigDecimal(2));
        BigDecimal xn1 = fx.divide(fpx,2*SQRT_DIG.intValue(), RoundingMode.HALF_DOWN);
        xn1 = xn.add(xn1.negate());
        BigDecimal currentSquare = xn1.pow(2);
        BigDecimal currentPrecision = currentSquare.subtract(c);
        currentPrecision = currentPrecision.abs();
        if (currentPrecision.compareTo(precision) <= -1){
            return xn1;
        }
        return sqrtNewtonRaphson(c, xn1, precision);
    }

    /**
     * Uses Newton Raphson to compute the square root of a BigDecimal.
     *
     * @author Luciano Culacciatti
     * @url http://www.codeproject.com/Tips/257031/Implementing-SqrtRoot-in-BigDecimal
     */
    public static BigDecimal bigSqrt(BigDecimal c){
        return sqrtNewtonRaphson(c,new BigDecimal(1),new BigDecimal(1).divide(SQRT_PRE));
    }

}
