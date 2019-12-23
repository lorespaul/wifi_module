package com.lorenzodaneo.cnc.converter;

import java.math.BigDecimal;
import java.util.LinkedList;

public class ComputingJunction {

    public static final int MAX_LIST_SIZE = 50;
    private static final BigDecimal JUNCTION_DEVIATION = BigDecimal.valueOf(0.010);

    private static final int COMMANDS_PER_ESTIMATION_BACK = 5;
    private static final int COMMANDS_PER_ESTIMATION_AHEAD = 5;

    private LinkedList<ComputingCommand> commands = new LinkedList<>();

    void addComputingCommandsList(LinkedList<ComputingCommand> commands){
        for (ComputingCommand command : commands){
            this.commands.addLast(command);
        }
    }

    LinkedList<ComputingCommand> getFirsts(int quantity){
        LinkedList<ComputingCommand> firsts = new LinkedList<>();
        for (int i = 0; i < quantity && i < commands.size(); i++){
            firsts.addLast(commands.getFirst());
            commands.removeFirst();
        }
        return firsts;
    }

    LinkedList<ComputingCommand> getAll(){
        LinkedList<ComputingCommand> all = new LinkedList<>(commands);
        commands.clear();
        return all;
    }

    boolean isFull(){
        return commands.size() >= MAX_LIST_SIZE;
    }

    public void computeJunctionSpeed(BigDecimal maxAcceleration, boolean lastCommands){

    }

}
