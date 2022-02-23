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

//Timers and more
#define SWITCH_PRESS_INTERVAL   100L
#define CAN_OPERATE_INTERVAL   5000L
BlynkTimer switchTimer;
BlynkTimer canOperateTimer;
bool canOperateSwitch = false;

/****************************************************/
/************* Function Declarations ****************/
/****************************************************/


/****************************************************/
/*************** Main Arduino Code ******************/
/****************************************************/

void setup()
{
  Serial.begin(115200);
  delay(100);
  setup_HWpins();
  BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();

  bool needsUpdate = false;




  //if (needsUpdate) update_BlynkServer(***);
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
  return true;
}

bool check_switch_status (void)
{
  return true;
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
