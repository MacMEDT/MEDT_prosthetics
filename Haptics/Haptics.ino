#define fsrpin1 A0
#define fsrpin2 A1
#define fsrpin3 A2
#define fsrpin4 A3
#define fsrpin5 A4
#define n 5 // number of force sensors added

  //int n =2; // number of force sensors
  int fsr[n]; // array for storing force sensor readings
  int lastreading[n];

  float intensity_mod[n];
  int timer[n];

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT); // vibration motor 1
  pinMode(6, OUTPUT); // vibration motor 2
  pinMode(3, OUTPUT); // vibration motor 2
  pinMode(10, OUTPUT); // vibration motor 2
  pinMode(11, OUTPUT); // vibration motor 2
  for (int i = 0; i<n; i++){
    fsr[i] = 0;
  }
}

void loop() {
  fsr[0] = analogRead(fsrpin1);
  fsr[1] = analogRead(fsrpin2);
  fsr[2] = analogRead(fsrpin3);
  fsr[3] = analogRead(fsrpin4);
  fsr[4] = analogRead(fsrpin5);      

  int intensity[n];
  for (int i = 0; i<n; i++){

    Serial.println(fsr[2]);
    Serial.println();

    intensity[i] = (int)((fsr[i] / 1050.0)*255.0); //multiply actual force by a factor of the possible max force

    if (lastreading[i] == 0 && fsr[i] >= 20){
      intensity[i] = 200; //provide a higher amplitude pulse at the beginning of touch
    } 
    if (intensity[i] < 0){ //remove negative intensities
      intensity[i] = 0;
    }

    intensity[i] = intensity[i] - (int)intensity_mod[i]; //applying intensity modification factor
    if (intensity[i] < 70 && intensity[i] >20){
      intensity[i] = 80; //sets minimum for PWM to 80 because intensity below ~70 doesn't actually produce vibration.
    } 
  }



  analogWrite(5, intensity[0]); // write the intensity to vibration motors
  analogWrite(6, intensity[1]);
  analogWrite(3, intensity[2]);
  analogWrite(10, intensity[3]);
  analogWrite(11, intensity[4]);

  for(int i = 0; i<n; i++){ //deducing the intensity modification factor
    if (fsr[i] <10){
      lastreading[i] = 0; // checks if there's no force, which resets the modifications and flags that the last reading was 0 
      timer[i] = 0;
      intensity_mod[i] = 0;
    } else {
      lastreading[i] = 1; //sustained pressure will result in the intensity tapering off. 
      timer[i]+=50;
      if (timer[i] > 5000){
        intensity_mod[i]+=0.25; //increase the intensity modification factor over time
      }
    }
  }

  delay(200);
}
