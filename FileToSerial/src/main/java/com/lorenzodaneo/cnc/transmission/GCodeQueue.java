package com.lorenzodaneo.cnc.transmission;

import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class GCodeQueue {

    private Queue<String> queue = new LinkedList<>();

    private Semaphore consumerSemaphore;
    private Semaphore producerSemaphore;

    public GCodeQueue(int bufferSize){
        consumerSemaphore = new Semaphore(0);
        producerSemaphore = new Semaphore(bufferSize);
    }


    String getGCode() {
        try {
            consumerSemaphore.acquire();
        } catch (InterruptedException e) {
            return null;
        }
        String gCode = queue.poll();
        producerSemaphore.release();
        return gCode;
    }


    void putGCode(String gCode) {
        try {
            producerSemaphore.acquire();
        } catch (InterruptedException e) {
            return;
        }
        queue.offer(gCode);
        consumerSemaphore.release();
    }


}
