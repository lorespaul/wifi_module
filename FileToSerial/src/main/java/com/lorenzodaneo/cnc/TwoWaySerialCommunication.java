package com.lorenzodaneo.cnc;

import gnu.io.*;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class TwoWaySerialCommunication {

    SerialPort serialPort;

    InputStream in;
    OutputStream out;

    boolean connect(String portName) throws Exception {
        CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier(portName);

        int timeout = 2000;
        if (portIdentifier.isCurrentlyOwned()) {
            System.out.println("Error: Port is currently in use");
            return false;
        } else {
            CommPort commPort = portIdentifier.open(this.getClass().getName(), timeout);

            if (commPort instanceof SerialPort) {
                serialPort = (SerialPort) commPort;
                serialPort.setSerialPortParams(
                        115200,
                        SerialPort.DATABITS_8,
                        SerialPort.STOPBITS_1,
                        SerialPort.PARITY_NONE
                );

                in = serialPort.getInputStream();
                out = serialPort.getOutputStream();

            } else {
                System.out.println("Error: Only serial ports are handled by this example.");
                return false;
            }
        }
        Thread.sleep(timeout);
        return true;
    }


    public String readLine(){
        byte[] buffer = new byte[1];
        StringBuilder builder = new StringBuilder();
        try {
            int len;
            while((len = this.in.read(buffer)) > -1) {
                builder.append(new String(buffer, 0, len));
                if(builder.toString().endsWith("\r\n")){
                    break;
                }
            }
        } catch(IOException e) {
            e.printStackTrace();
        }
        return builder.toString().replace("\r\n", "");
    }

    public void writeLine(String line){
        try {
            this.out.write(line.getBytes());
            this.out.flush();
        } catch( IOException e ) {
            e.printStackTrace();
        }
    }



    public void close(){
        try {
            in.close();
            out.close();
            serialPort.removeEventListener();
            serialPort.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}