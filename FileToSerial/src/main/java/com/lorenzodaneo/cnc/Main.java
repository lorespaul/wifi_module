package com.lorenzodaneo.cnc;


/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class Main {

    public static void main(String[] argv) throws Exception {

        TwoWaySerialCommunication serial = new TwoWaySerialCommunication();

        if(serial.connect("/dev/cu.usbmodem14201")){

            GCodeFileReader gCode = new GCodeFileReader("cnc.txt");
            String command;
            while ((command = gCode.getCommand()) != null){

                serial.writeLine(command);

                String line;
                do{
                    line = serial.readLine();
                    System.out.println(line);
                } while (!line.startsWith("CommandTime="));

//                line = line.replace("CommandTime=", "");
//                if(StringUtils.isNumeric(line)){
//                    int wait = Integer.valueOf(line);
//                    if(wait > 1000) wait -= 1000;
//                    Thread.sleep(wait);
//                }
            }

            gCode.close();
            serial.close();

        }

    }

}
