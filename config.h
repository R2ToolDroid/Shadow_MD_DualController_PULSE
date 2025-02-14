#include <Arduino.h>     

// =======================================================================================
//        SHADOW_MD:  Small Handheld Arduino Droid Operating Wand + MarcDuino
// =======================================================================================
//                          Last Revised Date: 08/23/2015
//                             Revised By: vint43
//                Inspired by the PADAWAN / KnightShade SHADOW effort
// =======================================================================================
//
//         This program is free software: you can redistribute it and/or modify it for
//         your personal use and the personal use of other astromech club members.  
//
//         This program is distributed in the hope that it will be useful 
//         as a courtesy to fellow astromech club members wanting to develop
//         their own droid control system.
//
//         IT IS OFFERED WITHOUT ANY WARRANTY; without even the implied warranty of
//         MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//         You are using this software at your own risk and, as a fellow club member, it is
//         expected you will have the proper experience / background to handle and manage that 
//         risk appropriately.  It is completely up to you to insure the safe operation of
//         your droid and to validate and test all aspects of your droid control system.
//
// =======================================================================================
//   Note: You will need a Arduino Mega ADK rev3 to run this sketch,
//   as a normal Arduino (Uno, Duemilanove etc.) doesn't have enough SRAM and FLASH
//
//   This is written to be a SPECIFIC Sketch - supporting only one type of controller
//      - PS3 Move Navigation + MarcDuino Dome Controller & Optional Body Panel Controller
//
//   PS3 Bluetooth library - developed by Kristian Lauszus (kristianl@tkjelectronics.com)
//   For more information visit my blog: http://blog.tkjelectronics.dk/ or
//
//   Sabertooth (Foot Drive):
//         Set Sabertooth 2x32 or 2x25 Dip Switches: 1 and 2 Down, All Others Up
//
//   SyRen 10 Dome Drive:
//         For SyRen packetized Serial Set Switches: 1, 2 and 4 Down, All Others Up
//         NOTE:  Support for SyRen Simple Serial has been removed, due to problems.
//         Please contact DimensionEngineering to get an RMA to flash your firmware
//         Some place a 10K ohm resistor between S1 & GND on the SyRen 10 itself
//
// =======================================================================================
//  These libraries consist of the PS3BT and PS3USB. These libraries allows you to use a Dualshock 3, Navigation or a
//  Motion controller with the USB Host Shield both via Bluetooth and USB.
//  In order to use your Playstation controller via Bluetooth you have to set the Bluetooth address of the dongle 
//  internally to your PS3 Controller. This can be achieved by first plugging in the Bluetooth dongle and wait a few seconds. 
//  Now plug in the controller via USB and wait until the LEDs start to flash. 
//  The library has now written the Bluetooth address of the dongle to the PS3 controller.
//  
//  Finally simply plug in the Bluetooth dongle again and press PS on the PS3 controller. 
//  After a few seconds it should be connected to the dongle and ready to use.
//  Note: You will have to plug in the Bluetooth dongle before connecting the controller, as the library needs to read the address of the dongle.
//  Alternatively you could set it in code like so: PS3BT.ino::L20.
//  For more information about the PS3 protocol see the official wiki: https://github.com/felis/USB_Host_Shield_2.0/wiki/PS3-Information.
// ---------------------------------------------------------------------------------------
//                        General User Settings
// ---------------------------------------------------------------------------------------

String PS3ControllerFootMac = "00:07:04:03:C9:5E";  //Set this to your FOOT PS3 controller MAC address
String PS3ControllerDomeMAC = "XX:XX:XX:XX:XX:XX";  //Set to a secondary DOME PS3 controller MAC address (Optional)

String PS3ControllerBackupFootMac = "XX";  //Set to the MAC Address of your BACKUP FOOT controller (Optional)
String PS3ControllerBackupDomeMAC = "XX";  //Set to the MAC Address of your BACKUP DOME controller (Optional)

String cmd ;/// Serial Data from Input Terminal Serial2 MASTER BODY CONTROLLER

int mode = 3; // Default Mode  0 = Input Coin Shadow CPU NEXTION Sound RAND Dome RAND
              // RC Mode       1 = Input RC
              // RC Show       2 = HUMAN
              //               3 = SERVICE

