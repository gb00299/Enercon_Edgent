/****************************************************/
/*     Enercon Power Control Module - Firmware      */
/****************************************************/

/****************************************************/
/************* Definitions & Includes ***************/
/****************************************************/

#define BLYNK_TEMPLATE_ID "TMPLFRtcxaBq"
#define BLYNK_DEVICE_NAME "Enercon Pwr"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG

#include "BlynkEdgent.h"

//HW Setup Definitions
#define ACTION_SWITCH_OPEN      1
#define ACTION_SWITCH_CLOSE     3
#define REPORT_P1              12
#define REPORT_P2              13
#define REPORT_P3              14
#define REPORT_SWITCH_STATUS   15

//Virtual Pins Definitions (see Datastreams on blynk.cloud)
#define BLYNK_ACTION_SWITCH_OPEN      V0
#define BLYNK_ACTION_SWITCH_CLOSE     V1
#define BLYNK_REPORT_SWITCH           V2
#define BLYNK_DISPLAY_P1             V11
#define BLYNK_DISPLAY_P2             V12
#define BLYNK_DISPLAY_P3             V13
#define BLYNK_DISPLAY_SWITCH_CLOSED  V14
#define BLYNK_DISPLAY_SWITCH_OPEN    V15

//Timers and global items
#define SWITCH_PRESS_INTERVAL   100L
#define CAN_OPERATE_INTERVAL   5000L
BlynkTimer switchTimer;
BlynkTimer canOperateTimer;

bool canOperateSwitch = false;
int phase1 = 0;
int phase2 = 0;
int phase3 = 0;
bool switchIsOpen = true;

/****************************************************/
/************* Function Declarations ****************/
/****************************************************/

void setup_HWpins (void);
bool check_phase_status (void);
bool check_switch_status (void);
void update_BlynkServer (int p1, int p2, int p3, bool switchOpen);
void closeSwitch (void);
void openSwitch (void);
void switchTimeout (void);
void canOperateTimeout (void);

/****************************************************/
/*************** Main Arduino Code ******************/
/****************************************************/

void setup()
{
  Serial.begin(115200);
  delay(100);
  setup_HWpins();
  BlynkEdgent.begin();
  update_BlynkServer(phase1, phase2, phase3, switchIsOpen);
}

void loop() {
  BlynkEdgent.run();

  bool needsUpdate = false;
  needsUpdate |= check_phase_status ();
  needsUpdate |= check_switch_status ();

  if (needsUpdate) update_BlynkServer(phase1, phase2, phase3, switchIsOpen);
}

/****************************************************/
/************** Function Definitions ****************/
/****************************************************/

void setup_HWpins (void)
{
  //Set the RESET button
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLUP);

  //Set the action pins
  pinMode(ACTION_SWITCH_OPEN, OUTPUT);
  pinMode(ACTION_SWITCH_CLOSE, OUTPUT);
  digitalWrite(ACTION_SWITCH_OPEN,  HIGH);
  digitalWrite(ACTION_SWITCH_CLOSE, HIGH);

  //Set the report pins
  pinMode(REPORT_P1, INPUT);
  pinMode(REPORT_P2, INPUT);
  pinMode(REPORT_P3, INPUT);
  pinMode(REPORT_SWITCH_STATUS, INPUT);
}

bool check_phase_status (void)
{
  bool update = false;

  int newP1 = digitalRead(REPORT_P1);
  int newP2 = digitalRead(REPORT_P2);
  int newP3 = digitalRead(REPORT_P3);

  if (newP1 != phase1) update = true;
  if (newP2 != phase2) update = true;
  if (newP3 != phase3) update = true;

  phase1 = newP1;
  phase2 = newP2;
  phase3 = newP3;

  return update;
}

bool check_switch_status (void)
{
  bool update = false;

  int pin = digitalRead(REPORT_SWITCH_STATUS);
  bool newSwitchIsOpen;
  if (pin == HIGH)
  {
    newSwitchIsOpen = true;
  }
  else
  {
    newSwitchIsOpen = false;
  }

  if (newSwitchIsOpen != switchIsOpen) update = true;
  switchIsOpen = newSwitchIsOpen;

  return update;
}

void update_BlynkServer (int p1, int p2, int p3, bool switchOpen)
{
  Blynk.virtualWrite (BLYNK_DISPLAY_P1, p1);
  Blynk.virtualWrite (BLYNK_DISPLAY_P2, p2);
  Blynk.virtualWrite (BLYNK_DISPLAY_P3, p3);

  if (digitalRead(switchOpen) == true)
  {
    Blynk.virtualWrite (BLYNK_DISPLAY_SWITCH_OPEN,   "   Switch open   ");
    Blynk.virtualWrite (BLYNK_DISPLAY_SWITCH_CLOSED, "-----------------");
  }
  else
  {
    Blynk.virtualWrite (BLYNK_DISPLAY_SWITCH_OPEN,   "-----------------");
    Blynk.virtualWrite (BLYNK_DISPLAY_SWITCH_CLOSED, "  Switch closed  ");
  }
}

/****************************************************/
/************ Blynk Actions and Triggers ************/
/****************************************************/

BLYNK_WRITE(BLYNK_ACTION_SWITCH_OPEN)
{
  if((param.asInt() == 1)&&(canOperateSwitch==true))
  {
    canOperateSwitch = false;
    openSwitch();
    canOperateTimer.setTimeout(CAN_OPERATE_INTERVAL, canOperateTimeout);
  }
}

BLYNK_WRITE(BLYNK_ACTION_SWITCH_CLOSE)
{
  if((param.asInt() == 1)&&(canOperateSwitch==true))
  {
    canOperateSwitch = false;
    closeSwitch();
    canOperateTimer.setTimeout(CAN_OPERATE_INTERVAL, canOperateTimeout);
  }
}

/****************************************************/
/****************** Blynk Helpers *******************/
/****************************************************/

void closeSwitch (void)
{
  digitalWrite(ACTION_SWITCH_OPEN, HIGH);
  digitalWrite(ACTION_SWITCH_CLOSE, LOW);
  switchTimer.setTimeout(SWITCH_PRESS_INTERVAL, switchTimeout);
}

void openSwitch (void)
{
  digitalWrite(ACTION_SWITCH_CLOSE, HIGH);
  digitalWrite(ACTION_SWITCH_OPEN, LOW);
  switchTimer.setTimeout(SWITCH_PRESS_INTERVAL, switchTimeout);
}

void switchTimeout (void)
{
  digitalWrite(ACTION_SWITCH_CLOSE, HIGH);
  digitalWrite(ACTION_SWITCH_OPEN, HIGH);
}

void canOperateTimeout (void)
{
  canOperateSwitch = true;
}

/****************************************************/
/****************************************************/
