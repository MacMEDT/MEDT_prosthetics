#define fsrpin1 A0
#define fsrpin2 A1
#define fsrpin3 A2
#define fsrpin4 A3
#define fsrpin5 A4
#define n 1 // number of force sensors added

  //int n =2; // number of force sensors
  int fsr[n]; // array for storing force sensor readings
  int lastreading[n];
  int queue[20]; //to store past fsr values

  int intensity_mod[n];
  int intensity[n];
  int timer[n];
  int counter =0;
  int average = 0;

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT); // vibration motor 1
  /*pinMode(6, OUTPUT); // vibration motor 2
  pinMode(3, OUTPUT); // vibration motor 2
  pinMode(10, OUTPUT); // vibration motor 2
  pinMode(11, OUTPUT); // vibration motor 2*/
  for (int i = 0; i<n; i++){
    fsr[i] = 0;
  }
}

void loop() {


  fsr[0] = analogRead(fsrpin1);
  /*fsr[1] = analogRead(fsrpin2);
  fsr[2] = analogRead(fsrpin3);
  fsr[3] = analogRead(fsrpin4);
  fsr[4] = analogRead(fsrpin5);   */  


  queue[counter] = fsr[0];
  for (int i=0; i<20; i++) {
      average+=queue[i];
  }      
  average = average/20;


  for (int i = 0; i<n; i++){
    intensity[i] = (int)((fsr[i] / 1050.0)*255.0); //multiply actual force by a factor of the possible max force

    if (lastreading[i] == 0 && fsr[i] >= 20){
      intensity[i] = 200; //provide a higher amplitude pulse at the beginning of touch
    } 
      Serial.print(fsr[0]);
      Serial.print(", ");    
      Serial.print(intensity[0]);
      Serial.print(", ");
      Serial.print(intensity_mod[0]);
      Serial.print(", ");
      Serial.print(average);      
      Serial.println();


   intensity[i] = (intensity[i] - intensity_mod[i]); //applying intensity modification factor
    if (intensity[i] < 0){ //remove negative intensities
      intensity[i] = 0;
    }
  }





  for(int i = 0; i<n; i++){ //deducing the intensity modification factor
    if (fsr[i] <10){
      lastreading[i] = 250; // checks if there's no force, which resets the modifications and flags that the last reading was 0 
      timer[i] = 0;
      intensity_mod[i] = 0;
    } else {
      lastreading[i] = 1; //sustained pressure will result in the intensity tapering off. 
      timer[i]+=50;

    if (fsr[0] <= average+80 && fsr[0] >= average-80){ //MUST TEST AND DETERMINE INTERVAL RANGE
      intensity_mod[i]+=10; //increase the intensity modification factor over time
      
    }
    else {
      intensity_mod[i] -= 20;
      if (intensity_mod[i]<0) {
        intensity_mod[i]=0;
      }
    }
    }
  analogWrite(5, intensity[0]); // write the intensity to vibration motors
  analogWrite(6, intensity[1]);
  analogWrite(3, intensity[2]);
  analogWrite(10, intensity[3]);
  analogWrite(11, intensity[4]);
  }
  counter = (counter+1)%20; //resets coutner to 0 when counter = 20
  delay(200);
}
