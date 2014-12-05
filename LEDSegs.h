// LEDSegs.h: header file for LEDSegs library

#ifndef _LEDSEGS_h
#define _LEDSEGS_h

#include <LPD8806.h>

//Storage limits for various things. You can redefine these before including this library.

#ifndef cMaxSegments
#define cMaxSegments 100  //Max number of definable segments
#endif

#ifndef cMaxParts
#define cMaxParts 20 //Max number of definable parts
#endif

#ifndef cMaxTimers
#define cMaxTimers 32 //Max number of definable timers
#endif

//General macros
#define _LEDSEGS_CNT(ary) (sizeof(ary) / sizeof(ary[ 0 ]))


//The number of spectrum analyzer shield bands and max value for a band read.
//DO NOT CHANGE THESE VALUES.

const short cSegNumBands = 7;
const short cMaxSegmentLevel = 1023; //Normalized max sample value out of MapBandsToSegments().

//Frequency band index values bit mask values (for DefineSegment()). Bit 0 (0x1) is the lowest freq.,
//bit 7 (0x40) is highest. These are fixed by the spectrum analyzer chip used on the shield, and the
//number of bands and center frequencies can't be changed. For most applications it's recommended
//not to use bands 1 and 7.

const short cSegBand1 = 0x01;  //63Hz center - I recommend omitting this band in most applications
const short cSegBand2 = 0x02;  //160Hz center
const short cSegBand3 = 0x04;  //400Hz center
const short cSegBand4 = 0x08;  //1.0KHz center
const short cSegBand5 = 0x10;  //2.5KHz center
const short cSegBand6 = 0x20;  //6.25KHz - Think about omitting this (6KHz is a high "audible" freq.)
const short cSegBand7 = 0x40;  //16KHz - I recommend omitting this one, just noise.

//LEDSegs Segment actions. See DefineSegment and SetSegment_Action.

const short cSegActionNone = 0;        //Do nothing (undefined or do-nothing segment)
const short cSegActionFromBottom = 1;  //Fill LEDs from the first LED up, based on current averaged band level vs the max possible
const short cSegActionFromTop = 2;     //Fill LEDs from the last down
const short cSegActionFromMiddle = 3;  //Fill LEDs from the middle out
const short cSegActionAll = 4;         //Fill all LEDs in segment. Do not look at current value.
const short cSegActionStatic = cSegActionAll; //(Legacy)
const short cSegActionRandom = 5;      //Illuminate foreground color LEDs randomly throughout the segment range based on level
const short cSegActionBits = 6;        //Display bits from a long[] array. Uses the BitsPtr value for the segment, which has to be set.

//Segment Options

const short cSegOptNoOffOverwrite =  0x01; //Do not overwrite an LED if value is RGBOff
const short cSegOptModulateSegment = 0x02; //Vary intensity of LEDs based on level
const short cSegOptBandAvg =         0x04; //Scale to the average across all band values, instead of using max

//Software gain control constants. This provides a simple noise gate and 'fast attack'/'slow decay' AGC.
//As each band sample is read, a fixed assumed noise value (defined below) for each band is subtracted out.
//In the MapBandsToSegments processing, the segment's max level seen so far is updated, but not permitted
//to go below cSegMaxLevelMin. Also, the max level is reduced by cSegLevelDecay on each cycle. This is all done
//prior to calling any custom display routine, Use SetSegment_MaxLevel() to change or reset the max level "seen".
//You can also change these noise constants to suit, but I don't recommend reducing them.

const static short cBandNoiseFloor[cSegNumBands] = {90, 90, 90, 90, 100, 100, 120};

//cSegNRandom is the number of randomizer 'slots' that contain distinct random values. It is used for cSegActionRandom
//segments. The LED index plus the pattern number for the segment, modulo cSegNRandom, is used to index the
//array of random values for comparison to determine whether to display that LED. The random pattern repeats every
//cSegNRandom LEDs, but that's enough to not be visible.

const short cSegNRandomMask = 0X3F; //Array count has to be power of 2.
const short cSegNRandom = cSegNRandomMask + 1;

/*
______________
LEDBits Class:
*/

class LEDBits {
public:
  static void BitRotate(short, uint8_t[], short int);
  static bool BitRead(short, uint8_t []);
};

