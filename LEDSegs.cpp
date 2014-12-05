
//Define this library if not already defined
#ifndef _LEDSEGS_cpp
#define _LEDSEGS_cpp

//Version ID
#define _LEDSEGS_ 35

//Some diagnostic definitions

//Define to output band level info to serial
//#define DIAGSEGS     //Debug output for segment processing
//#define DIAGRANDOM //Diagnose random segments
//#define DIAGDEBUG  //General debugging (currently nothing)

//Need to know if we need to init serial port
#if (defined DIAGSEGS) || (defined DIAGDEBUG) || (defined DIAGRANDOM)
#define DIAGINITSERIAL
#endif

/*

================
Sound-drive LED display for the Sparkfun 32-LED/meter RGB LED strip with an Arduino Due/Mega
optionally with the Bliptronics "spectrum analyzer" shield.

Steven Duff (sduff1@cox.net) April 2014

Copyright (c) 2013-2014, Steven Duff (aka "Ergodic" and "Ergodic00"), All Rights Reserved,

"MIT" open-source license as per http://opensource.org/licenses/MIT  granted to this code by
the author, and said license is included herein by reference.

  ________
  Revision History [SGD]

  LO19: Add segment spacing
  LO21: Add cSegOptModulateSegment option
  LO22: Add low/high levels to segments (work in progress)
  LO23: Bug fixes
  LO24: Display Routines, code cleanup, other misc
  LO25: Fixes & support for Mega - mostly a few obscure short/long issues
  LO26: Added cSegActionRandom, cSegOptInvertLevel, a few minor fixes
    No more "IncrementSegmentIndex" stuff.
  LO27: Add LEDSegsScaleBandMax and various scaling fixes
  LO28: Add LED strip "parts" functionality (see doc below)
  LO29: Timers, baby! (see doc below)
  LO30: New cSegActionBits action: display bits pointed to
    Code rewrites and fixes all over the damn place
    NOTE: The new timers functionality in LEDSegs:: for Setup() and Loop()
    should be used now, rather than delay(). Both will currently work,
    but new enhancements may require timers. Timers are
    documented below.
  LO31: Completely rework max level/ACG stuff.
    New cSegOptBandMax segment option
    Reduce default cMaxSegments to 100 for smaller Arduinos
  LO32: LEDBits class (bit field operations). Currently undocumented.
    Reworked current segment index stuff so you can define a specified segment #
  LO33: Enhance segment randomization capability with SetSegment_RandomPattern().
    Move project to Visual Micro
	cSegOptBandMax option made the default, cSegOptBandAvg is the new option for averaging
	 LO34: rescaling
    remove cSegInvertLevel & cSegOptRescale options
    Add persistence
    Misc. changes/fixes

=================
OK, Here we go...
=================
To use this code as written, you'll need some specific components:

  1) An Arduino Due - about $49 from your favorite Arduino dealer.
     ...Or an Arduino Mega 2560 - about $59. (More than the 32-bit Due, go figure.)
     ...Or this code MAY work with other AVR boards, see below

  2) ...Optionally stacked with a Bliptronics spectrum analyzer shield
     About $25 from sparkfun.com
     This shield is necessary if you want the strip responding to audio
     If you just want to create rotating/chasing segments, it isn't.

  3) ...Any length of Adafruit's LPD8806 weatherproof 32-LED/meter RGB strip.
     You'll also need an appropriate 5V supply to power it.

Using this library with the above hardware, you can easily program very sophisticated
motion and sound-drive "color organ" (aka "son et lumiere") effects to your LED strip.

____________________
The Spectrum Shield:

The spectrum analyzer shield is needed when you want audio levels to drive the display.
The shield produces seven, fixed-band audio amplitudes (from 0..1023) for both a right and left
channel each time the analyzer's values are scanned out.

The code here does some automatic gain adjustment to compensate for lower-level inputs. But
the noise level of the shield's bands runs about 10%, so for the best S/N results you
want to drive it close to the max. I found that a typical iPod/player output, while it works,
isn't quite enough to drive the shield to it's full dynamic range. I bought a "Boosteroo" on line
somewhere which gives a fixed 4x gain and I run the output of that into the shield. With an iPod
set at about 2/3 volume it's about right. 

_____________
The Segments:

The code here drives the LPD8806 LED strip in user-defined "segments" of RGB LEDs on the strip,
by default using the spectrum samples from the shield on each display refresh. You define the
segments, what spectrum bands (if any) drive them, and how the LEDs behave.

A "segment" is a contiguous section of LEDs on the strip. Segments have a defined starting
LED index, length, foreground/background color and other optional properties such as spacing,
background color, type of 'action,' etc. Any combination of the seven fixed spectrum analyzer
"bands" (including none) can be averaged together to drive the illumination of segments you define.

By default when you define a segment, the averaged level of the spectrum bands' audio samples
determines how many LEDs in that segment get illuminated on each display refresh cycle. The higher
the level, the more "on" LEDs in the segment are lit up.

But options can be defined for a segment to alter the display effect. You can also display
static-colored segments, backgrounds and bit mapped segments that are not related to spectrum level
input. And you can define custom display routines to more finely control segment display.

___________
The Boards:

The Due and Mega are the only boards I've tried. Other AVR-based boards may work, but you need
at least 3K SRAM and about 16-20K flash. You can reduce the cMax### constants to dial down the
needed SRAM. Please let me know of any results you have with other boards. Stack usage here is modest.
I have intentially avoided the much use of dynamic allocation, deep recursions, and malloc()s
where at all possible. So the compiled allocation plus the LEDSegs instance itself should be most of it.

One advantage to the Mega and other AVR-based boards is, if you use the Arduino IDE, the compile and
upload is much faster with the current V1.5.x beta IDE than for the Due. I use the Atmel/VIsual Micro IDE
which is much, much better in all regards, very easy to switch to, and so I recommend that (it's free.)

Note that although the Due is a 3.3V part, that level is fine to drive clock and data on the LPD8806
LED strip and the spectrum shield. A separate, stiff 5V supply is required to power the strip.

The Sparkfun LPD8806 library is used here and needs to be imported for your sketch. You must use the
most current version, and if you're using a Due, tweak it for SPI as described below.

===============
Hardware Setup:
===============

With this library, you can use either SPI output or any two digital output pins. SPI is STRONGLY
recommended uses much less CPU and so is faster. Using digital pins there's a 0.2ms/LED
time just for the digitalWrite() operations. For short strips this isn't a problem, but for
longer strips the lag with the audio becomes noticeable. Using SPI the I/O processing delay
is negligible and you can define a lot of complex segments on very long strips.

With SPI for the Mega, you pull SPI from digital pin 51 for data (MOSI) and digital pin 52 for clock (SCK).
For the Due's SPI output, you use the actual "SPI" header next to the SAM chip, not the ICSP header pins.
Attach your strip's clock lead to SPI "SCK" pin 3 (the second pin down, on the side closest to the CPU.)
Attach the data lead to MOSI pin 4 (the second pin on the other side of the header.

You'll also need to solder headers on to your spectrum shield so it can attach to your Arduino -- the
spectrum shield doesn't ship with them.

Attach a stiff 5V supply to the LED strip and join the grounds tightly between the supply and
the strip and the Arduino. Proper grounding is important as is the use of low-gauge wire for the strip power
in order to handle the net LED current at 5V. I'm able to successfully drive three 5-meter strips in
series with a single 5V 10A supply using a 10' run of 14GA landscape wire, though even with that there
is a little "brown-down" if everything is illuminated full white. Not really noticeable.

I recommend at least a full 5 meter LED strip if you can afford one. Be cautious connecting the strips
as they aren't polarity or voltage protected. A careless mistake can be quite costly. I also have found that the
strips sometimes ship with the connectors reversed, so check them before assuming anything. The strips
also are more physically delicate than they might appear, even with the rubberized cover. I sleeve mine
inside some 1"OD clear vinyl tubing. Anyway, handle gently or you can end up with dead LEDs.

You'll need the LPD8806 library. Make sure to get the latest one from github (linked via adafruit.)
If you use a Due you want to tweak it for its faster clock. Find the conditional section:

  "#ifndef SPI_CLOCK_DIV8 ..."

(If you don't see that section anywhere you have an old version of the LPD8806 library.) Before that
#ifndef, place the following conditional code to set the Due's SPI clock divider to something within
the strip's range (here 21 = 4MHz). 

  #if defined __SAM3X8E__
    #define SPI_CLOCK_DIV8 21
  #endif

The library is also missing conditional 'bracketing'. To avoid "duplicate definition" errors in your build,
bracket the top and bottom of LPD8806 library this way:

#ifndef _LPD8806_h
#define _LPD8806_h
...the LPD8806 library...
#endif

Now you can import the modified LPD8806 library to your IDE.

===============
LEDSegs object:
===============

The LEDSegs library defined here is written as a C++ class. It exposes methods and properties to make
creating LED strip effects simple. So first, include this library in your code.

  #include "LEDSegs.cpp"

If you want to change any of the cMax... constants described below, put those #define statements above
the #include for this code.

Next, in the Arduino setup() routine, instantiate an "LEDSegs" object, giving the total number of LEDs in
your strip:

  strip = new LEDSegs(LEDsInYourStrip);

This form of the constructor is for SPI output. If you just HAVE to use digital pins (remember
they're a lot slower), you give the two digital data/clock pin numbers explicitly as short int
arguments:

  strip = new LEDSegs(#LEDs, dataPin, clockPin);

The constructor for the LEDSegs object initializes it to a base state with no defined segments. You can
reset the LEDSegs object to this state at any time with a ResetStrip() call:

  strip->ResetStrip()

=========
Segments:
=========

Well, feeling pretty good huh? Now all you have to do is define your LED strip "segments" and you're
ready to start the damn party!

Define your strip's segments also in the setup() routine. For this, make calls to the LEDSegs
DefineSegment() method:

  strip->DefineSegment(start, n, action, color, bands [,part#]);

There are five required arguments to DefineSegment and one optional one:

  1) The starting LED (0-origin) of the segment (short int). In most cases this will be 0.

  2) The number of LEDs in this segment (short int). For a 5 meter strip this is 160.

  3) The type of basic action you want for the segment. Some actions are based on the input
     audio shield level. If you don't have a shield you won't want to use these.

     ***Audio Level-Based Actions:
     ________________
     cSegActionBottom: Illuminates LEDs up from the first LED in the segment based on audio level.
     ________________
     cSegActionTop:    Illuminates LEDs down, starting from the end of the segment down,
                       based on audio level.
     ________________
     cSegActionMiddle: Illuminates in both directions starting from the middle of the segment,
                       based on level.
     ________________
     cSegActionRandom: Illuminates LEDs randomly within the segment based on level.
          The randomization pattern stays fixed until:
            1) You reset the LEDSegs object, or
            2) You call the ResetRandom() method to change the randomization scheme. You can do this
                 any time, including setting a timer to do it periodically
            3) You call SetSegment_RandomPattern([iSegment,] 0..63) to select a different randomization
                 pattern from the current randomization scheme. See below.

     ***Non-Audio Level-Based Actions (no shield needed):
     ______________
     cSegActionAll: Illuminates all LEDs in the segment. Spectrum levels are ignored.
     _______________
     cSegActionBits:   Illuminates the segment's LEDs from a uint32_t[] sequence of bits.
          Spectrum levels are ignored for this action. "1" bits show as foreground
          color and "0" bits show as the background color. You must call the strip->SetBitsPtr()
          method to define a pointer to the bits longword(s) that you want displayed. The bits
          starting at this location are read out on each refresh cycle. This action is useful for
          rotating/chasing and sequencing displays and also diagnostic output to the strip.
          The low-order bit displays in the first LED of the segment. You can define as
          many bits as you like - it isn't limited to a single longword. Unused bits in the final
          longword are ignored.
     _______________
     cSegActionNone:   The segment is not displayed. You normally only use this with custom
         display routines (see below).

  4) The foreground (illumination) color (one of the RGBxxx color constants described below, or
     an LEDSegs::Color(R,G,B) value, where the R/G/B values are 0..127.

  5) A bitmask of cSegBand# (# from 1..7), representing which bands you want in the segment. I
     recommend you keep to the middle five (cSegBand2...cSegBand6), and avoid cSegBand1
     and cSegBand7 as they are mostly inaudible rubbish. (Especially cSegBand7 which specs
     an preposterous 16KHz center frequency.) Any segment can map any set of the 7 spectrum
     bands, and so of course there can be overlap using the same band is used in different segments.

     The spectrum shield has a fairly high inherent noise level and does not have any gain control
     or amplification. Thus this code attempts to do some simple noise gating and level normalization.
     Look at the code if you're interested.

  6) An optional "part" index. Parts are described below. If omitted, the part #0 is used, and for
     general applications will be what you want.
  
________________
Segment Example:

For example, here is a simple, 3-segment display for a 5-meter/160-LED strip: red, green and blue,
using different frequency bands from the analyzer for each segment:

  void setup() {
    strip->new LEDSegs(160);
    strip->DefineSegment( 0,  53, cSegActionFromTop,    RGBRed,   cSegBand2 | cSegBand3);
    strip->DefineSegment( 53, 53, cSegActionFromMiddle, RGBYellow, cSegBand4);
    strip->DefineSegment(106, 53, cSegActionFromBottom, RGBBlue,  cSegBand5 | cSegBand6);
    strip->TimedDisplay(40); //Refresh strip every 40ms
}

The TimedDisplay() registers a software timer that displays these segments every 40ms based on
current spectrum levels.

Now in your loop() routine, place a call to CheckTimers() to repeatedly check the segment timer and
display these segments each time the timer expires (every 40ms). Normally, this is all that's in the
loop() routine.

  loop() {strip->CheckTimers();}

This code will give you an LED strip action like this:

     <<....red*******|   <<..****yellow****..>>   |blue****....>>

The blue segment extends down from the 1/3 point on the strip based on the level of low frequencies.
The green maps the center band and expands outward in both directions from the middle of the strip,
and the red segment goes up from the 2/3 point.

________________
Segment Overlap:

Segments can overlap. Segments are "written" to the strip in index order, so later-defined (i.e. higher
index #) segments will overwrite your lower-index segments' LEDs if there is overlap.

The cSegOptNoOffOverwrite option can be given to a SetSegment_Options() call to prevent "off" LEDs
in that segment from overwriting earlier segments' LEDs, when an LED value in the segment is that
segments background color. Segment options are documented in more detail below.

______________
Segment Index:

Segments definitions are stored in an indexed array. They are written to the strip in index order.
Segments can be referenced in method calls by their index.

DefineSegment() sets the "current segment index" to the segment # being defined. (The "current"
is the index assumed when you call GetSegment_xxx()/SetSegment_xxx() methods without passing
an explicit index.) The first call to DefineSegment() sets the current segment index to 0.

A call to DefineSegment() locates an available segment by searching for a 0-LED length (undefined) segment
slot, starting at the current index and looping around.

  strip->GetSegmentIndex();  //returns the current (short integer) current segment index
  strip->SetSegmentIndex(n); //sets the current segment index to segment index "n".

If you want a particular segment # to be used by DefineSegment() then do a SetSegmentIndex(n) call
before the DefineSegment() call. DefineSegment() begins its search starting at the current segment.
As long as that segment is available, it will be the index defined, and that index will be the value
returned.

You can define up to cMaxSegments segments (100 by default). If you want a higher or lower max,
use a #define to set cMaxSegments before including this library.

You can "undefine" (free up) a segment by calling ResetSegment(iSegment). You can free up all segments
by calling ResetSegment(). ResetStrip() also un-defines all segments (and parts, etc.)
You can disable a segment's display without undefining it by setting it's action to cSegActionNone.
(A segment is undefined/available if it has a length=0 and an action of cSegActionNone)

___________________________
Get/Set Segment Properties:

Most properties of a segment can be read in your code and with GetSegment_property() methods.
These methods are mostly useful in custom segment display routines.

These methods are overloaded and you can call them without an index to get their value for the
current segment. Or you can specify a explicit segment index. E.g.:

  strip->GetSegment_ForeColor(); //assumes current segment index
  strip->GetSegment_ForeColor(isegment); //Gets forground color for specified segment #

Likewise the SetSegment_property([isegment, ] value) methods allow you to change properties
of a segment.

Here are the Get/Set methods for segment properties. Get methods return a type appropriate
for the item - usually a short int.

    Get/SetSegment_Action
    Get/SetSegment_BackColor
    Get/SetSegment_Bands
        SetSegment_BitsPtr        //(no Get method for this)
        SetSegment_DisplayRoutine //(no Get method for this)
    Get/SetSegment_FirstLED
    Get/SetSegment_ForeColor
    Get/SetSegment_Level
    Get/SetSegment_MaxLevel
    Get/SetSegment_NumLEDs
    Get/SetSegment_Options
    Get/SetSegment_Part
    Get/SetSegment_RandomPattern
        SetSegment_Persistence   //(no get method)
    Get/SetSegment_Spacing

Consult the class definition below to see the full list of Get/Set methods for segments.

________________
Segment Spacing:

Any segment can have an optional segment "spacing." A spacing of zero (the default) means all LEDs
in the segment are controlled during the segment's display.

You can set the segment's spacing to any value n > 0 with SetSegment_Spacing(n). This means that only
every (n+1)th LED in the segment's range will be addressed during a display cycle. LEDs in between are
skipped.

A positive spacing allows you to create spaced, overlapping segments that are "interleaved."
Spaced segments must still be defined by their total length, irrespective of the spacing.
To interleave spaced segments, specify the same spacing value for all, and bump the starting position
of each successive segment by +1. This works for all actions, including cSegActionMiddle.

IMPORTANT: the defined integer length of all interleaved segments MUST BE EQUAL in order for
them to overlap and display correctly.

__________________
Random Segments:

cSegAction_Random causes the segment's LEDs to be lit randomly, the total number lit will be in proportion
to the segment's current level. The randomization scheme stays fixed until a ResetStrip() or explicit
ResetRandom() call.

Within the current randomization scheme, you can select one of 32 randomization patterns [0...31] if you
want segments to have different randomizations without having to do a ResetRandom(). The default is
randomization pattern #0. There is no significance to the pattern number except that each is different.

You can define a timer or segment action routine, calling ResetRandom() or SetSegment_RandomPattern(),
if you want to shake things up.
________________
Segment Options:

You can define options for the segment using the SetSegment_Options(bitmask) method as follows:
  ______________________
  cSegOptNoOffOverwrite:

  This option prevents a background/off LED color from overwriting whatever is already set for
  that LED from the previous display of lower-index segments in this refresh cycle. Instead of
  writing the background color, the segment display simply skips over that LED, leaving it's
  color unaffected. Note all LEDs are set to OFF (=RGBOff) at the start of each display refresh cycle,
  so this doesn't carry across display refreshes.

  This option can be used to "merge" segments over each other with the earlier segment "showing through"
  the off LEDs on the higher-index segment. If you do this with cSegAction_Random segments that overlap,
  you need to use different randomization patterns on the different overlapped segments.
  _______________________
  cSegOptModulateSegment:

  This option modulates the RGB intensity of foreground-color LEDs illuminated in the segment between the
  background color and the foreground color based on the spectrum level. Only illuminated (foreground)
  LEDs are affected. RGB values are scaled independently and proportionately to the level for the segment.

  If the background color is RGBOff, then the intensity of the "on" LEDs will vary based on the volume of
  the bands mapped to that segment. If this option is applied to a cSegActionAll segment, then the
  entire segment will "pulse" according to the volume of the spectrum bands.
  _______________
  cSegOptBandAvg:
  
  By default, the level for a band on each display cycle is normalized to 0..1023, based on the max sample values
  of all bands mapped into the segment. This option changes that to make it the average across the bands.

You can't define options in the initial DefineSegment() call, you must make a call to SetSegmentOptions(),
e.g.:

    strip->DefineSegment(...);
    strip->SetSegment_Options(cSegOptModulateSegment | cSegOptBandAvg);
    
_____________
Value Scaling:

A segment's level can be from [0..1023 (==cMaxSegmentLevel)]. However, 1022 is the highest scaled value that will
be produced by the code. 1023 will only be generated as the current level if the actual input level reaches that value.
This allows you to detect over-modulation in a segment action routine if you want to.
    
You can define a rescaling array to remap levels before the segment is displayed. The format of this is a short int[] array.
The first [0] element is the number (N) of scaling pairs in the array. This is followed by N pairs of short int values.
[...inputlevel, outputlevel...]. N is required to be positive and so the total number of array elements will be 2N+1. The
array also requires that "inputlevel" increase monotonically for each pair.

The rescaling process finds the two pairs that bracket the value of the current level and rescales the level linearly within
that range. There is an assumed [0,0...] pair before the first pair, and a [...1023,1023] pair at the end.

For example, this will remap the levels of the current segment inversely, so an off level then illuminates all LEDs, down
to a full level where no LEDs will be on (not really recommended for most applications though):

  short int rescale[] = {2, 0, 1023, 1023, 0};
  strip->SetSegment_Recale(rescale);

======
Colors
======

LED colors are 32-bit unsigned values (type uint32_t) that can be encoded from RGB values using the static
LEDSegs::Color(R,G,B) method. R, G and B are red, green, blue values and can range from 0 to 127. Don't make
assumptions about the encoded format of this color value.

There are a number of predefined core color constants here:
  RGBGold, RGBSilver, RGBYellow, RGBOrange, RGBRed, RGBGreen, RGBCyan, RGBBlue, RGBPurple and RGBWhite
  
In addition there are destaturated versions of these core colors (except white itself):
  RGBGoldWhite, RGBSilverWhite, ... etc.

There also are dim and very dim versions of the core colors also, e.g.:
  RGBGoldDim, RGBGoldVeryDim, ... etc.
  

=====
Parts
=====

A "part" as defined here means a contiguous range of LEDs on the strip with a start, length and direction.
Once you've defined a part, you can create segments within that part to control how the segments display
relative to the part's definition. For short strips or simple applications, you don't need parts.

Parts are especially useful where you have your strip winding around, or up and down, and you
want to define your segments relative to these sections and their orientation, in order to make
coding your segments simpler.

"Up" direction for a part means illumination from the first LED at the beginning of the part. A "Down"
part reverses the display of segments in it, so they start illuminating in a mirror-image. (So the
display of cSegActionFromBottom and cSegActionFromTop segments are actually reversed when defined within
a "down"-direction part.)

There is one predefined part: part #0, which is the entire strip with "up" direction. If you don't associate
a segment to a part, it gets mapped to part #0.

To clarify a bit: assume a strip of 100 LEDs where the first 50 run left-to-right ("normal" order), then the
strip is bent into a "U" so the second 50 LEDs run right-to-left. You might want this second section
of the strip to have a "down" part definition, so that your segments defined in that part of the strip will
display left-to-right, as if there were two separate strips running in the same direction. This makes the
calculations and coding much simpler.

  strip->DefinePart(1, 0, 100, true);  //up part #1
  strip->DefinePart(2, 0, 100, false); //down part #2
  strip->DefineSegment(0, 50, cSegActionFromBottom, RGBBlue, -1, 1);
  strip->DefineSegment(0, 50, cSegActionFromBottom, RGBRed, -1, 2);

The first (blue) segment will display up from LED #0 for 10 LEDs. The second (red) segment will display
from LED #99 downwards 10 LEDs for 10 LEDs.

_______________
Defining Parts:

Define properties for a part index from part index 1...cMaxParts-1. (Part #0 is the entire strip with up
ordering and cannot be redefined.)

  strip->DefinePart(Index, Start, Len, DirUp);

  1) Index: The part index to define: 1...cMaxParts-1. (There is no "current" part index)
  2) Start: The absolute, 0-origin LED index of the first LED included in the part
  3) Len:   The number of LEDs in the part
  4) DirUp: Boolean: True for displaying segments in the part "up" from the first LED,
            or false to display "down", starting from the last LED.

Parts also are reset by a call to ResetStrip().

Segments are cropped (windowed) to the part they are in. So you can define a segment with, for instance,
a negative starting position, or a length greater than the length of the part. Segment LEDs outside the part's
defined range are not displayed.

Part logic is applied on each refresh cycle so parts also offer the ability to dynamically relocate a segment
simply by changing the part's properties within a segment display routine or timer expiration routine. If you
assign multiple segments to the same part (a typical use for parts), then changing the part's properties affects
the subsequent display of all segments in that part.

For example if you add +10 to a part's starting position, all the segments in it will then start their display
10 LEDs farther along the strip. You don't need to change each segment individually.

You place a segment in a part using either a DefineSegment() call with the final parameter giving the part #
as described above, or use the explicit segment method to set it into a part after the segment is defined:

  strip->SetSegment_Part([nSegment,] part-index);  //Places the segment into the designated part.

The maximum number of parts is defined by cMaxParts, which currently defaults to 20 (0..19). You can define
a larger value before including this library. You can redefine parts or part properties at any time,
however you cannot define or change the built-in part #0.

Any number of segments can be mapped into one part. They are processed as normal in segment index order
irrespective of their part, and all the usual segment logic applies.

As an illustration, these two are equivalent:

  strip->DefineSegment(40, 10, cSegActionFromTop, RGBBlue, -1);

     //..and...

  strip->DefinePart(1, 40, 10, false);
  strip->DefineSegment(0, 10, cSegActionFromBottom, RGBBlue, -1, 1);

The first line defines a segment (in part 0) which starts at the 41st LED, with a length of 10 LEDs,
and starts illuminating the segment downwards from LED #50.

The second two lines are equivalent: first define a part (#1) as starting at the 41st LED with a length
of 10 LEDs, going from the last LED and indexing down. The segment is now defined with the intent of
associating it to part #1; it's starting position is 0 (part-relative), and the segment action is defined
as cSegAction_FromBottom which inverts the effect of the part's direction: cSegPartDown. Thus the start of
the segment display still will be at LED #50, displaying downwards to LED #41.

You can use timer actions or custom segment display routines to dynamically alter part properties. For
example you can change the start value to move the part's display area around the strip on each timer
expiration.

____________
Persistence:

You can have a segment's net level include the prior level. This "persistence" allows you to dampen how
much the level can rise or fall, by weighting in the level from the previous sample. (So this operates
something like a simple RC circuit by controlling the rate of change.)

You give separate (short int) persistence values for increasing levels and decreasing in this call:

  SetSegment_Persistence([isegment,] up, down);

The values determine the weighting of the prior level into the current one. 1023 is defined as equal
weighting. Higher values are permitted. For example, this defines a "fast-attack, slow decay" persistence
for the current segment:

  SetSegment_Persistence(0, 1023);
  
The default for both values in a segment is 0, which means the prior sample's level isn't used.

_________________
Display Routines:

If you want serious customization of the LED display and you don't object to a bit of coding, you can
write custom "display routines" to get unique and sophisticated code-drive effects not possible
just with the basic segment definitions.

Display routines allow full program control of the segments on each refresh cycle. You can inspect and
dmodify any property of the segment, as well as other segments and parts, before the segments are
displayed.

To set a display routine for a segment, pass a reference to the routine you want called on each
display cycle for that segment, i.e.: SetSegment_DisplayRoutine(&your-routine-name). You can list the
same display routine for more than one segment.

A display routine gets called after the spectrum samples are gathered and have been normalized
to 0...cMaxSegmentLevel. But before the actual LED display refresh takes place. The display routine
is a void(short iseg) procedure that is called with a single short parameter: the index of that segment.

A display routine can inspect and change almost any of the segment properties, including color,
length, starting position, current normalized spectrum level, etc. These will be the values used
during display refresh. Use the SetSegment_xxx and GetSegment_xxx methods for this. The display
routine can also inspect and change properties of other segments and parts as well.

________________________
Segment Routine Example:

Here is an example. This was for a Christmas display. It moves a "sliding" segment up and down
the strip. The starting position of the segment is based on the total volume level, and the
slider segment's color is decided on each display cycle by which band has the largest amplitude.
There is also a background of soft blue for the whole strip.

    //**********
    const short nFirstLED = 0;      //First LED to illuminate
    const short nLEDs = 160;        //160 for a 5 meter strip
    const short C7SegLen = 30;      //The length of the "slider" segment
    const short C7SegMinLevel = 80; //The min level needed to show the slider
    short C7ColorSegs[3];           //Records the index of the three 0-length color segments
    uint32_t C7SegColors[3] = {RGBRed, RGBGreen, RGBGold};  //Colors to use based on loudest band

    void SegmentProgramChristmas7() {
      //Define a background for the whole strip
      strip->DefineSegment(nFirstLED, nLEDs, cSegActionAll, RGBBlueVeryDim, 0);

      //Define the "slider" segment. Color and starting position are set in the display routine
      strip->DefineSegment(0, C7SegLen, cSegActionAll, RGBBlue, 0x1E);
      strip->SetSegment_DisplayRoutine(&SegmentDisplayChristmas7);

      //Define three zero-length segments just to get the levels for the three bands we want to
      //check to set the color of the slider segment in the display routine
      C7ColorSegs[0] = strip->DefineSegment(0, 0, cSegActionNone, RGBRed, cSegBand2);
      C7ColorSegs[1] = strip->DefineSegment(0, 0, cSegActionNone, RGBGreen, cSegBand3);
      C7ColorSegs[2] = strip->DefineSegment(0, 0, cSegActionNone, RGBBlue, cSegBand4 | cSegBand5);
    }

    //Segment's display routine called for each display cycle
    void SegmentDisplayChristmas7(short iSegment) {
      short curLevel, startPos;
      uint32_t thiscolor;
      short colorseg, iseg, maxcolorlevel, thislevel;

      //Find the color to use based on the "loudest" band
      curLevel = strip->GetSegment_Level(iSegment) - C7SegMinLevel;
      maxcolorlevel = -1; colorseg = 0;
      for (iseg = 0; iseg < 3; iseg++) {
        thislevel = strip->GetSegment_Level(C7ColorSegs[iseg]);
        if (thislevel > maxcolorlevel) {maxcolorlevel = thislevel; colorseg = iseg;}
      }

      //Set the starting position of the strip. We have to scale the current level
      //to the possible starting positions. There's some small rounding error here
      //but it's not meaningful
      if (curLevel < 0) {strip->SetSegment_Action(iSegment, cSegActionNone);}
      else {
        strip->SetSegment_Action(iSegment, cSegActionAll);
        strip->SetSegment_ForeColor(iSegment, C7SegColors[colorseg]);
        startPos = ((curLevel * (nLEDs - C7SegLen)) /
            (cMaxSegmentLevel - C7SegMinLevel)) + nFirstLED;
        startPos = constrain(startPos, 0, nLastLED); //safety
        strip->SetSegment_FirstLED(iSegment, startPos);
      }
    }
    //***********

==============
Timer Objects:
==============

The LEDSegs:: class provides a set of synchronous timer methods you can use to handle display,
as well as other delayed and/or periodic tasks. The TimedDisplay() method described above for
the loop() routine creates a timer internally to handle the display sampling and refresh.

Create timers using the DefineTimer() method, calling it with these parameters:

  itimer = LEDSegs::DefineTimer(expiration, repeat, routine [,ptr]);
  
    itimer:     The returned timer index that was created. If 0 it wasn't created because there are
                no more timer slots. itimer is always positive (non-zero) if the timer was created.
    expiration: An offset expiration in milliseconds from the current mills() time. When this time is
                reached, the timer's routine gets called as soon as CheckTimers() sees the expiration.
    repeat:     A repeat time in MS. If zero then the timer fires once and is then freed up
    routine:    A void(short int) routine called on timer expiration. The argument is the timer index.
    ptr:        An arbitrary void* pointer passed to routine. You can cast this to an explicit pointer
                type if you use it. If omitted as in most cases, it defaults to NULL

You can have up to cMaxTimers (default is 32). You can set this prior to including this library if you want
a different number. If itimer is returned as -1, the timer was not created.

To cancel a timer, call:

  strip->CancelTimer(itimer);

You can inspect/adjust the properties of timers using any of these methods:

  Get/SetTimerExpiration(itimer, expirationdelay);
  Get/SetTimerRepeat(itimer, repeatdelay);
      SetTimerRoutine(itimer, routine);   //(no Get method for this)
      SetTimerPtr(itimer, void *ptr);     //(no Get method for this)

You will usually create your timers in the Arduino setup() routine. The CheckTimers() you put the loop()
routine will scan the timer queue and wait for expirations. CheckTimers() does ONE pass across all defined
timers, calling the timer routines for any that are active and have expired.

Timers are software constructs and as such entirely synchronous. A timer expiration can't "interrupt"
ongoing processing. When a timer routine completes and returns, the CheckTimers() scan continues for
any more active, expired timers.

===================
Dead Air Detection:
===================

The LEDSegs class has the ability to sense a no-input condition. You can use this to detect when the sound
has been turned on or off, or when there is a break in the audio for a given amount of time.

You first need to enable detection using EnableDeadAirDetect(level). Place this in the setup() routine.
"level" is the maximum level [0..1023] allowed before the input is not considered "dead air". 50-80
is about right for most applications.

After this, you can call CheckForDeadAir(secs) as needed, which returns true if there has been no
input above the given "level" for "secs" seconds.
*/

