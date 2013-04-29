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

// Intantiate a new Elro remote, also use pin 11 (same transmitter!)
ElroTransmitter elroTransmitter(11);
// Intantiate a new ActionTransmitter remote, use pin 11
ActionTransmitter actionTransmitter(11);

#define MAXCOMMANDLENGTH 16

String command = String(MAXCOMMANDLENGTH);
boolean commandComplete = false;

// List of function defined
void getIncomingChars();
void processCommand();
bool validateCommand();
bool isNumeric(char);
void processRFcommand();
// End of defines

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, Incontrol v1");
  command = "";
}

void loop(){
  if (Serial.available() > 0) {
    getIncomingChars();
  }
  if (commandComplete == true ) {
    processCommand();
  }
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