byte drivespeed1 = 70;   //For Speed Setting (Normal): set this to whatever speeds works for you. 0-stop, 127-full speed.
byte drivespeed2 = 110;  //For Speed Setting (Over Throttle): set this for when needing extra power. 0-stop, 127-full speed.

byte turnspeed = 50;      // the higher this number the faster it will spin in place, lower - the easier to control.
                         // Recommend beginner: 40 to 50, experienced: 50+, I like 75

byte domespeed = 100;    // If using a speed controller for the dome, sets the top speed
                         // Use a number up to 127

byte ramping = 1;        // Ramping- the lower this number the longer R2 will take to speedup or slow down,
                         // change this by increments of 1

                         

byte joystickFootDeadZoneRange = 15;  // For controllers that centering problems, use the lowest number with no drift
byte joystickDomeDeadZoneRange = 10;  // For controllers that centering problems, use the lowest number with no drift

byte driveDeadBandRange = 10;     // Used to set the Sabertooth DeadZone for foot motors

int invertTurnDirection = -1;   //This may need to be set to 1 for some configurations

byte domeAutoSpeed = 70;     // Speed used when dome automation is active - Valid Values: 50 - 100
int time360DomeTurn = 2500;  // milliseconds for dome to complete 360 turn at domeAutoSpeed - Valid Values: 2000 - 8000 (2000 = 2 seconds)

#define SHADOW_DEBUG       //uncomment this for console DEBUG output
#define SHADOW_VERBOSE     //uncomment this for console VERBOSE output

// ---------------------------------------------------------------------------------------
//                          MarcDuino Button Settings
// ---------------------------------------------------------------------------------------
// Std MarcDuino Function Codes:
//     1 = Close All Panels
//     2 = Scream - all panels open
//     3 = Wave, One Panel at a time
//     4 = Fast (smirk) back and forth wave
//     5 = Wave 2, Open progressively all panels, then close one by one
//     6 = Beep cantina - w/ marching ants panel action
//     7 = Faint / Short Circuit
//     8 = Cantina Dance - orchestral, rhythmic panel dance
//     9 = Leia message
//    10 = Disco
//    11 = Quite mode reset (panel close, stop holos, stop sounds)
//    12 = Full Awake mode reset (panel close, rnd sound, holo move,holo lights off)
//    13 = Mid Awake mode reset (panel close, rnd sound, stop holos)
//    14 = Full Awake+ reset (panel close, rnd sound, holo move, holo lights on)
//    15 = Scream, with all panels open (NO SOUND)
//    16 = Wave, one panel at a time (NO SOUND)
//    17 = Fast (smirk) back and forth (NO SOUND)
//    18 = Wave 2 (Open progressively, then close one by one) (NO SOUND)
//    19 = Marching Ants (NO SOUND)
//    20 = Faint/Short Circuit (NO SOUND)
//    21 = Rhythmic cantina dance (NO SOUND)
//    22 = Random Holo Movement On (All) - No other actions
//    23 = Holo Lights On (All)
//    24 = Holo Lights Off (All)
//    25 = Holo reset (motion off, lights off)
//    26 = Volume Up
//    27 = Volume Down
//    28 = Volume Max
//    29 = Volume Mid
//    30 = Open All Dome Panels
//    31 = Open Top Dome Panels
//    32 = Open Bottom Dome Panels
//    33 = Close All Dome Panels
//    34 = Open Dome Panel #1
//    35 = Close Dome Panel #1
//    36 = Open Dome Panel #2
//    37 = Close Dome Panel #2
//    38 = Open Dome Panel #3
//    39 = Close Dome Panel #3
//    40 = Open Dome Panel #4
//    41 = Close Dome Panel #4
//    42 = Open Dome Panel #5
//    43 = Close Dome Panel #5
//    44 = Open Dome Panel #6
//    45 = Close Dome Panel #6
//    46 = Open Dome Panel #7
//    47 = Close Dome Panel #7
//    48 = Open Dome Panel #8
//    49 = Close Dome Panel #8
//    50 = Open Dome Panel #9
//    51 = Close Dome Panel #9
//    52 = Open Dome Panel #10
//    53 = Close Dome Panel #10
//   *** BODY PANEL OPTIONS ASSUME SECOND MARCDUINO MASTER BOARD ON MEGA ADK SERIAL #3 ***
//    54 = Open All Body Panels
//    55 = Close All Body Panels
//    56 = Open Body Panel #1
//    57 = Close Body Panel #1
//    58 = Open Body Panel #2
//    59 = Close Body Panel #2
//    60 = Open Body Panel #3
//    61 = Close Body Panel #3
//    62 = Open Body Panel #4
//    63 = Close Body Panel #4
//    64 = Open Body Panel #5
//    65 = Close Body Panel #5
//    66 = Open Body Panel #6
//    67 = Close Body Panel #6
//    68 = Open Body Panel #7
//    69 = Close Body Panel #7
//    70 = Open Body Panel #8
//    71 = Close Body Panel #8
//    72 = Open Body Panel #9
//    73 = Close Body Panel #9
//    74 = Open Body Panel #10
//    75 = Close Body Panel #10
//   *** MAGIC PANEL LIGHTING COMMANDS
//    76 = Magic Panel ON
//    77 = Magic Panel OFF
//    78 = Magic Panel Flicker (10 seconds) 
//
// Std MarcDuino Logic Display Functions (For custom functions)
//     1 = Display normal random sequence
//     2 = Short circuit (10 second display sequence)
//     3 = Scream (flashing light display sequence)
//     4 = Leia (34 second light sequence)
//     5 = Display “Star Wars”
//     6 = March light sequence
//     7 = Spectrum, bar graph display sequence
//     8 = Display custom text
//
// Std MarcDuino Panel Functions (For custom functions)
//     1 = Panels stay closed (normal position)
//     2 = Scream sequence, all panels open
//     3 = Wave panel sequence
//     4 = Fast (smirk) back and forth panel sequence
//     5 = Wave 2 panel sequence, open progressively all panels, then close one by one)
//     6 = Marching ants panel sequence
//     7 = Faint / short circuit panel sequence
//     8 = Rhythmic cantina panel sequence
//     9 = Custom Panel Sequence


