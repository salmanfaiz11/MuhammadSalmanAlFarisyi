// Muhammad Salman Al Farisyi
// 13218061
// 6 Sigit 7 Segment Digital Clock Implementation With Arduino UNO

// ezButton library for debouncing
#include <ezButton.h>

// button declaration - debounced
ezButton button1(14);
ezButton button2(15);
ezButton button3(16);

int a = 0; // display port counter
int mode = 0; // digital clock mode
long hour, minute, second; // variable to store clock detail
long realtime = 3700; // main clock time
long mytime = 0; // clock setting time
long alarm = 0; // alarm time
long stopwatch = 0; // stopwatch time
int realtime_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store main clock parameter
int alarm_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store alarm parameter
int stopwatch_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store stopwatch parameter
int mytime_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store clock setting parameter
int set_waktu_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store temporary clock setting parameter
int set_alarm_arr[6] = {0, 0, 0, 0, 0, 0}; // array to store temporary alarm parameter
int sw_flag = 0; // flag for stopwatch
long sw_flag_time = 0; // stopwatch flag time
int stop_counter; // count time when stopwtach is stopped
int button2_counter; // counter if button 2 pressed

 // array to store binary for clock display
int num[10] = {	0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000010, 0b11111000, 0b10000000, 0b10010000};
 
void setup() {

  DDRB = 0xFF; // set pin D as output
  DDRC = 0b00111000; // set mode pin C3, C4, C5 as output
  DDRD = 0xFF; // set pin B as output
  PORTB = 0x00; // initial state for pin D
  PORTC = 0x00; // initial state for pin D
  PORTD = 0x00; // initial state for pin D
  UCSR0B = 0x00; // initial state for rx and tx

  init_int();
}

void loop() {

  // activare debouncing function
  button1.loop();
  button2.loop();
  button3.loop();

  // change mode if button 1 is pressed
  if (button1.isPressed() == HIGH) {
    mode++;
    mode %= 4;
  }
  
  // alarm time = main clock time - alarm activated
  if ((realtime == alarm) && (alarm != 0)) {
      PORTC = 0b00111000; // led 1, 2, and 3 active at a same time for a short duration
      delay(10);
  }

  // mode 0
  if (mode == 0) {
    PORTC = 0x00; // no active led
    button2_counter = 0; // reset button counter
    show_digit(realtime_arr); // show digit for main clock
    
	// reset clock setting parameter
    for (int i = 0; i < 6; i++) {
      set_waktu_arr[i] = 0;
    }
    
	// if button 2 is pressed, apply parameter from clock setting to main clock
    if (button2.isPressed() == HIGH) {
      realtime = mytime;
    }
	// mode 1
  } else if (mode == 1) {
    PORTC = 0b00001000; // led 1 active
    set_waktu(); // calculate clock setting parameter
    show_digit(mytime_arr);  // show digit for clock setting
	// mode 2
  } else if (mode == 2) {
    PORTC = 0b00010000; // led 2 active
    set_alarm(); // calculate alarm parameter
    show_digit(alarm_arr);  // show digit for alarm
	// mode 3
  } else {
    PORTC = 0b00100000; // led 3 active
    set_stopwatch(); // calculate stopwatch parameter
    show_digit(stopwatch_arr);  // show digit for stopwatch
  }
}

void init_int() {
  
  cli(); // Disable all interrupt
  TCCR1A = 0; // Reset timer1 register A
  TCCR1B = 0b00001101; // CTC and set prescaler 1024
  TCNT1 = 0; // Preload timer1
  OCR1A = 15624; // Value to compare
  TIMSK1 = 2; // Output Compare A Match Interrupt Enable
  sei(); // Enable all interrupt
}

void show_digit(int digit[6]) {
  
  // change selector for 7 segment display
  for (a = 0; a < 6; a++) {
    PORTB = 1 << a;
    if (PORTB == 0b00000001) {
      PORTD = num[digit[0]];
    } else if (PORTB == 0b00000010) {
      PORTD = num[digit[1]];
    } else if (PORTB == 0b00000100) {
      PORTD = num[digit[2]];
    } else if (PORTB == 0b00001000) {
      PORTD = num[digit[3]];
    } else if (PORTB == 0b00010000) {
      PORTD = num[digit[4]];
    } else {
      PORTD = num[digit[5]];
    }
    delay(1);
  }
}

