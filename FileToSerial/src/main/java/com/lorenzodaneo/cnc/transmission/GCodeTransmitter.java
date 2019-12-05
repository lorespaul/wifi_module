package com.lorenzodaneo.cnc.transmission;

import org.apache.log4j.Logger;

public abstract class GCodeTransmitter implements Runnable {

    Logger logger = Logger.getLogger(this.getClass());

    static final String TEST = "TEST";

    private Thread thread;

    GCodeTransmitter(){
        thread = new Thread(this);
    }


    public void start(){
        thread.start();
    }

    public void join() throws InterruptedException {
        thread.join();
    }

    public void stop(){
        thread.interrupt();
    }

}
