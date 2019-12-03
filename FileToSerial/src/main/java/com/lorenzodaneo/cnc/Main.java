package com.lorenzodaneo.cnc;


import com.lorenzodaneo.cnc.converter.ConverterManager;
import com.lorenzodaneo.cnc.transmission.GCodeConsumer;
import com.lorenzodaneo.cnc.transmission.GCodeProducer;
import com.lorenzodaneo.cnc.transmission.GCodeQueue;
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

            GCodeQueue queue = new GCodeQueue(20);
            GCodeProducer producer = new GCodeProducer(queue);
            GCodeConsumer consumer = new GCodeConsumer(serial, queue);

            producer.start();
            consumer.start();

            System.out.println("App started.");

            producer.join();
            consumer.join();

//            int executed = executeFromFile(serial, gCodeFile);
//            int executed = executeTest(serial);

//            System.out.println("Executed " + executed + " commands!");

//            for(int i = 0; i < executed * 3; i++){
//                System.out.println(serial.readLine());
//            }

            serial.close();

        }

    }




    private static int executeFromFile(TwoWaySerialCommunication serial, String gCodeFile) throws IOException, InterruptedException {

        FileIO gCode = new FileIO(gCodeFile);
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
