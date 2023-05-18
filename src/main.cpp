// STAY: Solid fuel propelled rocket
// Copyright (C) 2023  Liftorbit

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Arduino.h>
#include <Servo.h>
#include "logging.h"
#include "barometer.h"
#include "imu.h"
#include "rcs.h"

const int statusLedPin = 25;
const int flameSensorPin = 35;
const int mainEngineIgnitionPin = 26;

const int servoXPin = 2;
const int servoYPin = 32;

RCS rcs;
IMU imu;
Logging logging;
Barometer barometer;

Servo servoX;
Servo servoY;

void launchCountdown();
bool engineIsOn();
void mainEngineIgnition();
void launch();

void setup() {
    Serial.begin(9600);

    if(!logging.begin()) {
        Serial.println(F("Logging not started"));
        while(true);
    }

    // setup status
    logging.log(S_SETUP, LOG_INFO, F("STAY B Startup"));

    pinMode(statusLedPin, OUTPUT);
    pinMode(mainEngineIgnitionPin, OUTPUT);
    pinMode(flameSensorPin, INPUT);

    logging.log(S_SETUP, LOG_INFO, F("Pin mode defined"));
    logging.log(S_SETUP, LOG_WAIT, F("Wait RCS connection..."));

    String logDate = rcs.begin();
    logging.log(S_SETUP, LOG_INFO, "Computer startup in " + logDate);

    logging.log(S_SETUP, LOG_WAIT, F("Starting sensors..."));

    if(imu.begin()) {
        imu.updatePosition();
        String imuX = String(imu.getAccelerometerX());
        String imuY = String(imu.getAccelerometerY());
        logging.log(S_SETUP, LOG_SUCCESS, F("IMU Started"));
        logging.log(S_SETUP, LOG_INFO, "IMU x(" + imuX + ") y(" + imuY + ")");
    } else {
        logging.log(S_SETUP, LOG_ERROR, F("Failed to start IMU"));
    }

    if(barometer.begin()) {
        String alt = String(barometer.getAltitude());
        String temp = String(barometer.getTemperature());
        logging.log(S_SETUP, LOG_SUCCESS, F("Barometer Started"));
        logging.log(S_SETUP, LOG_INFO, "BAROMETER alt(" + alt + ") temp(" + temp + ")");
    } else {
        logging.log(S_SETUP, LOG_ERROR, F("Failed to start Barometer"));
    }

    if(engineIsOn()) {
        logging.log(S_SETUP, LOG_ERROR, F("Flame detected before launch"));
    } else {
        logging.log(S_SETUP, LOG_SUCCESS, F("Flame sensor in operation"));
    }

    logging.log(S_SETUP, LOG_INFO, F("Sensors started"));

    servoX.attach(servoXPin);
    servoY.attach(servoYPin);
    servoX.write(90);
    servoY.write(90);

    logging.log(S_SETUP, LOG_INFO, F("Servos attached"));
    logging.log(S_AUTH, LOG_WAIT, F("Wait launch authorization..."));

    rcs.sendLogs();

    int launchAuth = rcs.waitAuthorization();

    while(launchAuth == RCS_WITHOUT_DATA) {
        digitalWrite(statusLedPin, HIGH);
        delay(100);
        digitalWrite(statusLedPin, LOW);
        delay(100);
        digitalWrite(statusLedPin, HIGH);
        delay(100);
        digitalWrite(statusLedPin, LOW);
        delay(500);
        launchAuth = rcs.waitAuthorization();
    }

    if(launchAuth == LAUNCH_AUTHORIZED) {
        logging.log(S_AUTH, LOG_INFO, F("Launch authorized. Countdown."));

        // rocket action sequence
        launchCountdown();
        launch();
    } else if(launchAuth == RCS_DISCONNECTED || launchAuth == NO_AUTHORIZED) {
        logging.log(S_SETUP, LOG_INFO, F("Launch not authorized, restarting"));
        delay(1000);
        ESP.restart();
    }
};

void launchCountdown() {
    // 10 seconds countdown
    for(int i = 0; i < 10; i++) {
        digitalWrite(statusLedPin, HIGH);
        delay(500);
        digitalWrite(statusLedPin, LOW);
        delay(500);
    }
}

bool engineIsOn() {
    return !digitalRead(flameSensorPin);
}

void mainEngineIgnition() {
    // waits for ignition and then disconnects the ignition pin
    digitalWrite(mainEngineIgnitionPin, HIGH);
    while(!engineIsOn());
    digitalWrite(mainEngineIgnitionPin, LOW);
}

void launch() {
    int maxAltitude = 0, currentAltitude = 0;
    float rawX, rawY, maxSpeed, temperature;

    barometer.saveGroundAltitude();

    logging.log(S_LAUNCH, LOG_INFO, F("Main engine ignite"));
    mainEngineIgnition();

    while (static_cast<int>(barometer.getGroundDistance()) < 1);
    logging.log(S_LAUNCH, LOG_INFO, F("Liftoff"));

    // wait engine cut off
    while(engineIsOn()) {
        // control TVC
        imu.updatePosition();
        rawX = imu.getAccelerometerX();
        rawY = imu.getAccelerometerY();
        servoX.write(imu.convertAxesToServoTuning(rawX));
        servoY.write(imu.convertAxesToServoTuning(rawY));
    }

    // colecting data
    maxSpeed = barometer.getAverageSpeed(500);
    temperature = barometer.getTemperature();

    logging.log(S_LAUNCH, LOG_INFO, F("Main engine cut off"));

    while(true) {
        currentAltitude = barometer.getGroundDistance();
        if(currentAltitude >= maxAltitude) {
            maxAltitude = currentAltitude;
        } else {
            break;
        }
    }

    logging.log(S_LAUNCH, LOG_INFO, "Temperature: " + String(temperature) + " °C");
    logging.log(S_LAUNCH, LOG_INFO, "Max altitude: " + String(maxAltitude) + " m");
    logging.log(S_LAUNCH, LOG_INFO, "Max speed: " + String(maxSpeed) + " m/s");
}

void loop() {

};