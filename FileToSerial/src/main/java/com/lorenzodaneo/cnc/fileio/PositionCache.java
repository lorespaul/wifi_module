package com.lorenzodaneo.cnc.fileio;

import java.io.*;

public class PositionCache {

    private static final String CACHE_FILENAME = "position_cache.txt";

    private BufferedReader reader;
    private PrintWriter writer = null;

    public PositionCache() throws IOException {
        File positionCacheFile = new File(CACHE_FILENAME);
        if(!positionCacheFile.exists()){
            if(!positionCacheFile.createNewFile())
                throw new IOException("Can't create cache file.");
        }
        reader = new BufferedReader(new FileReader(CACHE_FILENAME));
    }


    public String readPosition(){
        try {
            return reader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }


    public void writePosition(String position){
        if(writer != null)
            writer.close();
        writer = openWriter();
        if(writer != null){
            writer.print(position);
            writer.flush();
        }
    }


    private PrintWriter openWriter(){
        try {
            return new PrintWriter(new BufferedWriter(new FileWriter(CACHE_FILENAME, false)));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void close() throws IOException {
        if(reader != null)
            reader.close();
        if(writer != null)
            writer.close();
    }

}