/* Start of LEDSEGS:: */

#include "LEDSegs.h"

/*
______________
LEDBits Class:

Some general routines for bit manipulation (this class is inherited in LEDSegs). This is very
much a work in progress and the specifics shouldn't be assumed for future releases
*/

/*
_________________
LEDSegs::BitRead:
Get the value of a bit in a bit field
*/
bool LEDBits::BitRead(short bitnum, uint8_t byteary[]) {return (byteary[bitnum >> 3] >> (bitnum & 0x07)) & 1;}

/*_________________
LEDSegs::BitRotate:
Left/right rotation by one bit of any arbitrary field of bits. bitwidth is the number
of bits, and bitary is the array of 8-bit bytes containing the bits to be rotated. The
"first" bit is bit #0 in bitary[0]. nbits gives the number of bits to rotate, positive
is a left rotate and negative is to the right.
*/

void LEDBits::BitRotate(short bitwidth, uint8_t bitary[], short int nbits) {
  short ibyte, lastbyte, bitstomove, irotate;
  byte nextcarry, lastcarry;

  lastbyte = ((bitwidth + 7) >> 3) - 1;
  bitstomove = abs(nbits);
  for (irotate = 0; irotate < bitstomove; irotate++) {
    if (nbits > 0) {  //left rotate
      lastcarry = (bitary[lastbyte] >> ((bitwidth - 1) & 0x07)) & 1;
      for (ibyte = 0; ibyte <= lastbyte; ibyte++) {
        nextcarry = bitary[ibyte] & 0x80;
        bitary[ibyte] = (bitary[ibyte] << 1) | (lastcarry ? 1 : 0);
        lastcarry = nextcarry;
      }
    }
    else {
      lastcarry = bitary[0] & 1;
      for (ibyte = lastbyte; ibyte >= 0; ibyte--) {
        nextcarry = bitary[ibyte] & 1;
        bitary[ibyte] = (bitary[ibyte] >> 1) | (lastcarry ? 0x80 : 0);
        lastcarry = nextcarry;
      }  
    }
  }
}

