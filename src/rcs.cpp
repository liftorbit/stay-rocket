// RCS - Rocket Communication System

#include <Arduino.h>
#include <BluetoothSerial.h>
#include "logging.h"
#include "rcs.h"

BluetoothSerial bt;
Logging testLogging;

void RCS::begin() {
    bt.begin("STAY B Rocket");

    while(!bt.connected()) {
        delay(100);
    }
};

bool RCS::readyForLaunch() {
    while(bt.connected()) {
        if(bt.available()) {
            String confirm = bt.readString();
            bt.print(confirm);
            return confirm == "rfl"; // rfl = ready for launch
        }
    }

    return false;
}

bool RCS::authorizedLaunch() {
    while(bt.connected()) {
        if(bt.available()) {
            String confirm = bt.readString();
            bt.print(confirm);
            return confirm == "la"; // la = launch authorized
        }
    }

    return false;
}

void RCS::sendLogs() {
    bt.print(testLogging.getLog());
}