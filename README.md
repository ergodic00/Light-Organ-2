Light-Organ-2
=============

This is Arduino code to easily create complex color display effects on the Adafruit LPD8806 LED strips, using the Ardunio Due or Mega boards. Using the Bliptronics spectrum analyzer shield with the Arduino you  can drive the LED strip based on sound levels in various spectra.

The code is written as a C++ class (LEDSegs). It is designed to make it very easy to implement even quite complex display effects by just calling the various class methods.

This new version includes random segments, better gain control, timer-based actions, strip "parts", and quite a few other enhancements and fixes. For the most part it is call-compatible with the original library.

LEDSegs.cpp and LEDSegs.h comprise the core library code that handles segment definition and display logic. I've also posted the LPD8806 library modified for SPI as described in the comments.

The example program LOXMAS_V35.ino contains code that cycles 13 different types of Christmas displays using segment definitions and is a good place to look after you've reviewed the extensive preamble comments in LEDSegs.cpp.
