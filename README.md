# wireless_switch
A wireless controlled outlet with Arduino boards. This project is in two parts: transmitter and receiver. You have to program each side one after the other.

# Before starting
**Please make sure you have changed the transmission key in the interrupteur433.ino file before uploading your code onto the boards!** The key max length is 64 bytes.

Install the Arduino IDE, and then download the SimpleHOTP library from the IDE library manager. If you don't know how to do that, follow these instructions: https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries

# Transmitter side
For transmitter programming, you don't have any changes to do (except the key, of course). Just plug in your board, and upload the code.

# Receiver side
Change the `#define TX_BOARD` line (near line 32) to `#define RX_BOARD`. Then compile, and upload to the another board.