/*
_________________________________
LEDTimers Class Member Functions:

The LEDTimers class is inherited in LEDSegs.
*/

//LEDTimers:: Constructor
LEDTimers::LEDTimers() {
  short int i;
  for (i = 0; i < cMaxTimers; i++) {
    Timers[i].timerExpiration = 0;
    Timers[i].timerRepeat = 0;
    Timers[i].timerPtr = NULL;
  }
}

unsigned short LEDTimers::DefineTimer(unsigned long expirationMS, unsigned long repeatMS, TimerRoutine timerSub) {
  DefineTimer(expirationMS, repeatMS, timerSub, NULL);
}

//Define a timer. (Note that we simply don't use index 0 and start at 1 -- making timer IDs positive and
//non-zero simplifies a lot of coding.)

unsigned short LEDTimers::DefineTimer(unsigned long expirationMS, unsigned long repeatMS, TimerRoutine timerSub, void *ptr) {
  short int i;
  for (i = 1; i < cMaxTimers; i++) {
    if (Timers[i].timerExpiration == 0) {
      SetTimerExpiration(i, expirationMS);
      SetTimerRepeat(i, repeatMS);
      Timers[i].timerSub = timerSub;
      Timers[i].timerPtr = ptr;
      return i;
    }
  }
  return 0;
}

