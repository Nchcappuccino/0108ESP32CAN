// https://github.com/nhatuan84/esp32-can-protocol-demo
//上のプログラムがベース
#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

CAN_device_t CAN_cfg;
CAN_frame_t tx_frame;
CAN_frame_t rx_frame;
int i;
int t;
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
unsigned long currentMillis = 0;
const int CAN_interval = 5;//ms

void setup() {
  Serial.begin(115200);
  Serial.println("CAN start");
  CAN_cfg.speed=CAN_SPEED_1000KBPS;
  // CAN_cfg.speed=CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_pin_id = GPIO_NUM_2;
  CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
  //start CAN Module
  ESP32Can.CANInit();
}

void loop() {
  // t = micros();
  currentMillis = millis();
  // Serial.printf("%d\t",t);
  // CAN_frame_t tx_frame;
  Serial.printf("loop\t");
  // put your main code here, to run repeatedly:

    if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){
        Serial.printf("receive\t");
      //do stuff!

      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("New standard frame");
      else
        printf("New extended frame");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          printf("%d\t", rx_frame.data.u8[i]);
        }
      }
    }

  printf("t = %lu\t",currentMillis-previousMillis);
  if (currentMillis - previousMillis >= CAN_interval){
    previousMillis = currentMillis;
    if(i<250){
      i++;
    }else{
      i=0;
    }
    tx_frame.FIR.B.FF = CAN_frame_std;
    tx_frame.MsgID = 0x456;
    // tx_frame.MsgID = 1;//仮、pico
    tx_frame.FIR.B.DLC = 8;
    tx_frame.data.u8[0] = 0;
    tx_frame.data.u8[1] = 100;
    tx_frame.data.u8[2] = i;
    tx_frame.data.u8[3] = 0;
    tx_frame.data.u8[4] = 0;
    tx_frame.data.u8[5] = 0;
    tx_frame.data.u8[6] = 0;
    tx_frame.data.u8[7] = 0;
    ESP32Can.CANWriteFrame(&tx_frame);
  }
  Serial.printf("\r\n");
}