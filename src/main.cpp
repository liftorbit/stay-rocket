#include <Arduino.h>
#include "logging.h"
#include "sensors.h"
#include "rcs.h"

const int setupStatus = 1;
const int readyForLaunchStatus = 2;
const int launchStatus = 3;
const int tankChangeStatus = 4;
const int waitAltitudeStatus = 5;
const int landingStatus = 6;
const int landedStatus = 7;

GPS gps;
RCS rcs;
Logging logging;
Pressure bmp;
Acelerometer bmi;

void setup() {
    Serial.begin(9600);
    
    if(!logging.begin()) {
        Serial.println("Logging not started");
        while(true);
    }

    rcs.begin();
    logging.log(setupStatus, LOG_INFO, "STAY B Startup");

    // starting sensors
    logging.log(setupStatus, LOG_WAIT, "Starting sensors...");
    bool sensorStartupFailed = false;

    if(!bmi.begin()) {
        logging.log(setupStatus, LOG_ERROR, "Failed to start BMI160");
        sensorStartupFailed = true;
    }

    if(!bmp.begin()) {
        logging.log(setupStatus, LOG_ERROR, "Failed to start BMP280");
        sensorStartupFailed = true;
    }

    if(sensorStartupFailed) {
        logging.log(setupStatus, LOG_ERROR, "Sensor startup failure");
        rcs.sendLogs();
        while(true);
    }

    logging.log(setupStatus, LOG_SUCCESS, "All sensors started");

    // starting GPS
    logging.log(setupStatus, LOG_WAIT, "Wait GPS satellites...");
    gps.begin();
    logging.log(setupStatus, LOG_SUCCESS, "GPS started");

    rcs.sendLogs();
    bool hasReady = rcs.readyForLaunch();

    // restarting if rocket not ready for launch
    if(!hasReady) {
        ESP.restart();
    }
};

void loop() {

};