/*

This version is working on the 2.8.0 version of IRremote

The program reads the IR signals and stores them in local memory to send

Utilizes a functions from IRremote to send and process ir signals

*/

#include <IRremote.h>

#define RAWBUF 100            // Maximum length of raw code buffer
#define USECPERTICK 50        // Duration of a tick in microseconds
#define MARK_EXCESS 10        // Amount to adjust for receiver distortion

int RECV_PIN = 11;
int SEND_BUTTON = 5;
int READ_BUTTON = 6;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

unsigned long codeValue;           // The decoded code value
unsigned int rawCodes[RAWBUF];     // Stores raw durations of the signal
int codeLen;                       // Length of the code

bool detectSignal = false;         // Flag to control signal detection
int ReadButtonState = 0;
int SendButtonState = 0;

void setup() {
  Serial.begin(9600);               // Start Serial for debugging
  irrecv.enableIRIn();              // Start the IR receiver
  pinMode(SEND_BUTTON, INPUT);
  pinMode(READ_BUTTON, INPUT);
  Serial.print("Setup complete\n");
}

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

void sendCode() {
  irsend.sendRaw(rawCodes, codeLen, 38);
  Serial.println("Sent stored IR code as raw signal.");
}

void myPrintResult() {                                        // Print out stored data
  irrecv.printResultShort(&Serial);
  Serial.println();
}

void loop() {
  // Reading button state
  ReadButtonState = digitalRead(READ_BUTTON);
  SendButtonState = digitalRead(SEND_BUTTON);

  // Check for user input
  char input = Serial.read();

  // Sending output
  if (input == 's' || SendButtonState == HIGH) {              // User types 's' to send the code
    Serial.println("Sending stored IR code...");
    sendCode();
  }

  // Reading input
  else if (input == 'r' || ReadButtonState == HIGH) {            // Set flag to enable detection
    Serial.println("Looking for IR signal...");
    detectSignal = true;                              
  }

  // If in detection mode, look for IR signal
  if (detectSignal && irrecv.decode(&results)) {
    storeCode();            // Store the detected code
    detectSignal = false;   // Stop detection after receiving one code
    irrecv.resume();        // Prepare for the next signal
  }
}
