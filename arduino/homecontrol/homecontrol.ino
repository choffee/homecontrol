/*
 *
 * Control my home using magic
 *
 * Command Format
 * aa nnnn nnnn nnnn
 * aa - is two chars that identify the command as below
 * nnnn are up to three data sections depending on the command.
 *
 * Commands:
 * RF 00D1 on - Using RF turn on device D1
 *
 * A lot of the serial code comes from this file:
 * https://github.com/UrsusExplorans/SerialControl/blob/master/serialControl.pde
 *
 * It's licenced under the GPL V2
 *
 */
#include <RemoteTransmitter.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <RF24.h>

// Intantiate a new Elro remote, also use pin 11 (same transmitter!)
ElroTransmitter elroTransmitter(11);
// Intantiate a new ActionTransmitter remote, use pin 11
ActionTransmitter actionTransmitter(11);

LiquidCrystal lcd(8, 7, 5, 4, 3, 2);

#define MAXCOMMANDLENGTH 16

String command = String(MAXCOMMANDLENGTH);
boolean commandComplete = false;

// List of function defined
void getIncomingChars();
void processCommand();
void processDisplayCommand();
bool validateCommand();
bool isNumeric(char);
void processRFcommand();
// End of defines

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);
// Radio pipe address
const uint64_t nrfSensorPipe            = 0xF0F0F0F0E1LL;


const int powerSwitchPin = A3;
const int fourSwitchPin = A4;
const int sixSwitchPin = A5;

const int switchPins[3] = {powerSwitchPin, fourSwitchPin, sixSwitchPin};
const int numberOfSwitches = 3;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, Incontrol v1");
  command = "";

  lcd.begin(20, 4);
  lcd.print("HomeAuto starting..");

  for (int i = 0; i < numberOfSwitches; i++) {
      pinMode(switchPins[i], INPUT);
  }
  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //


  // Open the pipe for listenting for sensors
  radio.openReadingPipe(1,nrfSensorPipe);
  radio.startListening();
}


// Grab all the chars from the serial port.
void getIncomingChars() {
  char inChar = Serial.read();
  if(inChar == 59 || inChar == 10 || inChar == 13){
    commandComplete = true;
  } else {
    command.concat(inChar);
  }
}

void processCommand() {
  if(validateCommand()) {
    Serial.print("You said: ");
    Serial.println(command);
    if ( command.charAt(0) == 'R' ) {
      if ( command.charAt(1) == 'F' ) {
        processRFcommand();
      }
    }
    if ( command.charAt(0) == 'D' ) {
      if ( command.charAt(1) == 'P' ) {
        processDisplayCommand();
      }
    }
  } else {
    Serial.print("Bad Command: ");
    Serial.println(command);
  }

  // Reset things for the next command
  command = "";
  commandComplete = false;
}

void processRFcommand() {
  char set;
  int num;
  bool state;
  Serial.println("Doing RF command");
  // Should be RF nnD1 on
  // or RF nnC1 off
  set = command.charAt(4);
  num = 0 + command.charAt(5) - 48;
  // Rough check
  if ( command.charAt(7) == 'n') {
    state = true;
  } else {
    state = false;
  }
  elroTransmitter.sendSignal(num,set,state);
}

void processDisplayCommand() {
  int row;
  int col;
  String text = String(20);
  text = "";
  Serial.println("Doing Display Command");
  // 1 digit row
  // two digits column
  // Now read everything till the newline as text to display.
  row = 0 + command.charAt(3) - 48;
  col = 0 + command.charAt(5) - 48 + ( 10 * ( 0 + command.charAt(4) - 48));
  // Just fix bad numbers
  if (col > 20)  col = 19;
  if (row > 4)  row = 3;
  // Copy 7 to 27 to the text string
  for ( uint8_t l = 6 ; l < 26 ; l++ ){
    if ( l >= command.length()  ) {
      break;
    } else {
      text.concat(command.charAt(l));
    }
  }
  // Send the text to the display
  lcd.setCursor(col, row);
  //lcd.print(millis()/1000);
  lcd.print(text);


}

bool validateCommand() {
  // This will check that the command is good.
  // Just check that it starts with a two digit string
  if (command.length() < 6 ) { return false; };
  // It should start with two chars
  if( (isNumeric(command.charAt(0)) && isNumeric(command.charAt(1)))){
    return false;
  }
  // If you made it this far we are good.
  return true;
}

bool isNumeric(char character){
  if(character >= 48 && character <= 75){
    return true;
  }
  return false;
}

void readSwitches() {
    for (int i = 0 ; i < numberOfSwitches ; i++) {
        if (digitalRead(switchPins[i])) {
            Serial.write("Pin pressed");
        }
    }
}

void readNrf24() {
    char receivedPayload[31];
    uint8_t len = 0;
    uint8_t pipe = 0;

    if ( radio.available(&pipe) ) {
        bool done = false;
        while (!done) {
            // Fetch the payload, and see if this was the last one.
            len = radio.getDynamicPayloadSize();
            done = radio.read( &receivedPayload, len );
            Serial.write("RF P:");
            Serial.write(pipe);
            Serial.write(" P:");
            Serial.write(receivedPayload);
            Serial.write("\n");
        }
    }
}

void loop(){
  if (Serial.available() > 0) {
    getIncomingChars();
  }
  if (commandComplete == true ) {
    processCommand();
  }
  readSwitches();
  readNrf24();
}
