package com.lorenzodaneo.cnc.converter;

import org.apache.log4j.Logger;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class ConverterManager {


    private static Logger logger = Logger.getLogger(ConverterManager.class);

    private static final BigDecimal MAX_POINT_TO_POINT_DISTANCE = BigDecimal.valueOf(2.0);

    public static final String COMMAND_GET_CURRENT_POSITION = "M114";
    public static final String FINAL_COMMAND = "M2";
    public static final String G_FAST = "G00";
    public static final String G_SLOW = "G01";
    public static final String G_H0ME = "G28";

    private static final BigDecimal MICROS_CONVERSION = BigDecimal.valueOf(1000000);
    private static final BigDecimal STD_SPEED_SLOW = BigDecimal.valueOf(30);
    private static final BigDecimal STD_SPEED_FAST = BigDecimal.valueOf(40);

    private static BigDecimal cachedFCommand = null;

    private List<SingleAxisConverter> axisConverters = Arrays.asList(
            new SingleAxisConverter(CommandSectionEnum.XAxis.value, true),
            new SingleAxisConverter(CommandSectionEnum.YAxis.value, true),
            new SingleAxisConverter(CommandSectionEnum.ZAxis.value, false)
    );


    public ConverterManager(String initPosition){
        if(initPosition != null && !initPosition.isEmpty()){
            for(SingleAxisConverter converter : this.axisConverters){
                String section = getSection(initPosition, CommandSectionEnum.getEnum(converter.getAxisId()));
                if(section != null && !section.isEmpty())
                    converter.setLastPosition(section);
            }
        }
    }


    public String getCurrentPositions(boolean appendUserSpeed){
        StringBuilder result = new StringBuilder();
        for(SingleAxisConverter axisConverter : this.axisConverters)
            result.append(axisConverter.getAxisId())
                    .append(new BigDecimal(axisConverter.getLastPosition()
                            .toString()).setScale(4, RoundingMode.HALF_EVEN).toString())
                    .append(" ");
        if(cachedFCommand != null && appendUserSpeed)
            result.append("F").append(cachedFCommand.toString());
        return result.toString().trim();
    }


    public List<String> convertCommand(String command) {
        if(command.equals(FINAL_COMMAND) || command.equals("")){
            return Collections.singletonList(command);
        }

        String fCommandString = getSection(command, CommandSectionEnum.FCommand);
        if(fCommandString != null && !fCommandString.isEmpty()) {
            fCommandString = fCommandString.replace("F", "");
            cachedFCommand = new BigDecimal(fCommandString);
            if(cachedFCommand.compareTo(BigDecimal.TEN) < 0 || cachedFCommand.compareTo(STD_SPEED_FAST) > 0)
                cachedFCommand = null;
        }

        String commandType = getSection(command, CommandSectionEnum.Command);
        if(commandType == null || (!commandType.equals(G_FAST) && !commandType.equals(G_H0ME) && !commandType.equals(G_SLOW))){
            logger.warn("Command must have a type: " + command);
            return Collections.singletonList("");
        }

        for(SingleAxisConverter converter : this.axisConverters){
            String section = getSection(command, CommandSectionEnum.getEnum(converter.getAxisId()));
            if(section != null && !section.isEmpty())
                converter.putNextPosition(section);
        }

        BigDecimal mmPerSecSpeed = commandType.equals(G_FAST) ? STD_SPEED_FAST : cachedFCommand != null ? cachedFCommand :  STD_SPEED_SLOW;

        List<SingleAxisConverter> implicatedMotors = new ArrayList<>();
        BigDecimal linearDistance = computeLinearDistance(this.axisConverters, implicatedMotors);
        if(linearDistance.compareTo(BigDecimal.ZERO) == 0 && !command.equals(G_H0ME))
            return Collections.singletonList("");

        List<BigDecimal> splitLinearDistance = new ArrayList<>();
        if(implicatedMotors.size() > 1 && linearDistance.compareTo(MAX_POINT_TO_POINT_DISTANCE) > 0){
            BigDecimal[] splitting = linearDistance.divideAndRemainder(MAX_POINT_TO_POINT_DISTANCE);
            for(BigDecimal i = BigDecimal.ZERO; i.compareTo(splitting[0]) < 0; i = i.add(BigDecimal.ONE)){
                splitLinearDistance.add(MAX_POINT_TO_POINT_DISTANCE);
            }
            splitLinearDistance.add(splitting[1]);
        } else {
            splitLinearDistance.add(linearDistance);
        }

        List<String> resultCommands = new ArrayList<>();
        for (int splittingPosition = 0; splittingPosition < splitLinearDistance.size(); splittingPosition++){
            BigDecimal distance = splitLinearDistance.get(splittingPosition);

            List<String> actuatorsValues = new ArrayList<>();
            external:for(BigDecimal checkSpeed = mmPerSecSpeed; checkSpeed.compareTo(BigDecimal.ZERO) > 0; checkSpeed = checkSpeed.subtract(BigDecimal.ONE)){

                if(checkSpeed.compareTo(BigDecimal.ONE) == 0){
                    logger.warn("Speed not adjustable.");
                    return null;
                }
                BigDecimal speedMicros = computeSpeedMicros(distance, checkSpeed);
                if(speedMicros.compareTo(BigDecimal.ZERO) == 0)
                    logger.warn("Speed is zero with command: " + command);

                for(SingleAxisConverter converter : this.axisConverters){
                    String value = converter.convert(speedMicros, splitLinearDistance, splittingPosition, commandType.equals(G_H0ME));
                    if(value != null && value.equals(SingleAxisConverter.RECALCULATES)){
                        actuatorsValues.clear();
                        continue external;
                    }
                    actuatorsValues.add(value);
                }

                break;
            }

            for(SingleAxisConverter converter : this.axisConverters)
                converter.completeConversion(splitLinearDistance, splittingPosition);

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


    private BigDecimal computeSpeedMicros(BigDecimal linearDistance, BigDecimal mmPerSec){
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSec, RoundingMode.HALF_EVEN).multiply(MICROS_CONVERSION);
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
