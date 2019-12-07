package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.fileio.GCodeReader;
import com.lorenzodaneo.cnc.converter.ConverterManager;

import java.io.IOException;
import java.util.Scanner;

public class GCodeProducer extends GCodeTransmitter {

    private GCodeQueue queue;
    private Scanner scanner = new Scanner(System.in);

    private Thread fileWorker = null;

    private static final String GCODE_FROM_FILE = "FILE";
    private static final String STOP_FILE = "STOP";
    private static final String PAUSE_FILE = "PAUSE";
    private static final String RESUME_FILE = "RESUME";

    private final Object pauseLock = new Object();
    private boolean paused = false;
    private boolean stopped = true;

    public GCodeProducer(GCodeQueue queue){
        super();
        this.queue = queue;
    }

    @Override
    public void run() {
        while (true){

            try{
                String input = scanner.nextLine();

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

                    String filename = input.replace(GCODE_FROM_FILE, "").trim();
                    if(filename.startsWith(TEST)){
                        filename = filename.replace(TEST, "").trim();
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
                                if(input.contains(TEST))
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

                } else if(input.equals("G28") || input.matches("^G0[012].*") || input.matches("^F\\d+.*")) {

                    queue.putGCodesOnTop(ConverterManager.FINAL_COMMAND, input);

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

}