/*
_________________
LEDTimers Class:

(This class is inherited in LEDSegs.)

Timers are defined with an expiration time (relative to current millis()), an optional repeat time in MS,
an action routine to call when the timer expires and an arbirary pointer passed to that routine.
Use CheckTimers() to perform a single pass across the timers looking for active, expired ones.
*/

class LEDTimers {
  public:
    LEDTimers();
    typedef void (*TimerRoutine) (short int, void *);
    unsigned short DefineTimer(unsigned long, unsigned long, TimerRoutine);
    unsigned short DefineTimer(unsigned long, unsigned long, TimerRoutine, void *);
    void CancelTimer(short);
    void CheckTimers();
    unsigned long int GetTimerExpiration(short int);
    void SetTimerExpiration(short int, unsigned long int);
    unsigned long int GetTimerRepeat(short int);
    void SetTimerRepeat(short int, unsigned long int);
    void SetTimerRoutine(short int, TimerRoutine);
    void SetTimerPtr(short int, void *);

  private:

    //A timer element
    struct LEDTimer {
      unsigned long timerExpiration;   //Timer expiration in ms (0=available timer)
      unsigned long timerRepeat;       //If the timer repeats, the # of ms for the repeat (0=no repeat)
      TimerRoutine timerSub;           //A reference to the timer routine to be called on expiration
      void *timerPtr;                  //Arbitrary pointer associated with the timer
    };
    
    //The array of timers. (Index 0 is ignored to keep timer IDs positive.)
    LEDTimer Timers[cMaxTimers];

}; //LEDTimers class

/*
_________________________
LED strip class (LEDSegs::)
*/

class LEDSegs : public LEDTimers, public LEDBits {

  public:
    typedef void (*SegmentDisplayRoutine) (short);
    LEDSegs(short);
    LEDSegs(short, short, short);
    ~LEDSegs();
    void LEDSegsInit(short, bool, short, short);
    short int TimedDisplay(short int);
    void DisplayStrip(bool, bool);
    void ResetRandom();
    void ResetStrip();
    void SetSegmentIndex(short);
    short GetSegmentIndex();
    void SetMaxLevelFloor(short int);
    short int GetMaxLevelFloor();
 	  void SetMaxLevelDecay(short int);
    short int GetMaxLevelDecay();
    
    void SetSegment_Action(short, short);
    void SetSegment_Action(short);
    void SetSegment_BackColor(short, uint32_t);
    void SetSegment_BackColor(uint32_t);
    void SetSegment_Bands(short, short);
    void SetSegment_Bands(short);
    void SetSegment_DisplayRoutine(short, SegmentDisplayRoutine);
    void SetSegment_DisplayRoutine(SegmentDisplayRoutine);
    void SetSegment_FirstLED(short, short);
    void SetSegment_FirstLED(short);
    void SetSegment_ForeColor(short, uint32_t);
    void SetSegment_ForeColor(uint32_t);
    void SetSegment_Level(short, short);
    void SetSegment_Level(short);
    void SetSegment_MaxLevel(short);
    void SetSegment_MaxLevel(short, short);
    void SetSegment_NumLEDs(short, short);
    void SetSegment_NumLEDs(short);
    void SetSegment_Part(short, short);
    void SetSegment_Part(short);
    void SetSegment_BitsPtr(short, uint32_t *);
    void SetSegment_BitsPtr(uint32_t *);
    void SetSegment_Options(short, short);
    void SetSegment_Options(short);
    void SetSegment_Persistence(short, short);
    void SetSegment_Persistence(short, short, short);
    void SetSegment_RandomPattern(short, short);
    void SetSegment_RandomPattern(short);
    void SetSegment_Spacing(short, short);
    void SetSegment_Rescale(const short int *);
    void SetSegment_Rescale(short, const short int *);
    void SetSegment_Spacing(short);

    short    GetSegment_Action(short);
    short    GetSegment_Action();
    uint32_t GetSegment_BackColor(short);
    uint32_t GetSegment_BackColor();
    short    GetSegment_Bands(short);
    short    GetSegment_Bands();
    short    GetSegment_FirstLED(short);
    short    GetSegment_FirstLED();
    uint32_t GetSegment_ForeColor(short);
    uint32_t GetSegment_ForeColor();
    short    GetSegment_Level(short);
    short    GetSegment_Level();
    short    GetSegment_MaxLevel(short);
    short    GetSegment_MaxLevel();
    short    GetSegment_NumLEDs(short);
    short    GetSegment_NumLEDs();
    short    GetSegment_Options(short);
    short    GetSegment_Options();
    short    GetSegment_RandomPattern(short);
    short    GetSegment_RandomPattern();
    short    GetSegment_Spacing(short);
    short    GetSegment_Spacing();

