const int AirValue = 3385;
const int WaterValue = 1750;

void setup() {
    pinMode(A0, INPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);

    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);

    Particle.function("read", takeReading);
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

void loop() {}
