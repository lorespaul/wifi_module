package com.lorenzodaneo.cnc.converter;

import com.lorenzodaneo.cnc.physics.PhysicalVector;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class ComputingCommand {

    private static final BigDecimal MAX_POINT_TO_POINT_DISTANCE = BigDecimal.valueOf(0.1);

    GCodeEnum commandType;
    BigDecimal linearDistance;
    List<BigDecimal> splitLinearDistance;
    BigDecimal mmPerSecSpeed;
    Map<Character, String> sections;

    List<PhysicalVector> vectors;

    ComputingCommand(GCodeEnum commandType, BigDecimal linearDistance, int involvedMotors, BigDecimal mmPerSecSpeed, Map<Character, String> sections){
        this.commandType = commandType;
        this.linearDistance = linearDistance;
        this.mmPerSecSpeed = mmPerSecSpeed;
        this.sections = sections;
        this.vectors = new ArrayList<>();
        computeSplitLinearDistance(involvedMotors);
    }

    public GCodeEnum getCommandType(){
        return this.commandType;
    }

    public List<BigDecimal> getSplitLinearDistance(){
        return this.splitLinearDistance;
    }

    public BigDecimal getMmPerSecSpeed(){
        return this.mmPerSecSpeed;
    }

    public void addVector(PhysicalVector vector){
        vectors.add(vector);
    }

//    public BigDecimal getJunctionSpeed(){
//        return this.junctionManagement.junctionSpeed;
//    }

    private void computeSplitLinearDistance(int involvedMotors){
       this.splitLinearDistance = new ArrayList<>();
        if(involvedMotors > 1 && linearDistance.compareTo(MAX_POINT_TO_POINT_DISTANCE) > 0){
            BigDecimal[] splitting = linearDistance.divideAndRemainder(MAX_POINT_TO_POINT_DISTANCE);
            for(BigDecimal i = BigDecimal.ZERO; i.compareTo(splitting[0]) < 0; i = i.add(BigDecimal.ONE)){
                this.splitLinearDistance.add(MAX_POINT_TO_POINT_DISTANCE);
            }
            this.splitLinearDistance.add(splitting[1]);
        } else {
            this.splitLinearDistance.add(linearDistance);
        }
    }

    public String getSection(char axisId){
        return sections.get(axisId);
    }

}
