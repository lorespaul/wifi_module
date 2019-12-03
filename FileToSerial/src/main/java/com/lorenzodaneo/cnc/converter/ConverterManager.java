package com.lorenzodaneo.cnc.converter;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ConverterManager {


    private enum CommandSectionEnum {
        Command(' '),
        FCommand('F'),
        XAxis('X'),
        YAxis('Y'),
        ZAxis('Z');

        char value;

        CommandSectionEnum(char value){
            this.value = value;
        }

        static CommandSectionEnum getEnum(char value){
            for(CommandSectionEnum sectionEnum : CommandSectionEnum.values()){
                if(sectionEnum.value == value)
                    return sectionEnum;
            }
            return CommandSectionEnum.Command;
        }
    }


    public static final String FINAL_COMMAND = "M2";
    private static final String G_FAST = "G00";
    private static final String G_SLOW = "G01";
    private static final String G_H0ME = "G28";

    private static final BigDecimal microsConversion = BigDecimal.valueOf(1000000);
    private static final BigDecimal stdSpeedSlow = BigDecimal.valueOf(30);
    private static final BigDecimal stdSpeedFast = BigDecimal.valueOf(40);

    private static BigDecimal F_LAST = null;

    private List<SingleAxisConverter> axisConverters = Arrays.asList(
            new SingleAxisConverter(CommandSectionEnum.XAxis.value),
            new SingleAxisConverter(CommandSectionEnum.YAxis.value),
            new SingleAxisConverter(CommandSectionEnum.ZAxis.value)
    );


    public String convertCommand(String command) {
        if(command.equals(FINAL_COMMAND))
            return command;

        String commandType = getSection(command, CommandSectionEnum.Command);
        if(commandType == null || (!commandType.equals(G_FAST) && !commandType.equals(G_H0ME) && !commandType.equals(G_SLOW)))
            throw new IllegalArgumentException("Command must have a type.");

        for(SingleAxisConverter converter : this.axisConverters){
            String section = getSection(command, CommandSectionEnum.getEnum(converter.getAxisId()));
            if(section != null && !section.isEmpty())
                converter.putNextPosition(section);
        }

        String fCommandString = getSection(command, CommandSectionEnum.FCommand);
        if(fCommandString != null && !fCommandString.isEmpty())
            F_LAST = new BigDecimal(fCommandString);
        BigDecimal fCommand = F_LAST != null ? F_LAST : commandType.equals(G_FAST) ? stdSpeedFast : commandType.equals(G_SLOW) ? stdSpeedSlow : stdSpeedSlow;

        List<String> actuatorsValues = new ArrayList<>();
        external:for(BigDecimal i = fCommand; i.compareTo(BigDecimal.ZERO) > 0; i = i.subtract(BigDecimal.ONE)){

            if(i.compareTo(BigDecimal.ONE) == 0){
                System.out.println("Speed not adjustable.");
                return null;
            }
            BigDecimal speedMicros = computeSpeedMicros(this.axisConverters, i);

            for(SingleAxisConverter converter : this.axisConverters){
                String value = converter.convert(speedMicros, commandType.equals(G_H0ME));
                if(value != null && value.equals(SingleAxisConverter.RECALCULATES)){
                    actuatorsValues.clear();
                    continue external;
                }
                actuatorsValues.add(value);
            }

            break;
        }

        for(SingleAxisConverter converter : this.axisConverters)
            converter.completeConversion();

        StringBuilder builder = new StringBuilder();
        boolean valueFound = false;
        for(String val : actuatorsValues){
            if(val != null && !val.isEmpty()){
                builder.append(val);
                builder.append(" ");
                valueFound = true;
            }
        }

        return valueFound ? builder.toString().trim() : "";
    }



    private BigDecimal computeSpeedMicros(List<SingleAxisConverter> axisConverters, BigDecimal mmPerSec){
        BigDecimal linearDistance = BigDecimal.ZERO;
        for (SingleAxisConverter converter : axisConverters){
            BigDecimal axisDistance = converter.computeStartToEndDistance();
            if(linearDistance.compareTo(BigDecimal.ZERO) == 0 && axisDistance.compareTo(BigDecimal.ZERO) > 0){
                linearDistance = axisDistance;
            } else if(axisDistance.compareTo(BigDecimal.ZERO) > 0) {
                linearDistance = bigSqrt(linearDistance.pow(2).add(axisDistance.pow(2))).setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN);
            }
        }
        return linearDistance.setScale(SingleAxisConverter.SCALE, RoundingMode.HALF_EVEN).divide(mmPerSec, RoundingMode.HALF_EVEN).multiply(microsConversion);
    }



    private String getSection(String command, CommandSectionEnum sectionEnum){
        String[] splitting = command.split(" ");

        for (String section : splitting){
            if(CommandSectionEnum.getEnum(section.charAt(0)) == sectionEnum){
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
