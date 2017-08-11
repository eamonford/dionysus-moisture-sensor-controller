#include "MQTT.h"
#include "PowerShield.h"

const int AirValue = 3385;
const int WaterValue = 1750;


byte mqttHost[] = { 192,168,86,100 };
MQTT mqttClient(mqttHost, 1883, mqttCallback);
PowerShield batteryMonitor;

void mqttCallback(char* topic, byte* payload, unsigned int length) {}

void setup() {
    batteryMonitor.begin();
    batteryMonitor.quickStart();

    pinMode(A0, INPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);

    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);

    Particle.function("read", takeReading);

    mqttClient.connect("sparkclient");
}

int sample() {
    int soilMoistureValue = analogRead(A0);
    int range = AirValue - WaterValue;
    int percentage = (soilMoistureValue - WaterValue) * 100 / range;
    if (percentage > 100) {
        percentage = 100;
    } else if (percentage < 0) {
        percentage = 0;
    }
    return 100 - percentage;
}

int takeReading(String samples) {
    int numSamples = 10;
    int parsed = atoi(samples);
    if (parsed > 0) {
        numSamples = parsed;
    }
    float average = 0;
    for (int i = 0; i < numSamples; i++) {
        average += (float)sample() / numSamples;
        delay(1000 / numSamples);
    }
    return (int)average;
}

void loop() {
  if (mqttClient.isConnected()) {
    String jsonString = "{\"device_id\":\"" + System.deviceID() +
                        "\",\"value\": " + String(takeReading("")) +
                        ",\"battery\": " + String(batteryMonitor.getSoC()) +
                        "}";

    mqttClient.publish("dionysus/moisture", jsonString);
    System.sleep(SLEEP_MODE_DEEP,600);
  } else {
    mqttClient.connect("sparkclient");
  }
}
