package com.lorenzodaneo.cnc;


import org.apache.commons.lang3.StringUtils;

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

            GCodeFileReader gCode = new GCodeFileReader("cnc.ngc");
            String command;
            int executed = 0;
            while ((command = gCode.getCommand()) != null){

                if(command.startsWith("G") || command.startsWith("M") || command.startsWith("F")) {
                    serial.writeLine(command);

                    String line;
                    int breacker = 0;
                    do {
                        line = serial.readLine();
                        System.out.println(line);
                        if(breacker > 1000){
                            System.out.println("Breacker goes up to 1000.");
                            break;
                        }
                        breacker++;
                    } while (!line.startsWith("CommandTime="));

                    line = line.replace("CommandTime=", "");
                    if(StringUtils.isNumeric(line)){
                        int wait = Integer.valueOf(line) + 10;
                        Thread.sleep(wait);
                    } else {
                        System.out.println("Error in command timing reading.");
                        serial.writeLine("G01 X0 Y0 Z0");
                        break;
                    }
                    executed++;
                }

            }

            System.out.println("Executed " + executed + " commands!");

            gCode.close();
            serial.close();

        }

    }

}