void LEDTimers::CancelTimer(short timerID) {
  if ((timerID > 0) && (timerID < cMaxTimers)) {
    Timers[timerID].timerExpiration = 0;
    Timers[timerID].timerRepeat = 0;
    Timers[timerID].timerPtr = NULL;}
}
    
void LEDTimers::CheckTimers() {
  short int i;
  unsigned long curtime, newexpiration;

  //Loop all timers once, looking for the active, expired ones
  for (i = 1; i < cMaxTimers; i++) {
    if (Timers[i].timerExpiration > 0) {
             
      //If this active timer has now expired, process it.
      curtime = millis();
      if (Timers[i].timerExpiration <= curtime) {
 
        //Call the timer routine with the timer's pointer arg
        Timers[i].timerSub(i, Timers[i].timerPtr);
 
        //Now cancel a one-time timer, or repeat if repeating and still active
        curtime = millis();
        newexpiration = 0;
        if ((Timers[i].timerExpiration > 0) && (Timers[i].timerRepeat > 0)) {
          newexpiration = Timers[i].timerExpiration + Timers[i].timerRepeat;
          if (newexpiration <= curtime) newexpiration = curtime + 1;
        };
        Timers[i].timerExpiration = newexpiration;
      }
    }
  }
}
 
//Get/Set methods for timers
    