//----------------------------------------------------
// CONFIGURE: The FOOT Navigation Controller Buttons
//----------------------------------------------------

//---------------------------------
// CONFIGURE: Arrow Up
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnUP_MD_func = 12;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnUP_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_use_DP1 = false;
int btnUP_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_use_DP2 = false;
int btnUP_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_use_DP3 = false;
int btnUP_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_use_DP4 = false;
int btnUP_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_use_DP5 = false;
int btnUP_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_use_DP6 = false;
int btnUP_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_use_DP7 = false;
int btnUP_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_use_DP8 = false;
int btnUP_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_use_DP9 = false;
int btnUP_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_use_DP10 = false;
int btnUP_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnLeft_MD_func = 13;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnLeft_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_use_DP1 = false;
int btnLeft_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_use_DP2 = false;
int btnLeft_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_use_DP3 = false;
int btnLeft_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_use_DP4 = false;
int btnLeft_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_use_DP5 = false;
int btnLeft_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_use_DP6 = false;
int btnLeft_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_use_DP7 = false;
int btnLeft_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_use_DP8 = false;
int btnLeft_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_use_DP9 = false;
int btnLeft_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_use_DP10 = false;
int btnLeft_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnRight_MD_func = 14;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnRight_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_use_DP1 = false;
int btnRight_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_use_DP2 = false;
int btnRight_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_use_DP3 = false;
int btnRight_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_use_DP4 = false;
int btnRight_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_use_DP5 = false;
int btnRight_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_use_DP6 = false;
int btnRight_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_use_DP7 = false;
int btnRight_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_use_DP8 = false;
int btnRight_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_use_DP9 = false;
int btnRight_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_use_DP10 = false;
int btnRight_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnDown_MD_func = 11;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnDown_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_use_DP1 = false;
int btnDown_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_use_DP2 = false;
int btnDown_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_use_DP3 = false;
int btnDown_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_use_DP4 = false;
int btnDown_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_use_DP5 = false;
int btnDown_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_use_DP6 = false;
int btnDown_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_use_DP7 = false;
int btnDown_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_use_DP8 = false;
int btnDown_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_use_DP9 = false;
int btnDown_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_use_DP10 = false;
int btnDown_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + CROSS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_CROSS_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnUP_CROSS_MD_func = 26;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnUP_CROSS_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_CROSS_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_CROSS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_CROSS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_CROSS_use_DP1 = false;
int btnUP_CROSS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_CROSS_use_DP2 = false;
int btnUP_CROSS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_CROSS_use_DP3 = false;
int btnUP_CROSS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_CROSS_use_DP4 = false;
int btnUP_CROSS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_CROSS_use_DP5 = false;
int btnUP_CROSS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_CROSS_use_DP6 = false;
int btnUP_CROSS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_CROSS_use_DP7 = false;
int btnUP_CROSS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_CROSS_use_DP8 = false;
int btnUP_CROSS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_CROSS_use_DP9 = false;
int btnUP_CROSS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_CROSS_use_DP10 = false;
int btnUP_CROSS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CROSS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + CROSS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_CROSS_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnLeft_CROSS_MD_func = 23;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnLeft_CROSS_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_CROSS_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_CROSS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_CROSS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_CROSS_use_DP1 = false;
int btnLeft_CROSS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_CROSS_use_DP2 = false;
int btnLeft_CROSS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_CROSS_use_DP3 = false;
int btnLeft_CROSS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_CROSS_use_DP4 = false;
int btnLeft_CROSS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_CROSS_use_DP5 = false;
int btnLeft_CROSS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_CROSS_use_DP6 = false;
int btnLeft_CROSS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_CROSS_use_DP7 = false;
int btnLeft_CROSS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_CROSS_use_DP8 = false;
int btnLeft_CROSS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_CROSS_use_DP9 = false;
int btnLeft_CROSS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_CROSS_use_DP10 = false;
int btnLeft_CROSS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CROSS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + CROSS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_CROSS_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnRight_CROSS_MD_func = 24;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnRight_CROSS_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_CROSS_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_CROSS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_CROSS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_CROSS_use_DP1 = false;
int btnRight_CROSS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_CROSS_use_DP2 = false;
int btnRight_CROSS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_CROSS_use_DP3 = false;
int btnRight_CROSS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_CROSS_use_DP4 = false;
int btnRight_CROSS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_CROSS_use_DP5 = false;
int btnRight_CROSS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_CROSS_use_DP6 = false;
int btnRight_CROSS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_CROSS_use_DP7 = false;
int btnRight_CROSS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_CROSS_use_DP8 = false;
int btnRight_CROSS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_CROSS_use_DP9 = false;
int btnRight_CROSS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_CROSS_use_DP10 = false;
int btnRight_CROSS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CROSS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + CROSS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_CROSS_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnDown_CROSS_MD_func = 27;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnDown_CROSS_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_CROSS_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_CROSS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_CROSS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_CROSS_use_DP1 = false;
int btnDown_CROSS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_CROSS_use_DP2 = false;
int btnDown_CROSS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_CROSS_use_DP3 = false;
int btnDown_CROSS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_CROSS_use_DP4 = false;
int btnDown_CROSS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_CROSS_use_DP5 = false;
int btnDown_CROSS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_CROSS_use_DP6 = false;
int btnDown_CROSS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_CROSS_use_DP7 = false;
int btnDown_CROSS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_CROSS_use_DP8 = false;
int btnDown_CROSS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_CROSS_use_DP9 = false;
int btnDown_CROSS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_CROSS_use_DP10 = false;
int btnDown_CROSS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CROSS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_CIRCLE_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnUP_CIRCLE_MD_func = 2;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnUP_CIRCLE_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_CIRCLE_use_DP1 = false;
int btnUP_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_CIRCLE_use_DP2 = false;
int btnUP_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_CIRCLE_use_DP3 = false;
int btnUP_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_CIRCLE_use_DP4 = false;
int btnUP_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_CIRCLE_use_DP5 = false;
int btnUP_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_CIRCLE_use_DP6 = false;
int btnUP_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_CIRCLE_use_DP7 = false;
int btnUP_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_CIRCLE_use_DP8 = false;
int btnUP_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_CIRCLE_use_DP9 = false;
int btnUP_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_CIRCLE_use_DP10 = false;
int btnUP_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_CIRCLE_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnLeft_CIRCLE_MD_func = 4;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnLeft_CIRCLE_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_CIRCLE_use_DP1 = false;
int btnLeft_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_CIRCLE_use_DP2 = false;
int btnLeft_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_CIRCLE_use_DP3 = false;
int btnLeft_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_CIRCLE_use_DP4 = false;
int btnLeft_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_CIRCLE_use_DP5 = false;
int btnLeft_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_CIRCLE_use_DP6 = false;
int btnLeft_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_CIRCLE_use_DP7 = false;
int btnLeft_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_CIRCLE_use_DP8 = false;
int btnLeft_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_CIRCLE_use_DP9 = false;
int btnLeft_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_CIRCLE_use_DP10 = false;
int btnLeft_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_CIRCLE_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnRight_CIRCLE_MD_func = 7;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnRight_CIRCLE_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_CIRCLE_use_DP1 = false;
int btnRight_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_CIRCLE_use_DP2 = false;
int btnRight_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_CIRCLE_use_DP3 = false;
int btnRight_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_CIRCLE_use_DP4 = false;
int btnRight_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_CIRCLE_use_DP5 = false;
int btnRight_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_CIRCLE_use_DP6 = false;
int btnRight_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_CIRCLE_use_DP7 = false;
int btnRight_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_CIRCLE_use_DP8 = false;
int btnRight_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_CIRCLE_use_DP9 = false;
int btnRight_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_CIRCLE_use_DP10 = false;
int btnRight_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + CIRCLE
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_CIRCLE_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnDown_CIRCLE_MD_func = 10;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnDown_CIRCLE_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_CIRCLE_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_CIRCLE_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_CIRCLE_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_CIRCLE_use_DP1 = false;
int btnDown_CIRCLE_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_CIRCLE_use_DP2 = false;
int btnDown_CIRCLE_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_CIRCLE_use_DP3 = false;
int btnDown_CIRCLE_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_CIRCLE_use_DP4 = false;
int btnDown_CIRCLE_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_CIRCLE_use_DP5 = false;
int btnDown_CIRCLE_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_CIRCLE_use_DP6 = false;
int btnDown_CIRCLE_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_CIRCLE_use_DP7 = false;
int btnDown_CIRCLE_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_CIRCLE_use_DP8 = false;
int btnDown_CIRCLE_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_CIRCLE_use_DP9 = false;
int btnDown_CIRCLE_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_CIRCLE_use_DP10 = false;
int btnDown_CIRCLE_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_CIRCLE_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow UP + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_PS_type = 2;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnUP_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnUP_PS_cust_MP3_num = 183;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_PS_cust_LD_type = 5;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_PS_use_DP1 = false;
int btnUP_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_PS_use_DP2 = false;
int btnUP_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_PS_use_DP3 = false;
int btnUP_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_PS_use_DP4 = false;
int btnUP_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_PS_use_DP5 = false;
int btnUP_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_PS_use_DP6 = false;
int btnUP_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_PS_use_DP7 = false;
int btnUP_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_PS_use_DP8 = false;
int btnUP_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_PS_use_DP9 = false;
int btnUP_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_PS_use_DP10 = false;
int btnUP_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_PS_type = 2;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnLeft_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnLeft_PS_cust_MP3_num = 186;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_PS_cust_LD_type = 1;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_PS_use_DP1 = false;
int btnLeft_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_PS_use_DP2 = false;
int btnLeft_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_PS_use_DP3 = false;
int btnLeft_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_PS_use_DP4 = false;
int btnLeft_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_PS_use_DP5 = false;
int btnLeft_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_PS_use_DP6 = false;
int btnLeft_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_PS_use_DP7 = false;
int btnLeft_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_PS_use_DP8 = false;
int btnLeft_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_PS_use_DP9 = false;
int btnLeft_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_PS_use_DP10 = false;
int btnLeft_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_PS_type = 2;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnRight_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnRight_PS_cust_MP3_num = 185;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_PS_cust_LD_type = 1;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_PS_use_DP1 = false;
int btnRight_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_PS_use_DP2 = false;
int btnRight_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_PS_use_DP3 = false;
int btnRight_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_PS_use_DP4 = false;
int btnRight_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_PS_use_DP5 = false;
int btnRight_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_PS_use_DP6 = false;
int btnRight_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_PS_use_DP7 = false;
int btnRight_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_PS_use_DP8 = false;
int btnRight_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_PS_use_DP9 = false;
int btnRight_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_PS_use_DP10 = false;
int btnRight_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + PS
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_PS_type = 2;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnDown_PS_MD_func = 0;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnDown_PS_cust_MP3_num = 184;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_PS_cust_LD_type = 1;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_PS_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_PS_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_PS_use_DP1 = false;
int btnDown_PS_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_PS_use_DP2 = false;
int btnDown_PS_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_PS_use_DP3 = false;
int btnDown_PS_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_PS_use_DP4 = false;
int btnDown_PS_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_PS_use_DP5 = false;
int btnDown_PS_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_PS_use_DP6 = false;
int btnDown_PS_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_PS_use_DP7 = false;
int btnDown_PS_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_PS_use_DP8 = false;
int btnDown_PS_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_PS_use_DP9 = false;
int btnDown_PS_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_PS_use_DP10 = false;
int btnDown_PS_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_PS_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Up + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnUP_L1_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnUP_L1_MD_func = 8;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnUP_L1_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnUP_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnUP_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnUP_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnUP_L1_use_DP1 = false;
int btnUP_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnUP_L1_use_DP2 = false;
int btnUP_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnUP_L1_use_DP3 = false;
int btnUP_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnUP_L1_use_DP4 = false;
int btnUP_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnUP_L1_use_DP5 = false;
int btnUP_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnUP_L1_use_DP6 = false;
int btnUP_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnUP_L1_use_DP7 = false;
int btnUP_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnUP_L1_use_DP8 = false;
int btnUP_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnUP_L1_use_DP9 = false;
int btnUP_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnUP_L1_use_DP10 = false;
int btnUP_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnUP_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Left + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnLeft_L1_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnLeft_L1_MD_func = 3;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnLeft_L1_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnLeft_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnLeft_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnLeft_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnLeft_L1_use_DP1 = false;
int btnLeft_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnLeft_L1_use_DP2 = false;
int btnLeft_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnLeft_L1_use_DP3 = false;
int btnLeft_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnLeft_L1_use_DP4 = false;
int btnLeft_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnLeft_L1_use_DP5 = false;
int btnLeft_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnLeft_L1_use_DP6 = false;
int btnLeft_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnLeft_L1_use_DP7 = false;
int btnLeft_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnLeft_L1_use_DP8 = false;
int btnLeft_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnLeft_L1_use_DP9 = false;
int btnLeft_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnLeft_L1_use_DP10 = false;
int btnLeft_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnLeft_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Right + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnRight_L1_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnRight_L1_MD_func = 5;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnRight_L1_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnRight_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnRight_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnRight_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnRight_L1_use_DP1 = false;
int btnRight_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnRight_L1_use_DP2 = false;
int btnRight_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnRight_L1_use_DP3 = false;
int btnRight_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnRight_L1_use_DP4 = false;
int btnRight_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnRight_L1_use_DP5 = false;
int btnRight_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnRight_L1_use_DP6 = false;
int btnRight_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnRight_L1_use_DP7 = false;
int btnRight_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnRight_L1_use_DP8 = false;
int btnRight_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnRight_L1_use_DP9 = false;
int btnRight_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnRight_L1_use_DP10 = false;
int btnRight_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnRight_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//---------------------------------
// CONFIGURE: Arrow Down + L1
//---------------------------------
//1 = Std MarcDuino Function, 2 = Custom Function
int btnDown_L1_type = 1;    

