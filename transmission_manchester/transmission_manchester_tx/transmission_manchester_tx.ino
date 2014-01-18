/*
  Transmitter
  
  quick and dirty transmitter for manchester codeing
  
  By AerospaceResearch.net, Andreas Hornig
  This work is licensed under a
  Creative Commons Attribution 4.0 International License.
 */

// the pin that is attached to the data pin of the TX module
int data_pin = 3;

// the setup routine runs once when you press reset:
void setup(){ 
  // declare pin to be an output:
  pinMode(data_pin, OUTPUT);
  Serial.begin(115200);
} 

// the loop routine runs over and over again forever:
// Keep in mind that delays are not a good way to send, because it
// there can be delay hickups. or you cannot easly switch from millis to micros in delays.
void loop(){
  // replace your transmitting text here.
  // the ! will line break at the receiver side.
  
  transmitting('S');
  transmitting('p');
  transmitting('a');
  transmitting('c');
  transmitting('e');
  transmitting(' ');
  transmitting('r');
  transmitting('o');
  transmitting('c');
  transmitting('k');
  transmitting('s');
  transmitting('!');
  
  delay(3000);
}

void transmitting(byte input){
  char test = input;
  Serial.print(test);
  if(test=='!'){
    Serial.println();
  }
  
  unsigned long tf = 10;
  unsigned long tf1 = tf;
  unsigned long tf2 = tf*4;
  unsigned long tfb = tf;
  
  // wake-up, RX!
  // this 20 times sending is used for waking up the
  // receiver module, because it uses on-off-keying in this configuration
  for(int i=0; i<20; i++){
    digitalWrite(data_pin,HIGH);
    delay(tf1);
    digitalWrite(data_pin,LOW);
    delay(tf1);
  }
  
  // synch blocks.
  // HIGH LOW HIGH LOW sending.
  // In this code I use two sync beeps to be able to distinguish
  // this signal from others with similar length.
  // The last low edge will be used to set the reference edge where
  // the signal of the the manchaster code starts.
  digitalWrite(data_pin,HIGH);
  delay(tf2);
  digitalWrite(data_pin,LOW);
  delay(tf1);
  digitalWrite(data_pin,HIGH);
  delay(tf2);
  digitalWrite(data_pin,LOW);
    
  // input transfer bit by bit
  // Manchester encoding, IEEE 802.3 convention
  // more, here http://en.wikipedia.org/wiki/Manchester_code
  delay(tfb);
  for(int i=0; i<8; i++){
    if(bitRead(input, i)==1){
      
      // rising edge for 1
      digitalWrite(data_pin,LOW);
      delay(tfb);
      digitalWrite(data_pin,HIGH);
      delay(tfb);
      
    } else{
      
      // falling edge for 0
      digitalWrite(data_pin,HIGH);
      delay(tfb);
      digitalWrite(data_pin,LOW);
      delay(tfb);
    }
    
    
    // shutting of TX
    digitalWrite(data_pin,LOW);
  }
}

