/*
 * Copyright (C) 2013 Ivar Holmqvist <ivarholmqvist@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <avr/io.h>
#include <util/delay.h>

/* NEXA learning code implementation.
 *
 * protocol from:
 *
 * http://tech.jolowe.se/home-automation-rf-protocols/
 *
 */

#define T 250

#define INVERT 0
#if INVERT
#define ZERO_LOW 5
#define ONE_LOW  1
#else
#define ZERO_LOW 1
#define ONE_LOW  5
#endif

#define RF_PIN  4
#define LED_PIN 5

/* Some LED stuff for debug. */
static int led = 0;
void set_led(int on)
{
    if (on) {
        PORTD &= ~(1 << LED_PIN);
        led = 1;
    } else {
        PORTD |= (1 << LED_PIN);
        led = 0;
    }

}
void invert_led()
{
    if (led) {
        led = 0;
    } else {
        led = 1;
    }
    set_led(led);
}

void blink_led()
{
    set_led(1);
    _delay_ms(10);
    set_led(0);
}

void error()
{
    for (;;) {
        invert_led();
        _delay_ms(100);
        invert_led();
        _delay_ms(100);
    }

}
void send_pulse(int high, int low)
{
    int i;
    set_led(0);
    PORTD |= (1 << 4);
    for (i = 0; i < high; i++) {
        _delay_us(T);
    }
    set_led(1);
    PORTD &= ~(1 << RF_PIN);
    for (i = 0; i < low; i++) {
        _delay_us(T);
    }
}

void send_zero()
{
    send_pulse(1,ONE_LOW);
    send_pulse(1,ZERO_LOW);
}

void send_one()
{
    send_pulse(1,ZERO_LOW);
    send_pulse(1,ONE_LOW);
}

void send_sync()
{
    send_pulse(1,10);
}

void send_pause()
{
    PORTD &= ~(1 << RF_PIN);
    _delay_us(T*40);
}


void send_bit(int bit)
{
    if (bit) {
        send_one();
    }
    else {
        send_zero();
    }
}
const char *xmitter_id = "1010 0100 1000 1001 0111 1100 01"; /* 26bit */

int send_str(const char *str)
{
    int i;
    int j;

    i = 0;
    j = 0;
    for (i = 0; str[i] != 0; i++) {
        if (str[i] == ' ')
            continue;
        if (str[i] == '1') {
            send_one();
            j++;
            continue;
        }
        if (str[i] == '0') {
            send_zero();
            j++;
            continue;
        }
    }
    return j;
}

void send_int(int v, int bits)
{
    int i;
    for (i = 0; i < bits; i++) {
        send_str(v&(1<<i)?"1":"0");
    }
}

void send_message(const char *id, int on, int chan, int button)
{
    int i;

    for (i = 0; i < 4; i++) {

        /* SYNC */
        send_sync();

        /* TRANSMITTER ID */
        send_str(id);

        /* GROUP */
        send_str("1");

        /* On/Off (11 -> DIM) */
        send_str(on?"1":"0");


        /* Channel */
        send_int(chan, 2);

        /* Button */
        send_int(button, 2);

        /* plain "zero" pulse,
         * there is no discenrable difference
         * between zero and one pulse here since
         * there is a "pause" after it...*/
        send_pulse(1, ZERO_LOW);

        _delay_ms(10);
    }
}




int main(void)
{

    /* All output */
	DDRD = 0xff;

	while (1) {
        send_message(xmitter_id, 0, 1, 1);
        _delay_ms(1000);

        send_message(xmitter_id, 1, 1, 1);
        _delay_ms(1000);
	}

	return 0;
}
