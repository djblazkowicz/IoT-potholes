// This #include statement was automatically added by the Particle IDE.
#include <HC-SR04.h>


const int triggerPin = A0;
const int echoPin = D0;
double inch;
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);

// We are going to declare a Particle.variable() here so that we can access the value of the photoresistor from the cloud.

    // This is saying that when we ask the cloud for "analogvalue", this will reference the variable analogvalue in this app, which is an integer variable.

    // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
//Particle.function("led",ledToggle);
    // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

void setup()
{
    Particle.variable("inch", inch);
    Serial.begin(9600);
    rangefinder.init();
}

void loop()
{
    unsigned long start = micros();
    inch = rangefinder.distInch();
    unsigned long calcTime = micros() - start;
    Serial.printf("Range finding duration: %lu | Distance in inches: %.2f\n", calcTime, inch);
    delay(500);
}
