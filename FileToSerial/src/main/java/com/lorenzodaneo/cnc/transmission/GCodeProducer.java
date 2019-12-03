package com.lorenzodaneo.cnc.transmission;

import com.lorenzodaneo.cnc.FileIO;
import com.lorenzodaneo.cnc.converter.ConverterManager;

import java.io.IOException;
import java.util.Scanner;

public class GCodeProducer extends GCodeTransmitter {

    private GCodeQueue queue;
    private Scanner scanner = new Scanner(System.in);

    private static final String GCODE_FROM_FILE = "FILE";

    public GCodeProducer(GCodeQueue queue){
        super();
        this.queue = queue;
    }

    @Override
    public void run() {
        while (true){

            String input = scanner.nextLine();
            if(input.startsWith(GCODE_FROM_FILE)){

                String filename = "gcodes/" + input.replace(GCODE_FROM_FILE, "").trim();
                FileIO gCode;
                try {
                    gCode = new FileIO(filename);
                } catch (IOException e) {
                    System.out.println("File " + filename + " not found.");
                    continue;
                }

                String command;
                while ((command = gCode.getLine()) != null){
                    queue.putGCode(command);
                }

            } else {

                queue.putGCode(input);
                queue.putGCode(ConverterManager.FINAL_COMMAND);

            }

        }
    }

}
