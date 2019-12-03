package com.lorenzodaneo.cnc.transmission;

public abstract class GCodeTransmitter implements Runnable {

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
