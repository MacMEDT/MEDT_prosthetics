#include <Servo.h>
#define fsrpin1 A0
#define fsrpin2 A1
#define fsrpin3 A2
#define fsrpin4 A3
#define fsrpin5 A4
#define n 1 // number of force sensors added

Servo servo1;
int pos=0;
int val = 0;
int target;

int fsr[n]; // array for storing force sensor readings
int lastreading[n];
 
int intensity_mod[n];
int timer[n];

bool haptics();
bool direction;
bool flag = false;

void setup() {
  // put your setup code here, to run once:
  servo1.attach(9);
  servo1.write(0);
  Serial.begin(9600);
  pinMode(5, OUTPUT); // vibration motor 1
  pinMode(6, OUTPUT); // vibration motor 2
  pinMode(3, OUTPUT); // vibration motor 2
  pinMode(10, OUTPUT); // vibration motor 2
  pinMode(11, OUTPUT); // vibration motor 2
  direction = true; //closing = true, opening = false
  for (int i = 0; i<n; i++){
    fsr[i] = 0;
  }
}

void loop() {
  Serial.print("loop");
  //read value
  Serial.println(Serial.available());
  while (Serial.available() == 0){flag = haptics();}
  val = Serial.parseFloat();
  if(val>5 && !direction){ // opening 
    Serial.print("IF 1");
    direction = !direction; 
    Serial.println(direction);
    target = 0;
    while(target<pos){
      pos-=1;
      flag = haptics();
      Serial.println(flag);
      servo1.write(pos);
      delay(5);
    }
  }
  else if(val>5 && direction && !flag){ // closing
    Serial.println("IF 2");
    direction = !direction;
    Serial.println(direction);
    target = 90;
    while(target>pos){
      pos+=1;
      flag = haptics();
      Serial.println(flag);
      if(!flag){servo1.write(pos);}
      else{break;}
      delay(5);
    }
  }
  delay(5);

}

bool haptics(){
  //Serial.println("ENTERING HAPTICS"); 
  fsr[0] = analogRead(fsrpin1);
  //fsr[1] = analogRead(fsrpin2);
  //fsr[2] = analogRead(fsrpin3);
  //fsr[3] = analogRead(fsrpin4);
  //fsr[4] = analogRead(fsrpin5); 
  bool flag = 0;    
 
  int intensity[n];
  for (int i = 0; i<n; i++){
 
   /* Serial.print(fsr[0]);
    Serial.print(", ");
    Serial.print(fsr[1]);
    Serial.print(", ");
    Serial.print(fsr[2]);
    Serial.print(", ");
    Serial.print(fsr[3]);
    Serial.print(", ");
    Serial.print(fsr[4]);
    Serial.println();*/
 
    intensity[i] = (int)((fsr[i] / 300.0)*255.0); //multiply actual force by a factor of the possible max force
 
    if (lastreading[i] == 0 && fsr[i] >= 20){
      intensity[i] = 200; //provide a higher amplitude pulse at the beginning of touch
    }
    /*
      Serial.print(intensity[0]);
      Serial.print(", ");
      Serial.print(intensity_mod[0]);
      Serial.println();
      */
   intensity[i] = (intensity[i] - intensity_mod[i]); //applying intensity modification factor
    if (intensity[i] < 0){ //remove negative intensities
      intensity[i] = 0;
    }
    /*if (intensity[i] < 70 && intensity[i] >20){
      intensity[i] = 80; //sets minimum for PWM to 80 because intensity below ~70 doesn't actually produce vibration.
    }*/
  }
 
  for(int i = 0; i<n; i++){ //deducing the intensity modification factor
    if (fsr[i] <10){
      lastreading[i] = 250; // checks if there's no force, which resets the modifications and flags that the last reading was 0
      timer[i] = 0;
      intensity_mod[i] = 0;
    } else {
      lastreading[i] = 1; //sustained pressure will result in the intensity tapering off.
      timer[i]+=50;
      /*Serial.print(timer[i]);
      Serial.print(", ");
      Serial.println(fsr[0]);*/
      flag = 1;
      Serial.println("FSR number: " + String(i));
      if (timer[i] > 500 and fsr[0]>400 or fsr[1]>400 or fsr[3]>400){
      intensity_mod[i]+=10; //increase the intensity modification factor over time
     
      }
      
    }
  //intensity[i] = abs(intensity[i] - intensity_mod[i]);
  analogWrite(5, intensity[0]); // write the intensity to vibration motors
  //analogWrite(6, intensity[1]);
  //analogWrite(3, intensity[2]);
  //analogWrite(10, intensity[3]);
  //analogWrite(11, intensity[4]);
  
  }
 
  delay(20);
  return flag;
}
