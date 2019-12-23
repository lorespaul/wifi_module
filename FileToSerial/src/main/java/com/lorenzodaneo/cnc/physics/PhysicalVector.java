package com.lorenzodaneo.cnc.physics;

import com.lorenzodaneo.cnc.utils.BigDecimalUtils;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class PhysicalVector {

    private Map<Character, BigDecimal> vectorDimensions;

    public PhysicalVector(){
        this.vectorDimensions = new HashMap<>();
    }

    public void addDimension(char axisId, BigDecimal dimension){
        vectorDimensions.put(axisId, dimension);
    }

    public void clear(){
        vectorDimensions.clear();
    }

    Map<Character, BigDecimal> getVectorDimensions(){
        return vectorDimensions;
    }

    int getVectorDimensionsCount(){
        return vectorDimensions.size();
    }

    // with dot product
    public static BigDecimal computeThetaAngleByDotProduct(PhysicalVector... vectors){
        if(vectors.length <= 1)
            throw new IllegalArgumentException("Vector in dot product must be more than one.");

        int dimensions = 0;
        Map<Character, List<BigDecimal>> dividendOperators = new HashMap<>();
        BigDecimal divisor = null;
        for (PhysicalVector vector : vectors){
            if(dimensions == 0)
                dimensions = vector.getVectorDimensionsCount();
            else if(vector.getVectorDimensionsCount() != dimensions)
                throw new IllegalArgumentException("Computing theta angle require vectors have same dimensions count.");

            BigDecimal partialDivisor = BigDecimal.valueOf(0.0);
            for (Map.Entry<Character, BigDecimal> entry : vector.getVectorDimensions().entrySet()){
                if(!dividendOperators.containsKey(entry.getKey()))
                    dividendOperators.put(entry.getKey(), new ArrayList<>());
                dividendOperators.get(entry.getKey()).add(entry.getValue());
                partialDivisor = partialDivisor.add(entry.getValue().pow(2));
            }

            partialDivisor = BigDecimalUtils.bigSqrt(partialDivisor).setScale(4, RoundingMode.HALF_EVEN);
            if(divisor == null)
                divisor = partialDivisor;
            else
                divisor = divisor.multiply(partialDivisor);
        }

        dimensions = 0;
        // finish computing dividend
        BigDecimal dividend = BigDecimal.valueOf(0.0);
        for (Map.Entry<Character, List<BigDecimal>> entry : dividendOperators.entrySet()){
            if(dimensions == 0)
                dimensions = entry.getValue().size();
            else if(entry.getValue().size() != dimensions)
                throw new IllegalArgumentException("Axis through vectors must have always same ids.");

            BigDecimal partialDividend = null;
            for(BigDecimal dimension : entry.getValue()){
                if(partialDividend == null)
                    partialDividend = dimension;
                else
                    partialDividend = partialDividend.multiply(dimension);
            }
            assert partialDividend != null;
            dividend = dividend.add(partialDividend);
        }

        return dividend.setScale(4, RoundingMode.HALF_EVEN).divide(divisor, RoundingMode.HALF_EVEN);
    }

}
