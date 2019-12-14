package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.converter.CommandSectionEnum;
import com.lorenzodaneo.cnc.converter.GCodeEnum;
import com.lorenzodaneo.cnc.fileio.GCodeReader;
import com.lorenzodaneo.cnc.converter.ConverterManager;
import com.lorenzodaneo.cnc.fileio.PositionCache;
import com.lorenzodaneo.cnc.listeners.IKeyInputReceiver;
import com.lorenzodaneo.cnc.listeners.KeyEnum;
import com.lorenzodaneo.cnc.listeners.KeyInputListener;

import java.io.IOException;
import java.math.BigDecimal;
import java.util.Scanner;

public class GCodeProducer extends GCodeTransmitter implements IKeyInputReceiver {

    private Scanner scanner = new Scanner(System.in);

    private Thread fileWorker = null;

    private PositionCache cache;

    private static final String GCODE_FROM_FILE = "FILE";
    private static final String STOP_FILE = "STOP";
    private static final String PAUSE_FILE = "PAUSE";
    private static final String RESUME_FILE = "RESUME";

    private final Object pauseLock = new Object();
    private boolean paused = false;
    private boolean stopped = true;

    public GCodeProducer(GCodeQueue queue, PositionCache cache){
        super(queue);
        this.cache = cache;
        KeyInputListener keyListener = new KeyInputListener();
        keyListener.setReceiver(this);
        keyListener.setVisible(true);
    }

    @Override
    public void run() {
        while (true){

            try{
                String input = scanner.nextLine();
                String temp = input;
                input = input.toUpperCase();

                try {
                    if(fileWorker != null && !fileWorker.isInterrupted()){
                        if(!getStopped() && input.startsWith(STOP_FILE)){
                            if(getPaused()){
                                setPaused(false);
                                synchronized (pauseLock){
                                    pauseLock.notifyAll();
                                }
                            }
                            fileWorker.interrupt();
                            setStopped(true);
                            fileWorker = null;
                            queue.clean();

                        } else if(!getPaused() && input.startsWith(PAUSE_FILE)){
                            setPaused(true);
                        }
                    }
                    if(getPaused() && fileWorker != null && input.startsWith(RESUME_FILE)) {
                        setPaused(false);
                        synchronized (pauseLock){
                            pauseLock.notify();
                        }
                    }
                } catch (Exception e){
                    logger.error(e);
                }

                if(fileWorker == null && input.startsWith(GCODE_FROM_FILE)){

                    String filename = temp.substring(GCODE_FROM_FILE.length()).trim();
                    if(input.contains(TEST)){
                        filename = filename.substring(TEST.length()).trim();
                    }

                    filename = "gcodes/" + filename;
                    GCodeReader gCode;
                    try {
                        gCode = new GCodeReader(filename);
                    } catch (IOException e) {
                        System.out.println("File " + filename + " not found.");
                        continue;
                    }

                    final String finalFilename = filename;
                    final boolean isTest = input.contains(TEST);
                    fileWorker = new Thread(() -> {
                        String command;
                        System.out.println("Start processing file " + finalFilename);
                        while ((command = gCode.getLine()) != null){
                            if(getStopped()){
                                System.out.println("FILE STOPPED.");
                                break;
                            }
                            if(getPaused()) {
                                try {
                                    System.out.println("FILE PAUSED.");
                                    synchronized (pauseLock){
                                        pauseLock.wait();
                                    }
                                    System.out.println("FILE RESUMED.");
                                } catch (InterruptedException e) {
                                    logger.error(e);
                                    Thread.currentThread().interrupt();
                                }
                            }
                            if(!command.isEmpty()){
                                if(isTest)
                                    command = TEST + command;
                                queue.putGCode(command);
                            }
                        }
                        gCode.close();
                        setStopped(true);
                        fileWorker = null;
                    });
                    setStopped(false);
                    fileWorker.start();

                } else if(GCodeEnum.isValidGCode(input)) {

                    if(input.equals(GCodeEnum.G28.value.get(0)))
                        positions = null;
                    queue.putGCodesOnTop(GCodeEnum.M02.value.get(0), input);

                }

            } catch (Exception e){
                e.printStackTrace();
            }
        }
    }


    private synchronized boolean getPaused(){
        return paused;
    }

    private synchronized void setPaused(boolean paused){
        this.paused = paused;
    }

    private synchronized boolean getStopped(){
        return stopped;
    }

    private synchronized void setStopped(boolean stopped){
        this.stopped = stopped;
    }

    private static final String HOME_POSITION = "X0.0000 Y0.0000 Z0.0000";
    private static final BigDecimal SIX = BigDecimal.valueOf(6);
    private String positions = null;

    @Override
    public void gotKey(KeyEnum key) {
        if(fileWorker == null){

            if (positions == null)
                positions = cache.readPosition();
            if(positions == null || positions.isEmpty())
                positions = HOME_POSITION;

            String axisPosition = null;
            char axisKey = ' ';
            switch (key){
                case ArrowTop:
                    axisPosition = changeXYPosition(positions, CommandSectionEnum.YAxis, true);
                    axisKey = CommandSectionEnum.YAxis.value;
                    break;
                case ArrowDown:
                    axisPosition = changeXYPosition(positions, CommandSectionEnum.YAxis, false);
                    axisKey = CommandSectionEnum.YAxis.value;
                    break;
                case ArrowLeft:
                    axisPosition = changeXYPosition(positions, CommandSectionEnum.XAxis, true);
                    axisKey = CommandSectionEnum.XAxis.value;
                    break;
                case ArrowRight:
                    axisPosition = changeXYPosition(positions, CommandSectionEnum.XAxis, false);
                    axisKey = CommandSectionEnum.XAxis.value;
                    break;
                case ZChar:
                    axisPosition = changeZPosition(positions);
                    axisKey = CommandSectionEnum.ZAxis.value;
                    break;
            }
            if(axisPosition != null) {
                queue.putGCode(GCodeEnum.G00.value.get(0) + " " + axisPosition);
                queue.putGCode(GCodeEnum.M02.value.get(0));
                if(axisKey != ' '){
                    positions = positions.replaceAll(axisKey + "-?\\d+(\\.\\d+)?", axisPosition);
                }
            }
        }
    }

    private String changeXYPosition(String position, CommandSectionEnum section, boolean add){
        String currentValueString = ConverterManager.getSection(position, section);
        if(currentValueString != null){
            BigDecimal currentValue = new BigDecimal(currentValueString.replace(String.valueOf(section.value), ""));
            if(add){
                return section.value + currentValue.add(BigDecimal.ONE).toString();
            } else if(currentValue.compareTo(BigDecimal.ZERO) > 0) {
                return section.value + currentValue.subtract(BigDecimal.ONE).toString();
            }
        }
        return null;
    }

    private String changeZPosition(String position){
        String currentValueString = ConverterManager.getSection(position, CommandSectionEnum.ZAxis);
        if(currentValueString != null){
            BigDecimal currentValue = new BigDecimal(currentValueString.replace(String.valueOf(CommandSectionEnum.ZAxis.value), ""));
            if(currentValue.compareTo(BigDecimal.ZERO) == 0){
                return CommandSectionEnum.ZAxis.value + currentValue.add(SIX).toString();
            } else {
                return CommandSectionEnum.ZAxis.value + currentValue.subtract(SIX).toString();
            }
        }
        return null;
    }

}
