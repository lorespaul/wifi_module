package com.lorenzodaneo.cnc.transmission;

import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class GCodeQueue {

    private int bufferSize;

    private Queue<String> queue = new LinkedList<>();

    private Semaphore consumerSemaphore;
    private Semaphore producerSemaphore;

    public GCodeQueue(int bufferSize){
        this.bufferSize = bufferSize;
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

    void putGCodesOnTop(String... gCodes){
        try {
            producerSemaphore.acquire(gCodes.length);
        } catch (InterruptedException e) {
            return;
        }
        for (String gCode : gCodes)
            ((LinkedList<String>)queue).addFirst(gCode);
        consumerSemaphore.release(gCodes.length);
    }

    void clean(){
        Semaphore tmpConsumer = consumerSemaphore;
        Semaphore tmpProducer = producerSemaphore;
        consumerSemaphore = new Semaphore(0);
        producerSemaphore = new Semaphore(bufferSize);
        tmpConsumer.release();
        tmpProducer.release();
        queue = new LinkedList<>();
    }


}
