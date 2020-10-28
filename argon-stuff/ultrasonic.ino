#include <HC-SR04.h>

const int triggerPin = A0;
const int echoPin = D0;
const int HISTORY_LIMIT = 1000;
const int SAMPLE_RATE = 100;
const double TRESHOLD = 120.0;
int counter = 0,inch_history_counter = 0, pothole_meter = 0;
double inch,avg_inch = 0,inch_history[HISTORY_LIMIT],baseline;
bool check_spike = false;
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);

void setup()
{
    Particle.variable("inch", inch);
    Serial.begin(9600);
    rangefinder.init();
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
            Serial.printf("POTHOLE METER: " + String(pothole_meter) + "\n");
            
        } else {
            if (pothole_meter > 100) 
            {
                (Serial.printf("POTHOLE SIZE: " + String(pothole_meter) + "*******************************************************************************************************************\n"));
                Particle.publish("my_event", String(pothole_meter), PRIVATE);
                pothole_meter = 0;
            } else
            {
                pothole_meter = 0;
            }
        }
    }

}


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


bool spike(double baseline, double current, double treshold) 
{
    double percent = (current / baseline) * 100;
    //Serial.printf("PERCENT: " + String(percent) + "\n");
    
    if (percent > treshold)
    {
        return true;
    }else {
        return false;
    }
    
}
