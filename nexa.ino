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
		send_int(0x353, 10);
		send_int(0x870d, 16);
		send_int(group, 1);
		send_int(onoff, 1);
		send_int(unit, 2);
		send_int(button, 2);
		Pulse(1, 40); // PAUSE
	}
}

	int button = 3;
	int unit = 3;

void loop()
{
	unsigned char x;

	Serial.print("enter: [bu01g?]");
	while (!Serial.available()) {
	}
	x = Serial.read();
	switch (x) {
	case 'b':
		Serial.print("\r\nbutton: ");
		while (!Serial.available()) {}
		button = Serial.read() - '0';
		Serial.print(button);
		Serial.print("\r\n");
		break;

	case 'u':
		Serial.print("\r\nunit: ");
		while (!Serial.available()) {}
		unit = Serial.read() - '0';
		Serial.print(unit);
		Serial.print("\r\n");
		break;

	case '1':
		Serial.print(button);
		Serial.print("b:");
		Serial.print(unit);
		Serial.print("u:");
		Serial.print("on\r\n");
		nexa(1, 0, button, unit);
		break;

	case '0':
		Serial.print(button);
		Serial.print("b:");
		Serial.print(unit);
		Serial.print("u:");
		Serial.print("off\r\n");
		nexa(1, 1, button, unit);
		break;

	case 'g':
		Serial.print("group off\r\n");
		nexa(0, 1, button, unit); /* Button shouldn't matter for group... unit? */
		break;

	default:
		Serial.print("\r\n");
		Serial.print("b - set button (current ");
		Serial.print(button);
		Serial.print(")\r\n");
		Serial.print("u - set unit (current ");
		Serial.print(unit);
		Serial.print(")\r\n");
		Serial.print("1 - button/unit on\r\n");
		Serial.print("0 - button/unit off\r\n");
		Serial.print("g - group off\r\n");
		break;
	}
}