void set_waktu() {
	
  // variable to store temporary value
  long set_hour, set_minute, set_second;
  int i;

  // increment variable if button 2 is pressed
  if (button2.isPressed() == HIGH) {
    button2_counter++;
    button2_counter %= 3;
  }

  if (button2_counter == 2) {
    i = 24;
  } else {
    i = 60;
  }
  
  // calculate clock setting parameter if button 3 is pressed
  if (button3.isPressed() == HIGH) {
    set_waktu_arr[button2_counter] = (set_waktu_arr[button2_counter] + 1)%i;
    set_hour = (long)set_waktu_arr[2] * (long)3600;
    set_minute = (long)set_waktu_arr[1] * (long)60;
    set_second = (long)set_waktu_arr[0];

    mytime = (long)set_hour + (long)set_minute + (long)set_second;

    hour = ((long)mytime/(long)3600)%(long)24;
    minute = ((long)mytime/(long)60)%(long)60;
    second = (long)mytime%(long)60;

    mytime_arr[0] = (second%10);
    mytime_arr[1] = (second/10);
    mytime_arr[2] = (minute%10);
    mytime_arr[3] = (minute/10);
    mytime_arr[4] = (hour%10);
    mytime_arr[5] = (hour/10);
  }
}

void set_alarm() {
  
  // variable to store temporary value
  long set_hour, set_minute, set_second;
  int i;

  // increment variable if button 2 is pressed
  if (button2.isPressed() == HIGH) {
    button2_counter++;
    button2_counter %= 3;
  }
  
  if (button2_counter == 2) {
    i = 24;
  } else {
    i = 60;
  }
  
  // calculate alarm parameter if button 3 is pressed
  if (button3.isPressed() == HIGH) {
    set_alarm_arr[button2_counter] = (set_alarm_arr[button2_counter] + 1)%i;
    set_hour = (long)set_alarm_arr[2] * (long)3600;
    set_minute = (long)set_alarm_arr[1] * (long)60;
    set_second = (long)set_alarm_arr[0];

    alarm = set_hour + set_minute + set_second;

    hour = (long)(alarm/3600)%24;
    minute = (long)(alarm/60)%60;
    second = (long)alarm%60;

    alarm_arr[0] = (second%10);
    alarm_arr[1] = (second/10);
    alarm_arr[2] = (minute%10);
    alarm_arr[3] = (minute/10);
    alarm_arr[4] = (hour%10);
    alarm_arr[5] = (hour/10);
  }
}

void set_stopwatch() {

  // set flag if button 2 is pressed
  if ((button2.isPressed() == HIGH) && (sw_flag == 0)) {
    stop_counter = 0;
    sw_flag = 1; // start mode
    sw_flag_time = realtime; 
  } else if ((button2.isPressed() == HIGH) && ((sw_flag == 1) || (sw_flag == 3))) {
    sw_flag = 2; // stop mode
  } else if ((button2.isPressed() == HIGH) && (sw_flag == 2)) {
    sw_flag = 3; // start again
  }

  // flag detection
  if (sw_flag == 1) {
    stopwatch = (long)realtime - (long)sw_flag_time;
  } else if (sw_flag == 3) {
    stopwatch = (long)realtime - (long)sw_flag_time - (long)stop_counter;
  }
  
  // reset mode
  if ((button3.isPressed() == HIGH) && (sw_flag == 2)) {
    stopwatch = 0;
    sw_flag = 0;
  }
    
  hour = (long)(stopwatch/3600)%24;
  minute = (long)(stopwatch/60)%60;
  second = (long)stopwatch%60;

  stopwatch_arr[0] = (second%10);
  stopwatch_arr[1] = (second/10);
  stopwatch_arr[2] = (minute%10);
  stopwatch_arr[3] = (minute/10);
  stopwatch_arr[4] = (hour%10);
  stopwatch_arr[5] = (hour/10);
}

ISR(TIMER1_COMPA_vect) {
	
  // reset realtime count
  if (realtime == 86400) {
    realtime = 0;
  }

  if (sw_flag == 2) {
    stop_counter++;
  }
  
  realtime++; // increment realtime count
  
  // calculate main clock parameter
  hour = (realtime/3600)%24;
  minute = (realtime/60)%60;
  second = realtime%60;
  
  realtime_arr[0] = second%10;
  realtime_arr[1] = second/10;
  realtime_arr[2] = minute%10;
  realtime_arr[3] = minute/10;
  realtime_arr[4] = hour%10;
  realtime_arr[5] = hour/10;
}
