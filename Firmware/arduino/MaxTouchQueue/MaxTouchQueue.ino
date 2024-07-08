#include <bb_captouch.h>
#include <Wire.h>
#include <OneBitDisplay.h>
#include "freertos/queue.h"
#define DEBUG_VERBOSE false

ONE_BIT_DISPLAY obd;

#define LCD_DISPON 4
#define LCD_EXTMODE 14

#define CS_PIN 3
#define MOSI_PIN 2
#define CLK_PIN 1

#define BITBANG 0
uint8_t ucBackBuf[400*240/8];

TOUCHINFO ti;
BBCapTouch bbct;
int iWidth, iHeight;

#define TOUCH_SDA 7
#define TOUCH_SCL 8
#define TOUCH_INT 6
#define TOUCH_RST 0

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t touch_queue = NULL;

#define TOUCH_QUEUE_LENGTH 12  // Try not to make it super large
struct xydata {
	uint16_t x;
	uint16_t y;
} xy_data;

int numints = 0;

void IRAM_ATTR touch_isr() {
	numints++;
  xQueueSendFromISR(gpio_evt_queue, &numints, NULL);
}

/* this function will be invoked when additionalTask was created */
void ISRprocessing( void * parameter )
{
  uint32_t io_num;
  /* loop forever */
  while (1) {
  
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
      if (bbct.getSamples(&ti)) {
          xy_data.x = ti.x[0];
          xy_data.y = ti.y[0];

          if (xQueueSend( touch_queue, &xy_data, ( TickType_t ) 0 ) == pdTRUE) {
                // The message was sent successfully
            } else {
                Serial.println("ERR: Could not add xy_data in queue");
            }
      } 
    }     
  }
  // Should never get out
  vTaskDelete( NULL );
}

void setup() {
  //create a queue to handle gpio event from isr
  gpio_evt_queue = xQueueCreate(TOUCH_QUEUE_LENGTH, sizeof(uint8_t));
  touch_queue = xQueueCreate(TOUCH_QUEUE_LENGTH, sizeof(xy_data));

  pinMode(TOUCH_INT, INPUT_PULLUP);
  attachInterrupt(TOUCH_INT, touch_isr, FALLING);

  pinMode(LCD_EXTMODE, OUTPUT);
  pinMode(LCD_DISPON, OUTPUT);
  digitalWrite(LCD_DISPON, 1);
  digitalWrite(LCD_EXTMODE, 1);
  
  Serial.begin(115200);

  obd.setSPIPins(CS_PIN, MOSI_PIN, CLK_PIN, -1);
  obd.SPIbegin(SHARP_400x240, 6000000L);
  obd.setBuffer(ucBackBuf);
  obd.fillScreen(OBD_BLACK);
  obd.setFont(FONT_16x16);
  obd.setTextColor(OBD_WHITE);
  obd.println();
  obd.println("SOFTWARE: bitbank LARRY");
  obd.println();
  obd.println("HARDWARE: Fasani Corp.");
  obd.display();
  delay(1000);
  obd.fillScreen(OBD_WHITE);
  obd.display();
  Serial.println("Test this TOUCH");

  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();
  Serial.printf("Sensor type = %d\n", iType);

    xTaskCreate(
    ISRprocessing,           /* Task function. */
    "ISRprocessing",        /* name of task. */
    8000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    5,                        /* priority of the task */
    NULL);  
    
}

void loop() {
    /* Read from the touch Queue */
    if( xQueueReceive(touch_queue, &xy_data, ( TickType_t ) 0 ) ) {
      #if DEBUG_VERBOSE
        Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
        Serial.print("  x: ");Serial.print(ti.x[i]);
        Serial.print("  y: ");Serial.print(ti.y[i]);
        Serial.print("  size: ");Serial.println(ti.area[i]);
      #endif
    
        obd.fillCircle(xy_data.y, xy_data.x, 4, OBD_BLACK);
        obd.display();
        //Serial.printf("num INTs:%d\n", numints);
      }  
}

