package com.lorenzodaneo.cnc.listeners;

import javax.swing.*;
import java.awt.*;
import java.awt.event.AWTEventListener;
import java.awt.event.KeyEvent;

public class KeyInputListener extends JFrame {

    private IKeyInputReceiver receiver;

    public KeyInputListener() {
        Toolkit defaultTookKit = Toolkit.getDefaultToolkit();
        defaultTookKit.addAWTEventListener(event -> {
            if(event instanceof KeyEvent && event.getID() == KeyEvent.KEY_PRESSED){
                KeyEvent keyEvent = (KeyEvent)event;
                KeyEnum enumValue = KeyEnum.getEnum(keyEvent.getKeyCode());
                if(enumValue != null)
                    receiver.gotKey(enumValue);
            }

        }, AWTEvent.KEY_EVENT_MASK);

//        AWTEventListener[] listeners = defaultTookKit.getAWTEventListeners();
//        System.out.println(listeners.length);
        setSize(0, 0);
        pack();
    }

    public void setReceiver(IKeyInputReceiver receiver){
        this.receiver = receiver;
    }

}
