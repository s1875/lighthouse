
//10,11 i 12,13

#define BUTTON1 11
#define BUTTON2 12
#define BUTTON1BARE PINB3
#define BUTTON2BARE PINB4

#define P0 PD4
#define P1 PD5
#define P2 PD6
#define P3 PD7
#define P4 PB0
#define P5 PB1

#define PORT0 PORTD
#define PORT1 PORTD
#define PORT2 PORTD
#define PORT3 PORTD
#define PORT4 PORTB
#define PORT5 PORTB


#define PIN_HIGH(n) PORT##n |= (1 << P##n)
#define PIN_LOW(n) PORT##n &= ~(1 << P##n)


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //buttons
  pinMode(10,OUTPUT);
  pinMode(BUTTON1,INPUT);
  digitalWrite(10,LOW);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  pinMode(BUTTON1,INPUT);
  pinMode(BUTTON2,INPUT);
  digitalWrite(BUTTON1,HIGH);
  digitalWrite(BUTTON2,HIGH);
  
  //
   
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  digitalWrite(4,HIGH);  
  digitalWrite(5,HIGH); 
  digitalWrite(6,HIGH); 
  digitalWrite(7,HIGH); 
  digitalWrite(8,HIGH); 
  digitalWrite(9,HIGH); 
  
  }


unsigned long lastMillis;
boolean lastButton1State = false;
boolean lastButton2State = false;

int color = 0;
int blinkMode = 0;

#define COLOR_SIZE 8
int colors[COLOR_SIZE][6] ={
                            {1,1,1,1,1,1},
                            {1,1,0,1,1,0},
                            {1,0,1,1,0,1},
                            {0,1,1,0,1,1},
                            {1,0,0,1,0,0},
                            {0,1,0,0,1,0},
                            {0,0,1,0,0,1},
                            {0,0,0,0,0,0}
                           };
#define BLINK_MODE_SIZE 3
int blinks[BLINK_MODE_SIZE][2][10] ={{{0,50,100,200,255,255,200,100,50,0},
                                     {0,50,100,200,255,255,200,100,50,0}},
                                     {{255,200,100,50,0,0,50,100,200,255},
                                     {0,50,100,200,255,255,200,100,50,0}},
                                     {{255,255,255,255,255,255,255,255,255,255},
                                     {255,255,255,255,255,255,255,255,255,255}}};
void button1Pressed(){
  color = (color + 1) % COLOR_SIZE;
  Serial.print("Colors(");Serial.print(color);Serial.print("):");
  for (int i = 0; i < 6; i++){
    Serial.print(colors[color][i]); Serial.print(" - ");}
  Serial.println("");
}

void button2Pressed(){
//  Serial.println("Button2 pressed");
  blinkMode = (blinkMode + 1) % BLINK_MODE_SIZE;
  Serial.print("Blinks(");Serial.print(blinkMode);Serial.print("):");
  for (int i = 0; i < 10; i++){
    Serial.print(blinks[blinkMode][0][i]); Serial.print(" - ");}
  Serial.println("");
  Serial.print("blink mode 2:");
  for (int i = 0; i < 10; i++){
    Serial.print(blinks[blinkMode][1][i]); Serial.print(" - ");}
  Serial.println("");
}

unsigned long b1Millis,b2Millis;
  boolean button1Lock = false, button2Lock = false;

void checkButtons(){
//  Serial.println(PINB & (1 << PINB3));
  boolean newButton1State = (PINB & (1 << PINB3)) > 0 ? false : true;
  boolean newButton2State = (PINB & (1 << PINB4)) > 0 ? false : true;

  
  if (newButton1State != lastButton1State){
    b1Millis = millis();
    Serial.println("Setting millis");
    lastButton1State = newButton1State;
  }

  if (millis() - b1Millis > 100 && newButton1State == lastButton1State){
      
       if (newButton1State && !button1Lock){
          button1Pressed();
          button1Lock = true;
          Serial.println("locking");
       }else if (!newButton1State && button1Lock){
        Serial.println("unlocking");
        button1Lock = false;
       }
    }

    if (newButton2State != lastButton2State){
      b2Millis = millis();
      Serial.println("Setting millis");
      lastButton2State = newButton2State;
  }

  if (millis() - b2Millis > 100 && newButton2State == lastButton2State){
      
       if (newButton2State && !button2Lock){
          button2Pressed();
          button2Lock = true;
          Serial.println("locking");
       }else if (!newButton2State && button2Lock){
        Serial.println("unlocking");
        button2Lock = false;
       }
    }
    
}


int pointer = 0;
 int pwm = 0;

int targets[6] = {0,0,0,0,0,0};

void setTargets(){
  if (pwm == 255){
    int pos1 = floor(pointer / 100);
    int pos2 = (pos1 + 1) % 10;
//    Serial.print("pos1=");Serial.println(pos1);
//    Serial.print("Target:");
    int pos1Value = blinks[blinkMode][0][pos1];
    int pos2Value =  blinks[blinkMode][0][pos2];
    for(int col = 0; col < 3; col++){
      targets[col] = colors[color][col] * ((pos1Value * (100 - (pointer - pos1 * 100)))/100 + (pos2Value * (pointer - pos1 * 100))/100);
//      Serial.print(targets[col]);
//      Serial.print(", ");
    }
    pos1Value = blinks[blinkMode][1][pos1];
    pos2Value =  blinks[blinkMode][1][pos2];
    for(int col = 3; col < 6; col++){
      targets[col] = colors[color][col] * ((pos1Value * (100 - (pointer - pos1 * 100)))/100 + (pos2Value * (pointer - pos1 * 100))/100);
//      Serial.print(targets[col]);
//      Serial.print(", ");
    }
//    Serial.println("");
    pointer = (pointer + 1) % 1000;
    pwm = 0;
  }else
    pwm++;
 // pwm = (pwm + 1) % 255;
  

 }

 void draw(){
  setTargets();
  if (targets[0] > pwm) PIN_LOW(0); else PIN_HIGH(0);
  if (targets[1] > pwm) PIN_LOW(1); else PIN_HIGH(1);
  if (targets[2] > pwm) PIN_LOW(2); else PIN_HIGH(2);
  if (targets[3] > pwm) PIN_LOW(3); else PIN_HIGH(3);
  if (targets[4] > pwm) PIN_LOW(4); else PIN_HIGH(4);
  if (targets[5] > pwm) PIN_LOW(5); else PIN_HIGH(5);
  
 }

void loop() {
 // Serial.println("loop");
  checkButtons();
  draw();
  //delay(10);
}
