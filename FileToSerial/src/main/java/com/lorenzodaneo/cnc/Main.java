package com.lorenzodaneo.cnc;


import org.apache.commons.lang3.StringUtils;
import java.io.IOException;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class Main {

    public static void main(String[] argv) throws Exception {

//        sudo chgrp lock /var/lock
//        sudo chmod 775 /var/lock
//        sudo ln -s /dev/ttyACM0 /dev/ttyS33

        TwoWaySerialCommunication serial = new TwoWaySerialCommunication();

        if(serial.connect("/dev/ttyS33")){

            int executed = executeFromFileWithPreLoading(serial);
//            int executed = executeFromFile(serial);
//            int executed = executeTest(serial);

            System.out.println("Executed " + executed + " commands!");


            serial.close();

        }

    }


    private static int executeFromFileWithPreLoading(TwoWaySerialCommunication serial) throws IOException {
        FileIO gCode = new FileIO("cnc.ngc");
        int written = 0;

        String command;
        while ((command = gCode.getLine()) != null){
            command = command.trim();
            if(command.startsWith("G") || command.startsWith("M") || command.startsWith("F") || command.equals("EXIT")){
                String line;
                do {
                    line = serial.readLine();
                    System.out.println(line);
//                    if(!line.startsWith("GET_NEXT") && !line.isEmpty()){
//                        gCode.writeLine(line);
//                    }
                } while (!line.startsWith("GET_NEXT"));

                serial.writeLine(command);
                written++;
            }

        }
        gCode.close();

        return written;
    }

    private static int executeFromFile(TwoWaySerialCommunication serial) throws IOException, InterruptedException {

        FileIO gCode = new FileIO("cnc.ngc");
        String command;
        int written = 0;
        while ((command = gCode.getLine()) != null){

            if(command.startsWith("G") || command.startsWith("M") || command.startsWith("F")) {
                serial.writeLine(command);

                String line;
                int breaker = 0;
                do {
                    line = serial.readLine();
                    System.out.println(line);
                    if(breaker > 1000){
                        System.out.println("Breaker goes up to 1000.");
                        break;
                    }
                    breaker++;
                } while (!line.startsWith("CommandTime="));

                line = line.replace("CommandTime=", "");
                if(StringUtils.isNumeric(line)){
                    int wait = Integer.parseInt(line) + 10;
                    Thread.sleep(wait);
                } else {
                    System.out.println("Error in command timing reading.");
                    serial.writeLine("G01 X0 Y0 Z0");
                    break;
                }
                written++;
            }

        }
        gCode.close();

        return written;
    }



    private static int executeTest(TwoWaySerialCommunication serial) throws InterruptedException {

        final String[] commands = { "G00 X330 Y330", "G00 X0 Y0" };
        final int cycleNumber = 10;

        for(int i = 0; i < cycleNumber; i++){
            final String command = commands[i%2];
            serial.writeLine(command);

            String line;
            int breaker = 0;
            do {
                line = serial.readLine();
                System.out.println(line);
                if(breaker > 1000){
                    System.out.println("Breacker goes up to 1000.");
                    break;
                }
                breaker++;
            } while (!line.startsWith("CommandTime="));

            line = line.replace("CommandTime=", "");
            if(StringUtils.isNumeric(line)){
                int wait = Integer.parseInt(line) + 100;
                Thread.sleep(wait);
            } else {
                System.out.println("Error in command timing reading.");
                serial.writeLine("G01 X0 Y0 Z0");
                break;
            }
        }

        return cycleNumber;
    }

}