// IF Std MarcDuino Function (type=1) 
// Enter MarcDuino Function Code (1 - 78) (See Above)
int btnDown_L1_MD_func = 9;

// IF Custom Function (type=2)
// CUSTOM SOUND SETTING: Enter the file # prefix on the MP3 trigger card of the sound to play (0 = NO SOUND)
// Valid values: 0 or 182 - 200  
int btnDown_L1_cust_MP3_num = 0;  

// CUSTOM LOGIC DISPLAY SETTING: Pick from the Std MD Logic Display Functions (See Above)
// Valid values: 0, 1 to 8  (0 - Not used)
int btnDown_L1_cust_LD_type = 0;

// IF Custom Logic Display = 8 (custom text), enter custom display text here
String btnDown_L1_cust_LD_text = "";

// CUSTOM PANEL SETTING: Pick from the Std MD Panel Functions or Custom (See Above)
// Valid Values: 0, 1 to 9 (0 = Not used)
int btnDown_L1_cust_panel = 0;

// IF Custom Panel Setting = 9 (custom panel sequence)
// Dome Panel #1
boolean btnDown_L1_use_DP1 = false;
int btnDown_L1_DP1_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP1_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #2
boolean btnDown_L1_use_DP2 = false;
int btnDown_L1_DP2_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP2_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #3
boolean btnDown_L1_use_DP3 = false;
int btnDown_L1_DP3_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP3_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #4
boolean btnDown_L1_use_DP4 = false;
int btnDown_L1_DP4_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP4_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #5
boolean btnDown_L1_use_DP5 = false;
int btnDown_L1_DP5_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP5_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #6
boolean btnDown_L1_use_DP6 = false;
int btnDown_L1_DP6_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP6_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #7
boolean btnDown_L1_use_DP7 = false;
int btnDown_L1_DP7_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP7_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #8
boolean btnDown_L1_use_DP8 = false;
int btnDown_L1_DP8_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP8_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #9
boolean btnDown_L1_use_DP9 = false;
int btnDown_L1_DP9_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP9_stay_open_time = 5; // in seconds (1 to 30)
// Dome Panel #10
boolean btnDown_L1_use_DP10 = false;
int btnDown_L1_DP10_open_start_delay = 1; // in seconds (0 to 30)
int btnDown_L1_DP10_stay_open_time = 5; // in seconds (1 to 30)

