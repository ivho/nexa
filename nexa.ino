#define T 260

int rf_pin = 7;
int led_pin = 13;
void setup()
{
  Serial.begin(9600);
  Serial.print("Starting\r\n");
  pinMode(rf_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
}

void Pulse(int high, int low)
{
        digitalWrite(rf_pin, HIGH);
        digitalWrite(led_pin, HIGH);
        delayMicroseconds(high * T);
        digitalWrite(rf_pin, LOW);
        digitalWrite(led_pin, LOW);
        delayMicroseconds(low * T);
}

void send_bit(int bit)
{
        if (bit) {
                Pulse(1,1);
                Pulse(1,5);
        } else {
                Pulse(1,5);
                Pulse(1,1);
        }
}

void send_int(unsigned long val, int bits)
{
        int i;
        for (i = bits - 1; i >= 0; i--) {
                send_bit(val & (1<<i));
        }
}

void nexa(int group, int onoff, int unit, int button)
{
        int i;
        for (i = 0; i < 5; i++) {
                Pulse(1,10); // SYNC
                send_int(0x353870d, 26);
                send_int(group, 1);
                send_int(onoff, 1);
                send_int(unit, 2);
                send_int(button, 2);
                Pulse(1, 40); // PAUSE
        }
}

void loop()
{
        unsigned char x;
        Serial.print("enter:");
        while (!Serial.available()) {
        }
        x = Serial.read();
        if (x == '1') {
                Serial.print("on\r\n");
                nexa(1,0,3,3);
        }else if (x == '0') {
                Serial.print("off\r\n");
                nexa(1,1,3,3);
        } else if (x == 'g') {
                Serial.print("group\r\n");
                nexa(0,1,3,3);
	} else {
                Serial.print("unknown\r\n");
	}
}
