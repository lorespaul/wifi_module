package com.lorenzodaneo.cnc;

import java.io.*;

/**
 * @author Lorenzo Daneo (mail to lorenzo.daneo@coolshop.it)
 * Coolshop Srl
 */
public class FileIO {

    private BufferedReader reader;
//    private PrintWriter writer;

    public FileIO(String fileName) throws IOException {
        reader = new BufferedReader(new FileReader(fileName));
//        File file = new File("/home/lorenzo/Desktop/executed.txt");
//        if(file.exists()){
//            file.delete();
//        }
//        file.createNewFile();
//        writer = new PrintWriter(new BufferedWriter(new FileWriter(file)));
    }


    public String getLine() throws IOException {
        return reader.readLine();
    }


//    public void writeLine(String line){
//        writer.println(line);
//    }


    public void close() throws IOException {
        reader.close();
//        writer.flush();
//        writer.close();
    }

}
