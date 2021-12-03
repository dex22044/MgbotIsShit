#include <IRremote.h>
#include <IRremoteInt.h>

#define IR_RECEIVE_PIN 27

IRrecv ir(IR_RECEIVE_PIN);
decode_results irres;

void initIR() {
  ir.enableIRIn();
}

int receiveIR() {
  if(ir.decode(&irres)) {
    ir.resume();
    return irres.value;
  }
  else {
    return -1;
  }
}