//----------------------------------------------------
// CONFIGURE: The DOME Navigation Controller Buttons
//----------------------------------------------------

//---------------------------------
// CONFIGURE: Arrow Up
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnUP_MD_func = 58;

//---------------------------------
// CONFIGURE: Arrow Left
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnLeft_MD_func = 56;

//---------------------------------
// CONFIGURE: Arrow Right
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnRight_MD_func = 57;

//---------------------------------
// CONFIGURE: Arrow Down
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnDown_MD_func = 59;

//---------------------------------
// CONFIGURE: Arrow UP + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnUP_CROSS_MD_func = 28;

//---------------------------------
// CONFIGURE: Arrow Left + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnLeft_CROSS_MD_func = 33;

//---------------------------------
// CONFIGURE: Arrow Right + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnRight_CROSS_MD_func = 30;

//---------------------------------
// CONFIGURE: Arrow Down + CROSS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnDown_CROSS_MD_func = 29;

//---------------------------------
// CONFIGURE: Arrow UP + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnUP_CIRCLE_MD_func = 22;

//---------------------------------
// CONFIGURE: Arrow Left + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnLeft_CIRCLE_MD_func = 23;

//---------------------------------
// CONFIGURE: Arrow Right + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnRight_CIRCLE_MD_func = 24;