unsigned long int LEDTimers::GetTimerExpiration(short int itimer) {return(Timers[itimer].timerExpiration);}
void LEDTimers::SetTimerExpiration(short int itimer, unsigned long int exp) {Timers[itimer].timerExpiration = exp + millis();}
    
unsigned long int LEDTimers::GetTimerRepeat(short int itimer) {return(Timers[itimer].timerRepeat);}
void LEDTimers::SetTimerRepeat(short int itimer, unsigned long int rpt) {Timers[itimer].timerRepeat = rpt;}

void LEDTimers::SetTimerRoutine(short int itimer, TimerRoutine timerroutine) {Timers[itimer].timerSub = timerroutine;}
void LEDTimers::SetTimerPtr(short int itimer, void *ptr) {Timers[itimer].timerPtr = ptr;}

/*
____________________________________________
LED strip class (LEDSegs::) member functions
*/

//Create a timer that refreshes the display. teTimedDisplay is private
short int LEDSegs::TimedDisplay(short int timeMS) {return(DefineTimer(timeMS, timeMS, LEDSegs::teTimedDisplay, this));}

//Reset the random permutation array (for cSegActionRandom)
void LEDSegs::ResetRandom() {
  unsigned short i;
  randomSeed(micros());
  for (i = 0; i < cSegNRandom; i++) {segRandomLevels[i] = random(cMaxSegmentLevel);}
}

void LEDSegs::SetSegmentIndex(short Idx) {
  segCurrentIndex = constrain(Idx, 0, cMaxSegments - 1);
  segMaxDefinedIndex = max(segMaxDefinedIndex, segCurrentIndex);
}
short LEDSegs::GetSegmentIndex() {return segCurrentIndex;}

//Smallest allowed max level (default=1023) and decay (default=1) of max level on each refresh cycle

void LEDSegs::SetMaxLevelFloor(short int iFloor) {stripMaxLevelFloor = constrain(iFloor, 1, cMaxSegmentLevel);}
short int LEDSegs::GetMaxLevelFloor() {return stripMaxLevelFloor;}
void LEDSegs::SetMaxLevelDecay(short int iDecay) {stripMaxLevelDecay = constrain(iDecay, 1, cMaxSegmentLevel);}
short int LEDSegs::GetMaxLevelDecay() {return stripMaxLevelDecay;}

//The SetSegment_xxx and GetSegment_xxx routines are overloaded. The segment # parameter
//can be omitted and defaults to the current segment index. Note that there are no GET methods
//for a couple of properties.

void LEDSegs::SetSegment_Action(short nSegment, short Action) {if (Action >= 0) {SegmentData[nSegment].segAction = Action;};}
void LEDSegs::SetSegment_Action(short Action) {SetSegment_Action(segCurrentIndex, Action);}
void LEDSegs::SetSegment_BackColor(short nSegment, uint32_t BackColor) {SegmentData[nSegment].segBackColor = BackColor;}
void LEDSegs::SetSegment_BackColor(uint32_t BackColor) {SetSegment_BackColor(segCurrentIndex, BackColor);}
void LEDSegs::SetSegment_Bands(short nSegment, short Bands) {SegmentData[nSegment].segBands = Bands; SegmentData[nSegment].segMaxLevel = stripMaxLevelFloor;}
void LEDSegs::SetSegment_Bands(short Bands) {SetSegment_Bands(segCurrentIndex, Bands);}
void LEDSegs::SetSegment_DisplayRoutine(short nSegment, SegmentDisplayRoutine Routine) {SegmentData[nSegment].segDisplayRoutine = *Routine;}
void LEDSegs::SetSegment_DisplayRoutine(SegmentDisplayRoutine Routine) {SetSegment_DisplayRoutine(segCurrentIndex, Routine);}
void LEDSegs::SetSegment_FirstLED(short nSegment, short FirstLED) {SegmentData[nSegment].segFirstLED = FirstLED;}
void LEDSegs::SetSegment_FirstLED(short FirstLED) {SetSegment_FirstLED(segCurrentIndex, FirstLED);}
void LEDSegs::SetSegment_ForeColor(short nSegment, uint32_t ForeColor) {SegmentData[nSegment].segForeColor = ForeColor;}
void LEDSegs::SetSegment_ForeColor(uint32_t ForeColor) {SetSegment_ForeColor(segCurrentIndex, ForeColor);}
void LEDSegs::SetSegment_Level(short nSegment, short level) {SegmentData[nSegment].segLevel = constrain(level, 0, cMaxSegmentLevel);}
void LEDSegs::SetSegment_Level(short level) {SetSegment_Level(segCurrentIndex, level);}
void LEDSegs::SetSegment_MaxLevel(short maxlevel) {SetSegment_Level(segCurrentIndex, maxlevel);}
void LEDSegs::SetSegment_MaxLevel(short nSegment, short maxlevel) {SegmentData[nSegment].segMaxLevel = maxlevel;}
void LEDSegs::SetSegment_NumLEDs(short nSegment, short nLEDs) {if ((nLEDs >= 0) && (nLEDs <= nLEDsInStrip)) {SegmentData[nSegment].segNumLEDs = nLEDs;};}
void LEDSegs::SetSegment_NumLEDs(short nLEDs) {SetSegment_NumLEDs(segCurrentIndex, nLEDs);}
void LEDSegs::SetSegment_Part(short nSegment, short partNum) {if ((partNum >= 0) && (partNum < cMaxParts)) SegmentData[nSegment].segPart = partNum;}
void LEDSegs::SetSegment_Part(short partNum) {SetSegment_Part(segCurrentIndex, partNum);}
void LEDSegs::SetSegment_BitsPtr(short nSegment, uint32_t *ptrval) {SegmentData[nSegment].segBitsPtr = ptrval;}
void LEDSegs::SetSegment_BitsPtr(uint32_t *ptrval) {SetSegment_BitsPtr(segCurrentIndex, ptrval);}
void LEDSegs::SetSegment_Options(short nSegment, short Options) {if (Options >= 0) {SegmentData[nSegment].segOptions = Options;};}
void LEDSegs::SetSegment_Options(short Options) {SetSegment_Options(segCurrentIndex, Options);}
void LEDSegs::SetSegment_Persistence(short up, short down) {SetSegment_Persistence(segCurrentIndex, up, down);}
void LEDSegs::SetSegment_Persistence(short nSegment, short up, short down) {SegmentData[nSegment].segPersistUp = up; SegmentData[nSegment].segPersistDown = down;}
void LEDSegs::SetSegment_RandomPattern(short nSegment, short RandomPattern) {if (RandomPattern >= 0) {SegmentData[nSegment].segRandomPattern = RandomPattern & cSegNRandomMask;};}
void LEDSegs::SetSegment_RandomPattern(short RandomPattern) {SetSegment_RandomPattern(segCurrentIndex, RandomPattern);}
void LEDSegs::SetSegment_Spacing(short nSegment, short Spacing) {if (Spacing >= 0) {SegmentData[nSegment].segSpacing = Spacing;};}
void LEDSegs::SetSegment_Rescale(const short int *scaleary) {SetSegment_Rescale(segCurrentIndex, scaleary);}
void LEDSegs::SetSegment_Rescale(short nSegment, const short int *scaleary) {SegmentData[nSegment].segRescaleAry = scaleary;}
void LEDSegs::SetSegment_Spacing(short Spacing) {SetSegment_Spacing(segCurrentIndex, Spacing);}

