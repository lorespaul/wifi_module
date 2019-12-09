package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.TwoWaySerialCommunication;
import com.lorenzodaneo.cnc.converter.CommandSectionEnum;
import com.lorenzodaneo.cnc.converter.ConverterManager;
import com.lorenzodaneo.cnc.converter.GCodeEnum;
import com.lorenzodaneo.cnc.fileio.PositionCache;

import java.util.Collections;
import java.util.List;

public class GCodeConsumer extends GCodeTransmitter {

    private TwoWaySerialCommunication serial;
    private PositionCache cache;
    private ConverterManager converterManager;

    private int maxCommandLength = 0;
    private int writtenCommands = 0;

    public GCodeConsumer(TwoWaySerialCommunication serial, GCodeQueue queue, PositionCache cache){
        super(queue);
        this.serial = serial;
        this.cache = cache;
        this.converterManager = new ConverterManager(cache.readPosition());
    }

    @Override
    public void run() {

        while (true) {
            try {
                String command = queue.getGCode();
                if(command != null){
                    command = command.trim();

                    boolean test = serial == null;
                    if(command.startsWith(TEST)){
                        test = true;
                        command = command.replace(TEST,  "");
                    }

                    List<String> convertedCommands = null;

                    GCodeEnum codeEnum = GCodeEnum.getEnum(command);
                    if(codeEnum == null) continue;
                    switch (codeEnum){
                        case G00:
                        case G01:
                        case G28:
                            convertedCommands = converterManager.convertCommand(command.trim());
                            break;
                        case G92:
                            converterManager.setHomePosition(CommandSectionEnum.XAxis, CommandSectionEnum.YAxis);
                            break;
                        case M02:
                            convertedCommands = Collections.singletonList(GCodeEnum.M02.value);
                            break;
                        case M114:
                            System.out.println(converterManager.getCurrentPositions(true));
                            break;
                        case F:
                            converterManager.trySetSpeed(command);
                            break;
                        default:
                            continue;
                    }

                    if(convertedCommands == null)
                        continue;

                    logger.info("From command: " + command + " - to position " + converterManager.getCurrentPositions(false));
//                    System.out.println("From command: " + command);
                    for(String convertedCommand : convertedCommands){

                        if(!convertedCommand.isEmpty()) {
                            logger.info(convertedCommand);
//                            System.out.println(convertedCommand);
                            if(!test){
                                String line;
                                do {
                                    line = serial.readLine();
        //                            System.out.println(line);
                                } while (!line.startsWith("GET_NEXT"));

                                serial.writeLine(convertedCommand);
                                cache.writePosition(converterManager.getCurrentPositions(false));
                            }

                            if(convertedCommand.length() > maxCommandLength)
                                maxCommandLength = convertedCommand.length();
                            writtenCommands++;
                        }
                    }

                    if (codeEnum == GCodeEnum.M02){
                        System.out.println("Commands executed: " + writtenCommands);
                        System.out.println("Max command length: " + maxCommandLength);
                        maxCommandLength = 0;
                        writtenCommands = 0;
                    }
                }

            } catch (Exception e){
                e.printStackTrace();
            }

        }

    }
}
