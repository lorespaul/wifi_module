package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.converter.GCodeEnum;

import java.util.LinkedList;
import java.util.List;
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

    List<String> getGCodes(int count, GCodeEnum flushingCode){
        List<String> codes = new LinkedList<>();
        for (int i = 0; i < count; i++){
            try {
                consumerSemaphore.acquire();
            } catch (InterruptedException e) {
                return null;
            }
            String code = queue.poll();
            codes.add(code);
            producerSemaphore.release();
            if(GCodeEnum.getEnum(code) == flushingCode)
                break;
        }
        return codes;
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
            producerSemaphore.acquire();
        } catch (InterruptedException e) {
            return;
        }
        for (String gCode : gCodes)
            ((LinkedList<String>)queue).addFirst(gCode);
        for (int i = 0; i < gCodes.length; i++)
            consumerSemaphore.release();
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
