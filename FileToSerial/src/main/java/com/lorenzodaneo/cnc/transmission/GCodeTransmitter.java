package com.lorenzodaneo.cnc.transmission;

import org.apache.log4j.Logger;

public abstract class GCodeTransmitter implements Runnable {

    Logger logger = Logger.getLogger(this.getClass());
    GCodeQueue queue;

    static final String TEST = "TEST";

    private Thread thread;

    GCodeTransmitter(GCodeQueue queue){
        thread = new Thread(this);
        this.queue = queue;
    }


    public void start(){
        thread.start();
    }

    public void join() throws InterruptedException {
        thread.join();
    }

}