    short DefineSegment(short, short, short, uint32_t, short);
    short DefineSegment(short, short, short, uint32_t, short, short);
    void ResetSegment(short int);
    void ResetSegments();
    static uint32_t Color(byte, byte, byte);
    static void Colorvals(uint32_t, byte []);
    
    /* Parts methods (public) */

    void DefinePart(short, short partStart, short partLen, bool partUp);
    short GetPart_Start(short);
    void  SetPart_Start(short, short);
    short GetPart_Len(short);
    void  SetPart_Len(short, short);
    bool GetPart_Up(short);
    void SetPart_Up(short, bool);

    bool CheckForDeadAir(short);
    void DisableDeadAirDetect();
    void EnableDeadAirDetect(short int);
    
  private:

    const static short cSpectrumReset = 5;
    const static short cSpectrumStrobe = 4;

    short int stripMaxLevelFloor, stripMaxLevelDecay;

    //Called by TimedDisplay() timer routine on expiration
    static void teTimedDisplay(short int, void *);

    struct stripSegment {
      SegmentDisplayRoutine segDisplayRoutine;  //Optional routine to call just before each display cycle
      const short int *segRescaleAry; //Level rescaling array (optional)
      uint32_t segForeColor;  //The base color of the segment's illuminated LEDs
      uint32_t segBackColor;  //Background color for un-illuminated LEDs
      uint32_t *segBitsPtr;   //Pointer to 32-bit unsigned long for Bits action. (Can go past 32-bits if the segment is longer)
      short segFirstLED;      //The first LED in the segment from the beginning (0-origin)
      short segNumLEDs;       //The number of LEDs in the segment
      short segBands;         //The spectrum bands that are averaged together to make up the sample value for this segment
      short segAction;        //The way the LEDs in the segment are populated (cSegActionXXX)
      short segSpacing;       //Spacing between LEDs that are illuminated in the segment (0 default = no added spacing)
      short segOptions;       //Options for the segment (cSegOptXXX)
      short segLevel;         //Normalized, averaged level for this segment's bands
      short segMaxLevel;      //Normalized, max level for this segment's bands
      short segPart;          //The part index associated with the segment (default is part 0 = the whole strip)
      short segRandomPattern; //A randomization index [0..63], for cSegActionRandom. Default=0.
      short segPersistUp;     //Weighting of prior level when this level is higher than prior
      short segPersistDown;   //Weighting of prior level when current level is less than prior
    };

    //The array of strip part definitions

    struct Parts {
      short start;
      short len;
      bool  partup;
    };
    Parts stripParts[cMaxParts];

    //The actual segments
    short segCurrentIndex;    //The "current" (default) index that will be modified
    short segMaxDefinedIndex; //Tracks the highest index defined
    stripSegment SegmentData[cMaxSegments];  //The segment array

    //The per-band level from the spectrum analyzer for the current sample (see ::ReadSpectrum)
    //The max is private for the dead air detection
    short SpectrumLevel[cSegNumBands];
    short SpectrumMax[cSegNumBands];

    //Spectrum analyzer left/right channels
    const static short cSegSpectrumAnalogLeft = 0; //Left channel
    const static short cSegSpectrumAnalogRight = 1; //Right channel

    //The sampling/segment processing routines
    void ReadSpectrum(bool, bool);
    void MapBandsToSegments();
    void ShowSegments();

    //Private reset routines

    //Initialize the parts array (all parts = entire strip with up order)
    void ResetParts();

    //A pointer to the low-level I/O LBD8806 strip object we talk to
    LPD8806* objLPDStrip;
    short nLEDsInStrip;

    //Array of random cutoff levels (for cSegActionRandom)
    unsigned short segRandomLevels[cSegNRandom];

    //Dead air detection methods
    short int DeadAirLevel, DeadAirDetectTimerID;
    uint32_t DeadAirSecondsCount;