short    LEDSegs::GetSegment_Action(short nSegment)    {return SegmentData[nSegment].segAction;}
short    LEDSegs::GetSegment_Action()                  {return SegmentData[segCurrentIndex].segAction;}
uint32_t LEDSegs::GetSegment_BackColor(short nSegment) {return SegmentData[nSegment].segBackColor;}
uint32_t LEDSegs::GetSegment_BackColor()               {return SegmentData[segCurrentIndex].segBackColor;}
short    LEDSegs::GetSegment_Bands(short nSegment)     {return SegmentData[nSegment].segBands;}
short    LEDSegs::GetSegment_Bands()                   {return SegmentData[segCurrentIndex].segBands;}
short    LEDSegs::GetSegment_FirstLED(short nSegment)  {return SegmentData[nSegment].segFirstLED;}
short    LEDSegs::GetSegment_FirstLED()                {return SegmentData[segCurrentIndex].segFirstLED;}
uint32_t LEDSegs::GetSegment_ForeColor(short nSegment) {return SegmentData[nSegment].segForeColor;}
uint32_t LEDSegs::GetSegment_ForeColor()               {return SegmentData[segCurrentIndex].segForeColor;}
short    LEDSegs::GetSegment_Level(short nSegment)     {return SegmentData[nSegment].segLevel;}
short    LEDSegs::GetSegment_Level()                   {return SegmentData[segCurrentIndex].segLevel;}
short    LEDSegs::GetSegment_MaxLevel(short nSegment)  {return SegmentData[nSegment].segMaxLevel;}
short    LEDSegs::GetSegment_MaxLevel()                {return SegmentData[segCurrentIndex].segMaxLevel;}
short    LEDSegs::GetSegment_NumLEDs(short nSegment)   {return SegmentData[nSegment].segNumLEDs;}
short    LEDSegs::GetSegment_NumLEDs()                 {return SegmentData[segCurrentIndex].segNumLEDs;}
short    LEDSegs::GetSegment_Options(short nSegment)   {return SegmentData[nSegment].segOptions;}
short    LEDSegs::GetSegment_Options()                 {return SegmentData[segCurrentIndex].segOptions;}
short    LEDSegs::GetSegment_RandomPattern(short nSegment) {return SegmentData[nSegment].segRandomPattern;}
short    LEDSegs::GetSegment_RandomPattern()           {return SegmentData[segCurrentIndex].segRandomPattern;}
short    LEDSegs::GetSegment_Spacing(short nSegment)   {return SegmentData[nSegment].segSpacing;}
short    LEDSegs::GetSegment_Spacing()                 {return SegmentData[segCurrentIndex].segSpacing;}

//Define segment methods

short LEDSegs::DefineSegment(short firstled, short nleds, short action, uint32_t forecolor, short bands) {
  return DefineSegment(firstled, nleds, action, forecolor, bands, 0);
};

//Free up a given segment. Current index is left undefined
void LEDSegs::ResetSegment(short int i) {
  SegmentData[i].segAction = cSegActionNone;
  SegmentData[i].segNumLEDs = -1;  //This and a none action marks an available segment
  if (segMaxDefinedIndex == i) segMaxDefinedIndex--;
  segCurrentIndex = -1;
}

//Free up all segments. Current index is left undefined
void LEDSegs::ResetSegments() {
  short int i;
  for (i = 0; i < cMaxSegments; i++) {ResetSegment(i);}
  segCurrentIndex = -1;
  segMaxDefinedIndex = -1;
}

//Methods that match LPD8806 member function, except declared static and does not set the high bit (this
//is done by LPD8806 setPixelColor. Return value is GRB (not RGB!) value in long int.
    
uint32_t LEDSegs::Color(byte r, byte g, byte b) {
  return ((uint32_t)(g) << 16) | ((uint32_t)(r) <<  8) | b;
}

//Get the r/g/b components of a color into byte values (remember value is GRB, not RGB), return array [0..2]
//is ordered RGB

void LEDSegs::Colorvals(uint32_t Color, byte rgbvals[]) {
  rgbvals[1] = ((Color >> 16) & 0x7F);
  rgbvals[0] = ((Color >> 8) & 0x7F);
  rgbvals[2] = (Color & 0x7F);
}
    
/* Parts methods (public) */

void LEDSegs::DefinePart(short partNum, short partStart, short partLen, bool partUp) {
  if ((partNum < 1) || (partNum >= cMaxParts)) return;
  stripParts[partNum].start = constrain(partStart, 0, nLEDsInStrip);
  stripParts[partNum].len = constrain(partLen, 0, nLEDsInStrip);
  stripParts[partNum].partup = partUp;
}

short LEDSegs::GetPart_Start(short ipart) {return stripParts[ipart].start;}
void  LEDSegs::SetPart_Start(short ipart, short partstart) {stripParts[ipart].start = partstart;}

short LEDSegs::GetPart_Len(short ipart) {return stripParts[ipart].len;}
void  LEDSegs::SetPart_Len(short ipart, short partlen) {stripParts[ipart].len = partlen;}

bool LEDSegs::GetPart_Up(short ipart) {return stripParts[ipart].partup;}
void LEDSegs::SetPart_Up(short ipart, bool up) {stripParts[ipart].partup = up;}

/* Dead air detection public methods */
bool LEDSegs::CheckForDeadAir(short secs) {return DeadAirSecondsCount >= secs;}
void LEDSegs::DisableDeadAirDetect() {CancelTimer(DeadAirDetectTimerID);}
void LEDSegs::EnableDeadAirDetect(short int level) {
  DisableDeadAirDetect();
  DeadAirLevel = level * 3; //3 is the number of bands summed for the level
  DeadAirDetectTimerID = DefineTimer(1000, 1000, teCheckForDeadAir, this); 
}

//Called by TimedDisplay() timer routine on expiration
void LEDSegs::teTimedDisplay(short int itimer, void *ptr) {((LEDSegs *) ptr)->DisplayStrip(true, true);}

//Initialize the parts array (all parts = entire strip with up order)
void LEDSegs::ResetParts() {
  short i;
  for (i = 0; i < cMaxParts; i++) {
    stripParts[i].start = 0;
    stripParts[i].len = nLEDsInStrip;
    stripParts[i].partup = true;
  }
}

//Called on dead air timer expiration every second. We sum selected bands' maxes to check for signal.
//ptr is the timer pointer, which is set to the "this" pointer for the segment class instance.
void LEDSegs::teCheckForDeadAir(short itimer, void *ptr) {
  short SumOfMax, iband;
  LEDSegs *segsptr = (LEDSegs *) ptr;
  SumOfMax = 0;
  for (iband = 1; iband <= 3; iband++) {SumOfMax += segsptr->SpectrumMax[iband]; segsptr->SpectrumMax[iband] = 0;}
  if (SumOfMax <= segsptr->DeadAirLevel) segsptr->DeadAirSecondsCount++; else segsptr->DeadAirSecondsCount = 0;
}
    
//Constructor and destructor
LEDSegs::LEDSegs(short nLEDs) {
  LEDSegsInit(nLEDs, true, 0, 0); //Constructor with default data/clock
}
LEDSegs::LEDSegs(short nLEDs, short pinData, short pinClock) {
  LEDSegsInit(nLEDs, false, pinData, pinClock); //Constructor with explicit data/clock
}

LEDSegs::~LEDSegs() {
  delete[] objLPDStrip;
}

/*
____________
LEDSegsInit:Common constructor code
*/

void LEDSegs::LEDSegsInit(short nLEDs, bool useSPI, short pinData, short pinClock) {
  short i;
  
  //Create an LED strip object. Either SPI or digital pins

  if (useSPI) objLPDStrip = new LPD8806(nLEDs);
  else objLPDStrip = new LPD8806(nLEDs, pinData, pinClock);

  nLEDsInStrip = nLEDs;

  //Setup pins to drive the spectrum analyzer.

  pinMode(cSpectrumReset, OUTPUT);
  pinMode(cSpectrumStrobe, OUTPUT);

  //Init spectrum analyzer to start reading from lowest band
  digitalWrite(cSpectrumStrobe, LOW);
  delay(1);
  digitalWrite(cSpectrumReset, HIGH);
  delay(1);
  digitalWrite(cSpectrumStrobe, HIGH);
  delay(1);
  digitalWrite(cSpectrumStrobe, LOW);
  delay(1);
  digitalWrite(cSpectrumReset, LOW);
  delay(5);

  //Init this guy
  ResetStrip();
}

/*____________________
LEDSegs::DefineSegment
Set the properties of the current LED segment. Return value is the segment index or -1 if no available segments
This routine sets the current segment index.
*/

