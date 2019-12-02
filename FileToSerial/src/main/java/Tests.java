import java.math.BigDecimal;

public class Tests {

    public static void main(String[] argv){
        test2();
        System.out.println("-----------------------------------------------------");
        test3();
    }


    private static void test3(){
        BigDecimal hypotenuse = BigDecimal.valueOf(30);
        BigDecimal baseAngle = BigDecimal.valueOf(2.25);
        BigDecimal angle = null;
        for(int i = 0; i <= 160; i++){
            angle = baseAngle.multiply(BigDecimal.valueOf(i));
            BigDecimal sinAngle = BigDecimal.valueOf(Math.sin(Math.toRadians(angle.doubleValue())));
            BigDecimal cosAngle = BigDecimal.valueOf(Math.cos(Math.toRadians(angle.doubleValue())));
            //System.out.println("angle" + angle.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " sin" + sinAngle.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " cos" + cosAngle.setScale(2, BigDecimal.ROUND_HALF_EVEN));
            BigDecimal x = hypotenuse.multiply(sinAngle).add(hypotenuse);
            BigDecimal y = hypotenuse.multiply(cosAngle).add(hypotenuse);
            System.out.println("G01 X" + x.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " Y" + y.setScale(2, BigDecimal.ROUND_HALF_EVEN));
        }
        System.out.println(angle.doubleValue());
    }


    private static void test2(){
        BigDecimal hypotenuse = BigDecimal.valueOf(100);
        BigDecimal baseAngle = BigDecimal.valueOf(2.25);
        BigDecimal angle = null;
        for(int i = 0; i <= 40; i++){
            angle = baseAngle.multiply(BigDecimal.valueOf(i));
            BigDecimal sinAngle = BigDecimal.valueOf(Math.sin(Math.toRadians(angle.doubleValue())));
            BigDecimal cosAngle = BigDecimal.valueOf(Math.cos(Math.toRadians(angle.doubleValue())));
            //System.out.println("angle" + angle.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " sin" + sinAngle.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " cos" + cosAngle.setScale(2, BigDecimal.ROUND_HALF_EVEN));
            BigDecimal x = hypotenuse.multiply(sinAngle).add(BigDecimal.TEN);
            BigDecimal y = hypotenuse.multiply(cosAngle).add(BigDecimal.TEN);
            System.out.println("G01 X" + x.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " Y" + y.setScale(2, BigDecimal.ROUND_HALF_EVEN));
            System.out.println("G01 X10 Y10");
            System.out.println("G00 Z0");
            System.out.println("G00 X" + x.setScale(2, BigDecimal.ROUND_HALF_EVEN) + " Y" + y.setScale(2, BigDecimal.ROUND_HALF_EVEN));
            System.out.println("G00 Z6");
        }
        System.out.println(angle.doubleValue());
    }


}
