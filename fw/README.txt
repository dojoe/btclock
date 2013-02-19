 USING THE BLUETOOTH CLOCK
===========================

1. Connecting for the first time

   1. Press the button on the controller board for a second or more, then release it.
   2. The clock will choose a random Bluetooth PIN and display it for 30 seconds.
   3. Search for Bluetooth devices on your computer, pair with "BT Clock", use the PIN you saw.
   
2. Connecting to the clock for control

After pairing, the clock will offer a Bluetooth Serial Port. On my system, I actually saw two; if that happens, pick the first.
Connect to the serial port using 9600 8N1 (9600 Baud, 8 data bits, no parity and one stop bit).

3. What the clock can do

  - Display the current time in 24-hour format, with blinking second dot
  - Display the current date in DD.MM. format
  - Display up to five arbitrary lines of text up to 45 characters each
     - If a line is longer than four characters, it will be scrolled across the display
     - Two scroll modes exist: Data Display mode and Marquee mode
     - Data Display mode will hold the beginning and end of the line for a short while; ideal for displaying data
     - Marquee mode will smoothly scroll the whole line through the display; ideal for messages or slogans
  - Configurable timed sequence of the above options
  - Configurable blank time during which the clock display will be turned off
  - Time is backed by a large capacitor and will continue running for about half a day if the power is interrupted
  - Configuration is stored in non-volatile memory and will survive power outages for as long as it takes 

4. How to set up the clock

The clock understands a few simple commands (as in "simple to parse by a microcontroller" ;).
Each command must be completed by a newline (LF, to be exact, CRs will be tolerated, but ignored).
The clock will respond "OK" if the command was correct, or "Nope" if not.

T=YYMMDDhhmmss
    Set the time, 24-hour format. Invalid values will be rejected.
    
T?
	Query the current time. The clock will respond the current time before "OK".

<N>=[!]<text>
	Set a text line, where <N> is a number between 1 and 5 and <text> is arbitrary text up to 45 characters.
	Valid characters are 1-9, a-z, A-Z, but case does not matter here. It's hard enough to invent a font for 7-segment displays as it is ;)
	Also, there are some special characters:
	   '=' will yield a dash
	   '<' will yield a capital C (as opposed to the lower-case c you get with 'c' and 'C')
	   '@' will yield a '°' (degree) symbol -- so "@<" will yield "°C" in case you want to display a temperature ;)
	All other characters will be mapped to space.
	If you put a '!' before the line, it will be scrolled in Marquee mode, otherwise in Data Display mode.
	
<N>?
    Query the <N>-th line. Will respond with the N-th line and "OK". 

S=Xttt[,Xttt[,...]]
    Configure the display sequence. 
    The sequence defines what will be displayed on the clock, in which order, and for how long.
    The sequence can consist of up to eight elements, separated by commas. It repeats after the last element.
    Each element starts with one character defining what should be displayed (the X), followed (without space) by a (decimal) time in seconds.
    For X, you have a choice of:
      "T" - display the current time
      "D" - display the current date
      "1".."5" - display text line 1..5
    So, for example, "S=T30,D10,120" will display the time for 30s, then the date for 10s, then line 1 for 20s, then repeat from the start.
    Please note that there is no space in the command, especially not after the commas.  
    
S?
    Query the sequence. Will respond with the sequence and "OK".

B=HHMM-HHMM
	Set the blank time. Starting with the first time, and up to the second time, the clock will blank its display.
	For example, "B=1800-0800" will keep the display off outside normal work hours.
	Likewise, "B=0800-1600" seems like a sensible setting for a hackerspace ;)

B?
	Query the blank time setting. Will respond with the blank time and "OK".
  