short LEDSegs::DefineSegment(short FirstLED, short nLEDs, short Action, uint32_t ForeColor, short Bands, short PartIndex) {

  short int i, iseg;
  
  //Loop all segment slots starting with the current, to find the first available (indicated by -1 length and no action).
  //We do it this way so if somebody wants to define a particular segment index then just call SetSegmentIndex(n)
  //before calling DefineSegment(...).
  
  for (i = 0; i < cMaxSegments; i++) {
    iseg = i + segCurrentIndex;
    if (iseg >= cMaxSegments) iseg = iseg - cMaxSegments;
    if ((SegmentData[iseg].segNumLEDs < 0) && (SegmentData[iseg].segAction == cSegActionNone)) break;
  }

  //If none available, return -1, and set iseg to be new current segment index
  if (i >= cMaxSegments) {return(segCurrentIndex = -1);}
  segCurrentIndex = iseg;

  //Track the highest segment index defined. This speeds the refresh loop a bit esp. if cMaxSegments is big.
  segMaxDefinedIndex = max(segMaxDefinedIndex, segCurrentIndex);
  
  //Set the segment properties passed in

  SetSegment_Part(constrain(PartIndex, 0, cMaxParts - 1));
  SetSegment_FirstLED(FirstLED);
  SetSegment_NumLEDs(nLEDs);
  SetSegment_Action(Action);
  SetSegment_ForeColor(ForeColor);
  SetSegment_Bands(Bands);

  //Segment defaults
  SetSegment_BackColor(RGBOff);
  SetSegment_Spacing(0);
  SetSegment_Options(0);
  SetSegment_DisplayRoutine(NULL);
  SetSegment_BitsPtr(NULL);
  SetSegment_RandomPattern(0);
  SetSegment_Persistence(0, 0);
  SegmentData[iseg].segRescaleAry = NULL;
  SegmentData[iseg].segLevel = 0;
  
  //Return the segment index that was defined
  return segCurrentIndex;
};

/*______________________
LEDSegs::DisplayStrip
Sample and display according to the defined segments
*/

void LEDSegs::DisplayStrip(bool doLeft, bool doRight) {
  ReadSpectrum(doLeft, doRight);
  MapBandsToSegments();
  ShowSegments();
};

/*_________________________
LEDSegs::MapBandsToSegments
Convert spectrum band samples into LED strip segment values in the range 0..(#-LEDs-in-segment).
We average all the bands defined for the segment, and then scale the final segment value
*/

void LEDSegs::MapBandsToSegments() {
  short iSegment, iBand, segBands, numbands, scaledTotal, maxTotal;
  short iscale, peak1, peak2, out1, out2, nscalemax;
  long sampleTotal;
  const short int *rescaleary;
  bool useBandMax;
  long int dividend, persist, lastlevel;

  //Loop all defined segments to calculate the normalized band value. We do this even for ActionNone
  //in case a segment display routine wants to change the action

  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {
    if (SegmentData[iSegment].segNumLEDs >= 0) {
      segBands = SegmentData[iSegment].segBands;
      useBandMax = ! (SegmentData[iSegment].segOptions & cSegOptBandAvg);
    
      //Loop all bands in the segment to accumulate the sample total and the max levels

      sampleTotal = 0;
      numbands = 0;

#if defined DIAGSEGS
    Serial.print("Values for Seg. "); Serial.print(iSegment); Serial.print("  ");
    Serial.print("Prev.Max="); Serial.print(SegmentData[iSegment].segMaxLevel); Serial.print(", ");
#endif

      for (iBand = 0; iBand < cSegNumBands; iBand++) {
        if ((segBands >> iBand) & 1) {
          numbands++;
          if (useBandMax) {sampleTotal = max(sampleTotal, SpectrumLevel[iBand]);}
          else {sampleTotal += SpectrumLevel[iBand];}
        }
      }
      if (numbands == 0) numbands = 1; //Safety
    
      //Average by number of bands, or if using BandMax option then use sample total
      scaledTotal = useBandMax ? sampleTotal : sampleTotal / numbands;

      //Compute max and record in segment
      maxTotal = SegmentData[iSegment].segMaxLevel;
      maxTotal = maxTotal - stripMaxLevelDecay;
      if (maxTotal < stripMaxLevelFloor) maxTotal = stripMaxLevelFloor;
      if (maxTotal <= scaledTotal) maxTotal = scaledTotal;
      SegmentData[iSegment].segMaxLevel = maxTotal;

#if defined DIAGSEGS
Serial.print("Total="); Serial.print(sampleTotal); Serial.print(",");
Serial.print("Scaled="); Serial.print(scaledTotal); Serial.print(",");
Serial.print("Max="); Serial.print(SegmentData[iSegment].segMaxLevel); Serial.print(",");
#endif

      //Scale level to [0..1022] based on max. We only allow scaling up to 1022. This allows
      //an action routine to detect clipping when the raw value is 1023
      if (scaledTotal < cMaxSegmentLevel) {
        scaledTotal = ((long) (scaledTotal * cMaxSegmentLevel)) / ((long) maxTotal);
#if defined DIAGSEGS
      Serial.print("Normalized="); Serial.print(scaledTotal); Serial.print(",");
#endif        
        //If a rescaling array, do that now
        rescaleary = SegmentData[iSegment].segRescaleAry;
        if (rescaleary != NULL) {
          nscalemax = (2 * rescaleary[0]) + 1;
          for (iscale = 1; iscale < nscalemax; iscale += 2) {if (rescaleary[iscale] > scaledTotal) break;}
          if (iscale == 1) {peak1 = out1 = 0;}
            else {peak1 = rescaleary[iscale-2]; out1 = rescaleary[iscale-1];}
          if (iscale >= nscalemax) {peak2 = out2 = cMaxSegmentLevel;}
            else {peak2 = rescaleary[iscale]; out2 = rescaleary[iscale+1];}
          scaledTotal = out1 + (((long) (out2 - out1) * (long) (scaledTotal - peak1)) / (long) (peak2 - peak1));

          //Limit scaled value to 1022
          if (scaledTotal >= cMaxSegmentLevel) scaledTotal = cMaxSegmentLevel - 1;
        }
      }

      //If we have persistence, do that calc. Note that .segLevel must still be set to the prior level
      lastlevel = SegmentData[iSegment].segLevel;
      persist = scaledTotal < lastlevel ? SegmentData[iSegment].segPersistDown : SegmentData[iSegment].segPersistUp;      
      if (persist > 0) {
        dividend = persist * SegmentData[iSegment].segLevel; //(actually still last level)
        dividend = dividend + (scaledTotal * cMaxSegmentLevel);
        scaledTotal = dividend / (persist + cMaxSegmentLevel);
      }
      
      //Record final scaled level for segment
      SegmentData[iSegment].segLevel = scaledTotal;      
      
#if defined DIAGSEGS
    if ((scaledTotal < 0) || (scaledTotal > cMaxSegmentLevel)) {Serial.print(" OUT OF RANGE! ");} else {Serial.print("Final=");}
    Serial.print(scaledTotal);
    Serial.println("");
#endif

    } //end if-segment-defined
  } //end segments loop
};

/*___________________
LEDSegs::ReadSpectrum
Read the spectrum band samples into class array SpectrumLevel[].
"Channels" tells whether to read left, right, or average both channels.
*/
void LEDSegs::ReadSpectrum(bool doLeft, bool doRight) {
  short iBand, thisLevel;
  short leftLevel, rightLevel;
  
#if defined DIAGSEGS
  Serial.println();
  Serial.print("Bands: (L/R/Cur/Max):");
#endif

  //This loop happens nBands times per sample. It just records the current sample values
  //into the band values array and subtracts out the noise floor
  for (iBand = 0; iBand < cSegNumBands; iBand++) {

    //Read the spectrum for this band
    thisLevel = 0;
    if (doLeft)  thisLevel = leftLevel = analogRead(cSegSpectrumAnalogLeft);
    if (doRight) thisLevel = rightLevel = analogRead(cSegSpectrumAnalogRight);
    if (doLeft && doRight) {thisLevel = max(leftLevel, rightLevel);}

#if defined DIAGSEGS
    Serial.print(" ");
    Serial.print(iBand); Serial.print("=(");
    Serial.print(leftLevel); Serial.print(",");
    Serial.print(rightLevel); Serial.print(",");
#endif

    //Subtract out assumed noise floor for this band
    thisLevel -= cBandNoiseFloor[iBand];
    if (thisLevel < 0) thisLevel = 0;

    //Set current value for this band
    SpectrumLevel[iBand] = thisLevel;
    SpectrumMax[iBand] = max(SpectrumMax[iBand], thisLevel);

#if defined DIAGSEGS
    Serial.print(SpectrumLevel[iBand]); Serial.print(",");
    Serial.print(SpectrumMax[iBand]); Serial.print(")");
#endif

    //Toggle to readout next band
    digitalWrite(cSpectrumStrobe, HIGH);
    digitalWrite(cSpectrumStrobe, LOW);
  }
#if defined DIAGSEGS
  Serial.println();
#endif
}

/*_________________
LEDSegs::ResetStrip
Reset the LED strip to initial state
*/

void LEDSegs::ResetStrip() {
  short iband;
  ResetSegments();
  segCurrentIndex = 0;
  ResetParts();
  stripMaxLevelDecay = 1;
  stripMaxLevelFloor = cMaxSegmentLevel;
  ResetRandom(); //Init the random permutation array (for cSegActionRandom)
  DeadAirDetectTimerID = -1;
  for (iband = 0; iband < cSegNumBands; iband++) {SpectrumMax[iband] = 0;} //Reset band maxes
  objLPDStrip->begin(); //Clear and init the strip
  objLPDStrip->show();  //Update the LED strip display to display all off to start
}

/*___________________
LEDSegs::ShowSegments
Display the segment values on the LED strip. You're in for a hairy ride...
*/

