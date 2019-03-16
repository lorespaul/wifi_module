package com.lorenzodaneo.cnc;


import org.apache.commons.lang3.StringUtils;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class Main {

    public static void main(String[] argv) throws Exception {

        TwoWaySerialCommunication serial = new TwoWaySerialCommunication();

        if(serial.connect("/dev/cu.usbmodem14201")){

            GCodeFileReader gCode = new GCodeFileReader("cnc.ngc");
            String command;
            int executed = 0;
            while ((command = gCode.getCommand()) != null){

                if(command.startsWith("G") || command.startsWith("M") || command.startsWith("F")) {
                    serial.writeLine(command);

                    String line;
                    do {
                        line = serial.readLine();
                        System.out.println(line);
                    } while (!line.startsWith("CommandTime="));

                    line = line.replace("CommandTime=", "");
                    if(StringUtils.isNumeric(line)){
                        int wait = Integer.valueOf(line);
                        Thread.sleep(wait);
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
