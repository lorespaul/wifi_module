package com.lorenzodaneo.cnc;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class GCodeFileReader {

    private BufferedReader reader;

    public GCodeFileReader(String fileName) throws FileNotFoundException {
        reader = new BufferedReader(new FileReader(fileName));
    }


    public String getCommand() throws IOException {
        return reader.readLine();
    }

    public void close() throws IOException {
        reader.close();
    }

}
