#include <Arduino.h>

#define REQ_PIN 5 // physical pin 11 on AtMega8
#define CLK_PIN 6 // physical pin 12 on AtMega8
#define DAT_PIN 7 // physical pin 13 on AtMega8

#define FREQ 10
#define NIBBLES_PER_MESSAGE 13
#define NUM_BITS (NIBBLES_PER_MESSAGE * 4)
#define DIGIT_COUNT 6
#define DIGIT_OFFSET 5

#define POS_SIGN 4
#define POS_UNIT 12
#define POS_DEC_POINT 11

void setup() {
  pinMode(REQ_PIN, OUTPUT);
  digitalWrite(REQ_PIN, HIGH);

  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DAT_PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void wait_for_falling_clk_edge() {
  while (digitalRead(CLK_PIN) == LOW) {
  }
  while (digitalRead(CLK_PIN) == HIGH) {
  }
}

void loop() {

  byte data[NIBBLES_PER_MESSAGE];
  static unsigned long last_read = micros();

  digitalWrite(REQ_PIN, LOW);

  for (int i = 0; i < NUM_BITS; i++) {
    wait_for_falling_clk_edge();

    if (i % 4 == 0) {
      data[i / 4] = 0;
    }

    data[i / 4] |= (digitalRead(DAT_PIN) << (i % 4));
    if (i == 0) {
      digitalWrite(REQ_PIN, HIGH);
    }
  }

  // print sign
  if (data[POS_SIGN] & 8) {
    Serial.print('-');
  }

  int pos_dec_point = data[POS_DEC_POINT];
  bool disp_digit = false;
  // print value
  for (int i = 0; i < DIGIT_COUNT; i++) {
    int digit = data[i + DIGIT_OFFSET];
    disp_digit |= digit || (pos_dec_point + i >= 5);
    if(disp_digit) {
      Serial.print(digit);
    }

    // print decimal point
    if (pos_dec_point && (pos_dec_point + i == 5)) {
      Serial.print('.');
    }
  }

  // print unit
  if (data[POS_UNIT] == 0) {
    Serial.print(" mm");
  } else {
    Serial.print(" inch");
  }


  // newline
  Serial.print('\n');

  while (micros() - last_read < 1000000 / FREQ);
  last_read = micros();

}
