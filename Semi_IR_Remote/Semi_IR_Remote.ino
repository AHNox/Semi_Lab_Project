/*

This version is working on the 2.8.0 version of IRremote

The program reads the IR signals and stores them in local memory to send

Utilizes a functions from IRremote to send and process ir signals

*/

#include <IRremote.h>

// Maximum length of raw code buffer
#define RAWBUF 100

// Duration of a tick in microseconds
#define USECPERTICK 50

// Amount to adjust for receiver distortion
#define MARK_EXCESS 10

// Setting variables for pins and program globals
int RECV_PIN = 11;
int SEND_BUTTON = 5;
int READ_BUTTON = 6;
bool detectSignal = false;
int ReadButtonState = 0;
int SendButtonState = 0;

// Setting up IR Remote objects
IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;
unsigned long codeValue;
unsigned int rawCodes[RAWBUF];
int codeLen;

// Setup function 
void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  pinMode(SEND_BUTTON, INPUT);
  pinMode(READ_BUTTON, INPUT);
  Serial.print("Setup complete\n");
}

// Function to store code
void storeCode() {
  int count = results.rawlen;
  codeLen = count - 1;
  for (int i = 1; i <= codeLen; i++) {
    rawCodes[i - 1] = results.rawbuf[i] * USECPERTICK + (i % 2 == 0 ? MARK_EXCESS : -MARK_EXCESS);
  }
  codeValue = results.value;

  Serial.println("IR code received and stored.");
  myPrintResult();
}

// Function to send code
void sendCode() {
  irsend.sendRaw(rawCodes, codeLen, 38);
  Serial.println("Sent stored IR code as raw signal.");
}

// Print out stored data
void myPrintResult() {
  irrecv.printResultShort(&Serial);
  Serial.println();
}

void loop() {

  // Re-enable receiver
  irrecv.enableIRIn();
  
  // Reading button state
  ReadButtonState = digitalRead(READ_BUTTON);
  SendButtonState = digitalRead(SEND_BUTTON);

  // Check for user input
  char input = Serial.read();

  // Outputing stored code
  if (input == 's' || SendButtonState == HIGH) {
    Serial.println("Sending stored IR code...");
    sendCode();
  }

  // Changing flag to enable detection
  else if (input == 'r' || ReadButtonState == HIGH) {
    Serial.println("Looking for IR signal...");
    detectSignal = true;                              
  } 
  
  // Printing the contents that is stored for testing
  else if (input == 'p'){
    myPrintResult();
  }

  // Look for IR signal when detection flag is on
  if (detectSignal && irrecv.decode(&results)) {
    storeCode();
    detectSignal = false;
    irrecv.resume();
  }
}