    //Called on dead air timer expiration every second. We sum selected bands' maxes to check for signal.
    //ptr is the timer pointer, which is set to the "this" pointer for the segment class instance.
    static void teCheckForDeadAir(short, void *);
}; //LEDSegs class

//Various colors. The bit format of these is defined by the LPD8806 library.
//Assume nothing about the format except they are an unsigned long int and 0..127

const uint32_t RGBOff =         LEDSegs::Color(0, 0, 0);
const uint32_t RGBBlack =       RGBOff;

const uint32_t RGBWhite =       LEDSegs::Color(127, 127, 127); //Watch out - 60mA per LED with this
const uint32_t RGBGold =        LEDSegs::Color(110,  15,   7);
const uint32_t RGBSilver =      LEDSegs::Color( 15,  30,  60);
const uint32_t RGBYellow =      LEDSegs::Color( 90,  70,   0);
const uint32_t RGBOrange =      LEDSegs::Color( 80,  20,   0);
const uint32_t RGBRed =         LEDSegs::Color(127,   0,   0);
const uint32_t RGBGreen =       LEDSegs::Color(  0, 127,   0);
const uint32_t RGBCyan =        LEDSegs::Color(  0,  73,  43);
const uint32_t RGBBlue =        LEDSegs::Color(  0,   0, 127);
const uint32_t RGBPurple =      LEDSegs::Color( 40,   0,  40);

//White-ish versions of the base colors (except white!)

const uint32_t RGBGoldWhite =   LEDSegs::Color(110,  70,  30);
const uint32_t RGBSilverWhite = LEDSegs::Color( 20,  45,  90);
const uint32_t RGBYellowWhite = LEDSegs::Color(127, 100,  15);
const uint32_t RGBOrangeWhite = LEDSegs::Color( 80,  35,   5);
const uint32_t RGBRedWhite =    LEDSegs::Color(100,   3,   5); //aka "Pink"
const uint32_t RGBGreenWhite =  LEDSegs::Color( 20, 127,  20);
const uint32_t RGBCyanWhite =   LEDSegs::Color( 20,  63,  63);
const uint32_t RGBBlueWhite =   LEDSegs::Color( 10,  20, 127);
const uint32_t RGBPurpleWhite = LEDSegs::Color( 40,   8,  40);

//Dimmer versions of the base colors
const uint32_t RGBWhiteDim =       LEDSegs::Color(12, 15, 15);
const uint32_t RGBGoldDim =        LEDSegs::Color(12,  3,  1);
const uint32_t RGBSilverDim =      LEDSegs::Color( 8, 15, 24);
const uint32_t RGBYellowDim =      LEDSegs::Color(15, 12,  0);
const uint32_t RGBOrangeDim =      LEDSegs::Color(15,  3,  0);
const uint32_t RGBRedDim =         LEDSegs::Color(20,  0,  0);
const uint32_t RGBGreenDim =       LEDSegs::Color( 0,  6,  0);
const uint32_t RGBCyanDim =        LEDSegs::Color( 0,  6,  6);
const uint32_t RGBBlueDim =        LEDSegs::Color( 0,  0, 24);
const uint32_t RGBPurpleDim =      LEDSegs::Color(10,  0, 10);

//Very dim versions of the colors
const uint32_t RGBWhiteVeryDim =   LEDSegs::Color( 1,  2,  2);
const uint32_t RGBGoldVeryDim =    LEDSegs::Color( 4,  2,  1);
const uint32_t RGBSilverVeryDim =  LEDSegs::Color( 1,  2,  4);
const uint32_t RGBYellowVeryDim =  LEDSegs::Color( 4,  3,  0);
const uint32_t RGBOrangeVeryDim =  LEDSegs::Color( 4,  1,  0);
const uint32_t RGBRedVeryDim =     LEDSegs::Color( 1,  0,  0);
const uint32_t RGBGreenVeryDim =   LEDSegs::Color( 0,  1,  0);
const uint32_t RGBCyanVeryDim =    LEDSegs::Color( 0,  2,  2);
const uint32_t RGBBlueVeryDim =    LEDSegs::Color( 0,  0,  1);
const uint32_t RGBPurpleVeryDim =  LEDSegs::Color( 1,  0,  1);

#endif //_LEDSEGS_h
