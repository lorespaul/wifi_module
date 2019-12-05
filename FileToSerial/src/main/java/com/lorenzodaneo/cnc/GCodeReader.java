package com.lorenzodaneo.cnc;

import org.apache.log4j.Logger;

import java.io.*;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class GCodeReader {

    private BufferedReader reader;

    public GCodeReader(String fileName) throws IOException {
        reader = new BufferedReader(new FileReader(fileName));
    }


    public String getLine() {
        try {
            return reader.readLine();
        } catch (IOException e) {
            System.out.println("Read line from g-code file breaks.");
            return null;
        }
    }


    void close() throws IOException {
        reader.close();
    }

}
