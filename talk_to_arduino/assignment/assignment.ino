const unsigned int MAX_MESSAGE_LENGTH = 256;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  unsigned long int result = 1;
  while (Serial.available() > 0) {
    
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    char inByte = Serial.read();

    if (inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1)) {
      message[message_pos] = inByte;
      message_pos++;
    } else {
      message[message_pos] = '\0';

      if (message[0] == 'a') {
        // turn on led
        digitalWrite(13, HIGH);
        Serial.println("Done");
      } else if (message[0] == 'b') {
        // turn off led
        digitalWrite(13, LOW);
        Serial.println("Done");
      } else if (message[0] == 'c') {
        // flash led 5 times
        for (int j = 0; j < 5; j++) {
          digitalWrite(13, HIGH);
          delay(200);
          digitalWrite(13, LOW);
          delay(200);
        }
        Serial.println("Done");
      } else {
        // calculate factorial
        int number = atoi(message);
        for (int i = 1; i <= number; ++i) {
          result *= i;
        }
        Serial.println(result);
      }
      message_pos = 0;
    }
  }
}
