/*
  Receiver
  
  quick and dirty receiver for manchester codeing
  
  By AerospaceResearch.net, Andreas Hornig
  This work is licensed under a
  Creative Commons Attribution 4.0 International License.
 */

// the pin that the ouput pin of the RF receiver module
// is attached to
int data_pin = 3;

// the setup routine runs once when you press reset:
void setup(){ 
  // declare pin to be an output:
  pinMode(data_pin, INPUT);
  Serial.begin(115200);
} 

// some variables
unsigned long duration;
unsigned long time_PulseStart;
unsigned long time_contactBounce;
unsigned long time_PulseEnd;
unsigned long time_dif = 150;

unsigned long timeSyncPulse1End;
unsigned long signalDuration;

int syncCounter = 0;
int syncFlag = 2;
byte input;

unsigned long time_bit = 10000;             // bit length in micros. according to the TX
unsigned long time_syncBit = time_bit*4;
unsigned long time_bitscan = time_bit*0.6;  // percentage of one bit length you want to collect high/low measurments
unsigned long time_bitstop;

int bitCollector1;
int bitCollector0;

// the loop routine runs over and over again forever:
void loop(){
  if(digitalRead(data_pin)==HIGH){
    // detecting the length of an impulse. the function includes a very simple low pass filter
    // with time_diff
    signalDuration = signalLength();
    
    // there are two 40ms sync beeps (HIGH) in front of the
    // manchester coded bit signals
    if(signalDuration >= (time_syncBit*0.9) && signalDuration <= (time_syncBit*1.1)){
      syncCounter++;
    }
    
    // check for neighboring sync pulses.
    // this assures that the two 40ms sync bits are detected at the
    // very beginning of the data which are only separated by a 10 ms delay
    if(syncCounter==2 && (time_PulseEnd-timeSyncPulse1End)> (time_syncBit+(2*time_bit))){
      syncCounter=1;
    }
    timeSyncPulse1End=time_PulseEnd;
    
    // there are 2 sync beeps, that need to be detected first,
    // so syncFlag filters here.
    if(syncCounter==syncFlag){
      syncCounter = 0;
      
      time_bitstop = timeSyncPulse1End+time_bit/2;
    
      for(int i=0; i<8; i++){
        bitCollector1=0;
        bitCollector0=0;
        
        // the time maker time_bitstop is the middle of the scanning zone.
        time_bitstop = time_bitstop + time_bit*2 - time_bitscan/2;
        while(micros()<=time_bitstop){
          // just wasting time here
        }
        
        // during the scan time, the pin state is collected for a certain duration.
        time_bitstop = time_bitstop + time_bitscan;
        while(micros()<=time_bitstop){
          if(digitalRead(data_pin)==HIGH){
            bitCollector1++;
          } else{
            bitCollector0++;
          }
        }
        time_bitstop = time_bitstop - time_bitscan/2;
        
        //when there are more deteced HIGHs, then bit=1, else bit=0
        if(bitCollector1>=bitCollector0){
          bitWrite(input, i, 1);
        } else{
          bitWrite(input, i, 0);
        }
      }
      
      // output the received data.
      // the line break with ! can be changed, of course
      Serial.print(char(input));
      if(input=='!'){
        Serial.println();
      }
    }
  }
}

// this detects the length of a pulse,
// which is needed for the two sync beeps.
// It also works with one beep, but two beeps distinguish it from other statistical beeps.
unsigned long signalLength(){
  
  time_PulseStart=micros();
  time_contactBounce=time_PulseStart+time_dif;
  
  // including a timer is for "entprellung" of small signal blips
  while(digitalRead(data_pin)==HIGH || time_contactBounce > micros()){
    
    if(digitalRead(data_pin)==HIGH){
      // when there is another high, reset the counter end!
      time_PulseEnd=micros();
      // end time of pulse is stored here, because the while loop keeps on running to detect
      // another beep, but in case there is no beep the time reference is better.
      time_contactBounce=time_PulseEnd+time_dif;
    }
  }
  return (time_PulseEnd-time_PulseStart);
}
