/*
this sketch is used to detect if the measured distance is above the treshold for a long enough timeline, then upload the measurement to google cloud database.
Gabor Nemeth 2020
TODO:
-Communication with PI ZERO CAM
*/
#include <TinyGPS.h>
// ultrasonic rangefinder lib
#include <HC-SR04.h>


TinyGPS gps;
char szInfo[64];
const int triggerPin = A0;
const int echoPin = D0;
const int HISTORY_LIMIT = 1000;
const int SAMPLE_RATE = 100;
const double TRESHOLD = 166.0;
int counter = 0,inch_history_counter = 0, pothole_meter = 0;
double inch,avg_inch = 0,inch_history[HISTORY_LIMIT],baseline;
bool check_spike = false;
int pothole_detected = 0;
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);

void setup()
{
    Particle.variable("inch", inch);
    Particle.variable("pothole_detected", pothole_detected);
    Serial.begin(9600);
    Serial1.begin(9600);
    rangefinder.init();
    //get average distance from sensor as baseline so we have something to measure against
    baseline = get_average_inch();

}

void loop()
{
    static int prev_ms = millis();
    if ((millis() - prev_ms) > 1000 / SAMPLE_RATE)
    {
        inch = rangefinder.distInch();
        check_spike = spike(baseline,inch,TRESHOLD);
        if (check_spike)
        {
            pothole_meter++;
            //Serial.printf("POTHOLE METER: " + String(pothole_meter) + "\n");
            
        } else {
            
            pothole_detected = 0;
            
            if (pothole_meter > 500) 
            
            {
                (Serial.printf("POTHOLE SIZE: " + String(pothole_meter) + "\n"));

                getGPSloc();
                String cSTR = szInfo;
                String returnstring = "pothole detected," + cSTR;
                Particle.publish("my_event", returnstring, PRIVATE);
                pothole_meter = 0;
                pothole_detected = 1;
            } else
            {
                pothole_meter = 0;
            }
        }
    }

}

// this is used to establish a baseline distance
double get_average_inch() {

    double buffer = 0;
    
    for (int i = 0; i < HISTORY_LIMIT + 1; i++)
    {
            inch = rangefinder.distInch();
            inch_history[inch_history_counter] = inch;
            inch_history_counter++;
            if (inch_history_counter > HISTORY_LIMIT - 1)
            {
                inch_history_counter = 0;
                for (int i = 0; i < HISTORY_LIMIT + 1; i++)
                {
                    buffer = buffer + inch_history[i];
                //Serial.printf("BUFFER: " + String(buffer) + "\n");
                }
            buffer = buffer / double(HISTORY_LIMIT);
            //Serial.printf("DISTANCE: " + String(buffer) + "\n");
            }

    }
    
    
    return buffer;
}

//this is to check if distance is beyond treshold
bool spike(double baseline, double current, double treshold) 
{
    double percent = (current / baseline) * 100;
    //Serial.printf("PERCENT: " + String(percent) + "\n");
    
    if (percent > treshold)
    {
        return true;
    } else {
        return false;
    }
    
}

void getGPSloc()

{
    //TinyGPS gps;
    bool isValidGPS = false;
    
    
    for (unsigned long start = millis(); millis() - start < 1000;){
        // Check GPS data is available
        while (Serial1.available()){
            char c = Serial1.read();
            
            // parse GPS data
            if (gps.encode(c))
                isValidGPS = true;
        }
    }

    // If we have a valid GPS location then publish it
    if (isValidGPS){
        float lat, lon;
        unsigned long age;
    
        gps.f_get_position(&lat, &lon, &age);
        
        sprintf(szInfo, "%.6f,%.6f", (lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat), (lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon));
    }
    else{
        // Not a vlid GPS location, jsut pass 0.0,0.0
        // This is not correct because 0.0,0.0 is a valid GPS location, we have to pass a invalid GPS location
        // and check it at the client side
        sprintf(szInfo, "0.0,0.0");
    }
    
        Spark.publish("gpsloc", szInfo);
    
}
