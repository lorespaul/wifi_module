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
            String command = queue.getGCode();
            if(command != null && (command.startsWith("G") || command.startsWith("M") || command.startsWith("F"))){

                command = command.trim();
                command = converterManager.convertCommand(command);

                System.out.println(command);
                if(!command.isEmpty()) {

                    String line;
                    do {
                        line = serial.readLine();
                        System.out.println(line);
                    } while (!line.startsWith("GET_NEXT"));

                    serial.writeLine(command);
                    if(command.length() > maxCommandLength)
                        maxCommandLength = command.length();
                    writtenCommands++;
                }
            }

        }

    }
}
