package com.lorenzodaneo.cnc;

import gnu.io.*;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Enumeration;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class TwoWaySerialCommunication {

    private SerialPort serialPort;

    private InputStream in;
    private OutputStream out;

    boolean connect(String portName) throws Exception {

        try
        {
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
                            230400,
                            SerialPort.DATABITS_8,
                            SerialPort.STOPBITS_1,
                            SerialPort.PARITY_NONE
                    );

                    in = serialPort.getInputStream();
                    out = serialPort.getOutputStream();

                } else {
                    System.out.println("Error: Only serial ports are handled.");
                    return false;
                }
            }
            Thread.sleep(timeout);
            return true;

        } catch (NoSuchPortException e) {
            System.out.println("-------------------------------------------------------------------------------");
            System.out.println("No serial port found. App will start in testing mode.");
            System.out.println("-------------------------------------------------------------------------------");
            return false;
        }
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
            line = line + "\r\n";
            this.out.write(line.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public void close(){
        try {
            in.close();
            out.close();
            serialPort.removeEventListener();
            serialPort.close();
            System.out.println("Serial communication closed.");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}