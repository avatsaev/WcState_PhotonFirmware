
// Bathroom Status Notifier Firmware
// Author: Vatsaev Aslan (@avatsaev)
// Firmware Version: 0.1
// Build: 1


//This firmware is burned directly into the Photon Board, allows it sur read the sensors, update states, and send events to the cloud.

/*
–-----------Event list categorised by prefix---------------------

[bathroom]:
    bathroom-light-status : is fired when sensor detects that the light has been on for more than 15 minutes
        data: 0/1 : 0 for off, 1 for on


–-----------Cloud variables list---------------------------------

light_state: INT
    0/1: 0 for off, 1 for on

------------Cloud functions list---------------------------------


-----------------------------------------------------------------
*/

//analog values for photosensor
#define LIGHT_ON_THRESHOLD 3500 //above this threshold light is considered as ON
#define LIGHT_OFF_THRESHOLD 2900 //below this threshold light is considered as OFF

IntervalTimer timer1; //timer for 15 minutes to notify the user

char publishString[40];
int photoresistor = A0; // Photoresistor Input
int statusLED = D7;
int power = A1; // Supplies power to the Photoresistor
int analogvalue; // Reads analogic value from the Photoresistor
int light_state = 0; // Counts the amout of mails slided into the mailbox (if several mails are slided at the same time, they'll be counted as one )


// Pre-define ISR callback functions
void notify(void);
void init(void);
void setLightStatus(int);

// Next we go into the setup function.

void setup() {


    // First, declare all of our pins. This lets our device know which ones will be used for outputting voltage, and which ones will read incoming voltage.


    pinMode(power,OUTPUT); // The pin powering the photoresistor is output (sending out consistent power)
    pinMode(statusLED, OUTPUT);

    //Serial.begin(9600);

    // Next, write the power of the photoresistor to be the maximum possible, so that we can use this for power.
    digitalWrite(power,HIGH);

    init();

    Spark.variable("light_state", light_state);

}

void loop() {

    // check to see what the value of the photoresistor is and store it in the int variable analogvalue
    analogvalue = analogRead(photoresistor);

    if(analogvalue > LIGHT_ON_THRESHOLD && light_state != 1){
        setLightStatus(1);
        //sprintf(publishString, "Turn off the lights (light sensor reading: %d)", light_state);
        //Spark.publish("bathroom-light-status", publishString);

    }

    if(analogvalue < LIGHT_OFF_THRESHOLD && light_state != 0){
        setLightStatus(0);
        //timer1.end();
    }

    //Serial.printf("analog: %d\n", analogvalue);
}

void setLightStatus(int status){

  if(status){
    light_state = 1;
    digitalWrite(statusLED, HIGH);
  }else{
    light_state = 0;
    digitalWrite(statusLED, LOW);
  }

  Spark.variable("light_state", light_state);

  notify();


}

void init(){
    analogvalue = analogRead(photoresistor);
    if(analogvalue > LIGHT_ON_THRESHOLD) setLightStatus(1);
    if(analogvalue < LIGHT_OFF_THRESHOLD) setLightStatus(0);
}

void notify(){
    sprintf(publishString, "%d", light_state);
    Particle.publish("light_state_changed", publishString);
}
