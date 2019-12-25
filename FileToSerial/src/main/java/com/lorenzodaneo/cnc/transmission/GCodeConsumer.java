package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.TwoWaySerialCommunication;
import com.lorenzodaneo.cnc.converter.CommandSectionEnum;
import com.lorenzodaneo.cnc.converter.ConverterManager;
import com.lorenzodaneo.cnc.converter.GCodeEnum;
import com.lorenzodaneo.cnc.fileio.PositionCache;

import java.util.*;
import java.util.stream.Collectors;

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

        mainLoop:while (true) {
            try {
                String command = queue.getGCode();

//                Map<GCodeEnum, List<String>> movementCommands = new HashMap<>();
                boolean test = serial == null, flush = false;
                List<String> convertedCommands = null;
                if (command != null) {
                    command = command.trim();

                    if (command.startsWith(TEST)) {
                        test = true;
                        command = command.replace(TEST, "");
                    }


                    GCodeEnum codeEnum = GCodeEnum.getEnum(command);
                    if (codeEnum == null) continue;
                    switch (codeEnum) {
                        case M02:
                            flush = true;
                        case G00:
                        case G01:
                        case G28:
                            convertedCommands = converterManager.convertCommands(command);
                            break;
                        case G92:
                            converterManager.setHomePosition(CommandSectionEnum.XAxis, CommandSectionEnum.YAxis, CommandSectionEnum.ZAxis);
                            continue mainLoop;
                        case M114:
                            System.out.println(converterManager.getCurrentPositions(true));
                            continue mainLoop;
                        case F:
                            converterManager.trySetSpeed(command);
                            continue mainLoop;
                    }
                }
//                }

//                logger.info("From command: " + command + " - to position " + converterManager.getCurrentPositions(false));
//                    System.out.println("From command: " + command);

//                boolean flush = false;
//                for (GCodeEnum c : movementCommands.keySet()) {
//                    if(c == GCodeEnum.M02){
//                        flush = true;
//                        break;
//                    }
//                }

//                convertedCommands = converterManager.convertCommands(movementCommands.values().stream().flatMap(List::stream).collect(Collectors.toList()), flush);
//                if(flush)
//                    convertedCommands.add(GCodeEnum.M02.value.get(0));
                if(convertedCommands == null)
                    continue;
                if(convertedCommands.isEmpty())
                    convertedCommands.add("");

                for(int i = 0; i < convertedCommands.size(); i++){
                    String convertedCommand = convertedCommands.get(i);

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

                    if (flush && i == convertedCommands.size() - 1){
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
