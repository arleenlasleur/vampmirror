const int ShiftPWM_latchPin=8;
const bool ShiftPWM_invertOutputs=false;
const bool ShiftPWM_balanceLoad=false;

#include <ShiftPWM.h>                 // include AFTER setting the pins
#include <avr/pgmspace.h>

#define TESTBTN  (!digitalRead(4))
#define STARTBTN (!digitalRead(3))

int           vbat=0,
              i_tmp=0,
              animframe=0;
byte          nbat=0,
              b_tmp=0,
              animstep=15;
bool          ena_anim=0;
unsigned long last_test=millis(),
              last_anim=millis();

const byte loga[64]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,18,22,25,28,29,30,33,36,39,42,46,49,53,56,60,64,68,72,
77,81,86,90,95,100,105,110,116,121,127,132,138,144,150,156,163,169,176,182,189,196,203,210,218,225,233,240,248,255};

const byte disch[64] PROGMEM={248,240,236,234,232,231,230,228,227,226,224,223,222,220,219,218,216,215,214,214,214,
212,211,210,210,209,207,207,206,205,203,202,202,201,199,198,197,196,193,192,190,189,188,186,185,184,182,181,180,177,
176,173,172,171,169,168,164,161,153,147,136,122,95,60};

void setup(){
  ShiftPWM.SetAmountOfRegisters(8);   // registers qty
  ShiftPWM.SetPinGrouping(1);         // no RGB grouping
  ShiftPWM.Start(75,255);             // synth frequency, top threshold
  pinMode(3,INPUT_PULLUP);            // activate btn
  pinMode(4,INPUT_PULLUP);            // test btn
  pinMode(A5,INPUT);                  // vbat
}

void loop(){
byte i,k;
char j;
  if(TESTBTN && millis()-last_test>=5000){               // bat test
    last_test=millis();
    ShiftPWM.SetAll(0);
    delay(200);
    vbat=analogRead(A5);
    nbat=65;
    for(i=0;i<64;i++){
      b_tmp=pgm_read_byte(&disch[i]);
      i_tmp=(int)b_tmp;
      i_tmp+=612;
      if(i_tmp>=vbat) nbat=i;
    }
    if(nbat==65){
      for(i=0;i<8;i++){
        ShiftPWM.SetAll(255);
        delay(80);
        ShiftPWM.SetAll(0);
        delay(80);
      }
    }else{
      for(i=0;i<=63-nbat;i++){
        ShiftPWM.SetOne(i,255);
        delay(32);
      }
    }
    delay(2000);
    ShiftPWM.SetAll(0);
  }
  if(!ena_anim && STARTBTN){                             // animation
    ena_anim=1;
    animframe=0;
  }
  if(ena_anim && millis()-last_anim>=animstep){

    if(animframe>=0 && animframe<128){             // 4 sect fadein
      j=animframe;
      if(animframe>=96) animstep=8;
       else animstep=15;
      for(i=0;i<16;i++){
        ShiftPWM.SetOne(i,   loga[constrain(j,0,63)]);
        ShiftPWM.SetOne(i+16,loga[constrain(j,0,63)]);
        ShiftPWM.SetOne(i+32,loga[constrain(j,0,63)]);
        ShiftPWM.SetOne(i+48,loga[constrain(j,0,63)]);
        j-=4;
      }
    }else if(animframe>=128 && animframe<160){    // fadeout
      j=animframe-127; //1-32
      ShiftPWM.SetAll(loga[64-(j*2)]);
    }else if(animframe>=160 && animframe<192){    // fadein
      j=animframe-160; //0-31
      ShiftPWM.SetAll(loga[j*2]);
    }else if(animframe>=192 && animframe<224){    // fadeout
      j=animframe-191;
      ShiftPWM.SetAll(loga[64-(j*2)]);
    }else if(animframe>=224 && animframe<256){    // fadein
      j=animframe-224;
      ShiftPWM.SetAll(loga[j*2]);
    }else if(animframe>=256 && animframe<288){    // fadeout
      j=animframe-255;
      ShiftPWM.SetAll(loga[64-(j*2)]);
    }else if(animframe>=288 && animframe<320){    // fadein
      j=animframe-288;
      ShiftPWM.SetAll(loga[j*2]);
    }else if(animframe>=320 && animframe<384){    // fadeout slow
      animstep=25;
      j=animframe-320;
      ShiftPWM.SetAll(loga[63-j]);
    }else if(animframe>=384) ena_anim=0;          // disable

    animframe++;
    last_anim=millis();
  }
}