//---------------------------------
// CONFIGURE: Arrow Down + CIRCLE
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnDown_CIRCLE_MD_func = 25;

//---------------------------------
// CONFIGURE: Arrow UP + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnUP_PS_MD_func = 38;

//---------------------------------
// CONFIGURE: Arrow Left + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnLeft_PS_MD_func = 40;

//---------------------------------
// CONFIGURE: Arrow Right + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnRight_PS_MD_func = 41;

//---------------------------------
// CONFIGURE: Arrow Down + PS
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnDown_PS_MD_func = 39;

//---------------------------------
// CONFIGURE: Arrow Up + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnUP_L1_MD_func = 34;

//---------------------------------
// CONFIGURE: Arrow Left + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnLeft_L1_MD_func = 36;

//---------------------------------
// CONFIGURE: Arrow Right + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnRight_L1_MD_func = 37;

//---------------------------------
// CONFIGURE: Arrow Down + L1
//---------------------------------
// Enter MarcDuino Function Code (1 - 78) (See Above)
int FTbtnDown_L1_MD_func = 35;

// ---------------------------------------------------------------------------------------
//               SYSTEM VARIABLES - USER CONFIG SECTION COMPLETED
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
//                          Drive Controller Settings
// ---------------------------------------------------------------------------------------

int marcDuinoBaudRate = 9600; // Set the baud rate for the Syren motor controller
                                    
#define SYREN_ADDR         129      // Serial Address for Dome Syren
#define SABERTOOTH_ADDR    128      // Serial Address for Foot Sabertooth

#define ENABLE_UHS_DEBUGGING 1
