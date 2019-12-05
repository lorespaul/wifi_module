package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.TwoWaySerialCommunication;
import com.lorenzodaneo.cnc.converter.ConverterManager;

public class GCodeConsumer extends GCodeTransmitter {

    private TwoWaySerialCommunication serial;
    private GCodeQueue queue;
    private ConverterManager converterManager = new ConverterManager();

    private int maxCommandLength = 0;
    private int writtenCommands = 0;

    public GCodeConsumer(TwoWaySerialCommunication serial, GCodeQueue queue){
        super();
        this.serial = serial;
        this.queue = queue;
    }

    @Override
    public void run() {

        while (true) {
            try {
                String command = queue.getGCode();
                if(command != null && (command.startsWith("G") || command.startsWith("M2") || command.startsWith("F") || command.startsWith(TEST))){
                    if(command.startsWith("G02")){
                        System.out.println(converterManager.getCurrentPositions());
                        continue;
                    }

                    boolean test = serial == null;
                    if(command.startsWith(TEST)){
                        test = true;
                        command = command.replace(TEST,  "");
                    }

                    command = converterManager.convertCommand(command.trim());

                    if(!command.isEmpty()) {
                        logger.info(command);

                        if(!test){
                            String line;
                            do {
                                line = serial.readLine();
    //                            System.out.println(line);
                            } while (!line.startsWith("GET_NEXT"));

                            serial.writeLine(command);
                        }

                        if(command.length() > maxCommandLength)
                            maxCommandLength = command.length();
                        writtenCommands++;
                    }

                    if (command.equals(ConverterManager.FINAL_COMMAND)){
                        System.out.println("Commands executed: " + writtenCommands);
                        System.out.println("Max command length: " + maxCommandLength);
                        maxCommandLength = 0;
                        writtenCommands = 0;
                    }
                }

            } catch (Exception e){
                System.out.println(e.getMessage());
            }

        }

    }
}