void LEDSegs::ShowSegments() {
  short    iSegment, iLEDinSegment, iLED, LEDIncrement, segval;
  short    partStart, partLen, partEnd;
  short    segFirstLED, segNumLEDs, segRandomPattern;
  short    Action, Options, segSpacing1, SpacingCount;
  bool     optOffOverwrite, optModulate, notSpacingLED, partUp;
  uint32_t thisColor, backColor, foreColor;
  byte     bcRGB[3], fcRGB[3];
  stripSegment *segptr;
  uint32_t (*bitsary);
  short bitscounter;
  static uint32_t zerobits = 0;
  SegmentDisplayRoutine routine;
  
  //Call any defined segment display routines that are defined

  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {
    routine = SegmentData[iSegment].segDisplayRoutine;
    if (routine != NULL) routine(iSegment);
  };

  //Init all LEDs in the strip to off
  for (iLED = 0; iLED < nLEDsInStrip; iLED++) {objLPDStrip->setPixelColor(iLED, RGBOff);}

  //Write defined segment in segment index order
  for (iSegment = 0; iSegment <= segMaxDefinedIndex; iSegment++) {

    segptr = &SegmentData[iSegment];
    Action = segptr->segAction;

    //Process segment if it does something

    if (Action != cSegActionNone) {

      /* Local vars for fast reference */
      partStart =   stripParts[segptr->segPart].start;
      partLen =     stripParts[segptr->segPart].len;
      partEnd =     partStart + partLen - 1;
      partUp =      stripParts[segptr->segPart].partup;
      backColor =   segptr->segBackColor;
      foreColor =   segptr->segForeColor;
      segSpacing1 = segptr->segSpacing + 1;
      segNumLEDs =  segptr->segNumLEDs;
      segRandomPattern = segptr->segRandomPattern;
      
      Options = segptr->segOptions;
      optOffOverwrite = (Options & cSegOptNoOffOverwrite) == 0;
      optModulate = (Options & cSegOptModulateSegment) != 0;
      
      //The value coming out of MapBandsToSegments() is [0...1023]. Now we apply any scaling options...
      //When done, segval will contain the number of LEDs to illuminate for this segment.
      segval = segptr->segLevel;

      //Rescale final value to the number of LEDs that segval means for this segment's length

      segval = (((long) segval) * ((long) (segNumLEDs + 1))) / ((long) (cMaxSegmentLevel + 1));

#if defined DIAGSEGS
      short diagval1, diagval2, diagval3, diagval4, diagval5;
      if ((segval < 0) || (segval > segNumLEDs)) {
        diagval1 = segNumLEDs;
        diagval2 = SegmentData[iSegment].segLevel;
        diagval3 = SegmentData[iSegment].segLevel * (segNumLEDs + 1);
        diagval4 = cMaxSegmentLevel + 1;
        diagval5 = (SegmentData[iSegment].segLevel * (segNumLEDs + 1)) / (cMaxSegmentLevel + 1);
        Serial.print("# LEDs out of range: "); Serial.print(segval); Serial.print(" of "); Serial.print(segNumLEDs);
        Serial.print(" (Diags: "); Serial.print(diagval1);
        Serial.print(", "); Serial.print(diagval2);
        Serial.print(", "); Serial.print(diagval3);
        Serial.print(", "); Serial.print(diagval4);
        Serial.print(", "); Serial.print(diagval5);
        Serial.print(")");
        Serial.println("");
      }
      Serial.print("SegmentLevel["); Serial.print(iSegment); Serial.print("]="); Serial.print(segptr->segLevel); Serial.print(" = ");
      Serial.print(segval); Serial.print(" of "); Serial.print(segNumLEDs); Serial.println(" LEDs");
#endif
      segval = constrain(segval, 0, segNumLEDs); //Safety to keep in expected range

      //If this is a cSegModulateSegment option, then figure the foreground color scaled between
      //backcolor and forecolor according to the segment's spectrum level.

      if (optModulate) {
        Colorvals(backColor, bcRGB);
        Colorvals(foreColor, fcRGB);
        foreColor = LEDSegs::Color(
                      bcRGB[0] + (((fcRGB[0] - bcRGB[0]) * segval) / segNumLEDs)
                      , bcRGB[1] + (((fcRGB[1] - bcRGB[1]) * segval) / segNumLEDs)
                      , bcRGB[2] + (((fcRGB[2] - bcRGB[2]) * segval) / segNumLEDs));
      }

      //Get the starting LED index (segFirstLED) for this segment based on the action. For
      //parts that have a down direction, the start position for the segments is inverted
      //within the part.

      segFirstLED = segptr->segFirstLED + partStart; //default value
      switch (Action) {
        case cSegActionFromBottom:
        case cSegActionRandom:
        case cSegActionBits:
        case cSegActionFromTop:
          if (!partUp) segFirstLED = (partStart + partLen) - (segptr->segFirstLED + segNumLEDs);
          break;
        case cSegActionAll:
        case cSegActionFromMiddle:
          break;
      }

      //Now figure the initial starting LED for the segment, and the initial increment to get to the
      //next LED in sequence (+1, 0, -1)

      LEDIncrement = 1;   //default value
      iLED = segFirstLED; //default value
      switch (Action) {
        case cSegActionFromBottom:
        case cSegActionRandom:
          if (!partUp) {LEDIncrement = -1; iLED = segFirstLED + segNumLEDs - 1;}
          break;
        case cSegActionFromTop:
          if (partUp) {LEDIncrement = -1; iLED = segFirstLED + segNumLEDs - 1;}
          break;
        case cSegActionAll:
          break;
        case cSegActionFromMiddle:
          LEDIncrement = 0;
          iLED = segFirstLED + ((segNumLEDs - 1) >> 1);
          break;
        case cSegActionBits:
          if (!partUp) {LEDIncrement = -1; iLED = segFirstLED + segNumLEDs - 1;}
          bitsary = segptr->segBitsPtr;
          if (bitsary == NULL) bitsary = &zerobits;
          bitscounter = 0;
          break;
      }

#if defined DIAGSEGS
  Serial.print("iLED="); Serial.print(iLED);
  Serial.print(", LEDInc="); Serial.print(LEDIncrement);
  Serial.print(", segFirstLED="); Serial.print(segFirstLED);
  Serial.print(", segNumLEDs="); Serial.print(segNumLEDs);
  Serial.println();
#endif

      //This counts down from spacing-1 each time it hits 0 (an illuminated LED).
      //The first LED is always a non-spacer, which is why we init to zero.

      SpacingCount = 0;

      //Loop across the LEDs in the segment being illuminated.

      for (iLEDinSegment = 0; iLEDinSegment < segNumLEDs; iLEDinSegment++) {

        //If the LED is outside the segment's part range, or a spacing-skipped LED,
        //then we don't process the display for it (this is the "cropping")

        notSpacingLED = (SpacingCount == 0);
        if ((iLED >= partStart) && (iLED <= partEnd) && (notSpacingLED)) {

          //Calculate the color (foreground/background) based on the action type.
          thisColor = backColor;  //Assume background color
          switch (Action) {
            case cSegActionFromBottom:
            case cSegActionFromTop:
            case cSegActionFromMiddle:
              //(Note the ">" is correct. ">=" will give you an always-on first LED).
              if (segval > iLEDinSegment) thisColor = foreColor;
              break;
            case cSegActionAll:
              thisColor = foreColor;
              break;
            case cSegActionRandom:
              if (segRandomLevels[(iLEDinSegment + segRandomPattern) & cSegNRandomMask] <= segptr->segLevel) thisColor = foreColor;
#if defined DIAGRANDOM
  Serial.print("**Random: iLED="); Serial.print(iLEDinSegment);
  Serial.print(", RanLev="); Serial.print(segRandomLevels[(iLEDinSegment + segRandomPattern) & cSegNRandomMask]);
  Serial.print(", SegLev="); Serial.print(segptr->segLevel);
  Serial.print(", Color="); Serial.print(thisColor,HEX);
  Serial.println();
#endif
              break;
            case cSegActionBits:
              if (((*bitsary) >> bitscounter) & 1) thisColor = foreColor;
              bitscounter++;
              if (bitscounter >= 32) {bitscounter = 0; bitsary++;};
              break;
          }

          //Write the LED color, but not if the value is the background color and this is a no-off-overwrite segment.
          if ((thisColor != backColor) || optOffOverwrite) {
            objLPDStrip->setPixelColor(iLED, thisColor);
          }          

        } //Segment LED within part range and not spacer

        //Move to next LED. For from-middle, we jump back and forth around the center of the
        //segment, increasing the increment's absolute value by one more each jump.

        if (Action == cSegActionFromMiddle) {
          if (LEDIncrement <= 0) {
            LEDIncrement--;
            if (notSpacingLED) {
              SpacingCount = segSpacing1;
            }
            SpacingCount--;
          }
          else {
            LEDIncrement++;
          }
          LEDIncrement = -LEDIncrement;
        }
        else {
          if (notSpacingLED) {
            SpacingCount = segSpacing1;
          }
          SpacingCount--;
        }

        iLED += LEDIncrement;
      } //LED-in-segment loop
    } //If an action defined
  }  //Segment loop

  //Finally, refresh the strip.
  objLPDStrip->show();
}

#endif  //_LEDSEGS_cpp
