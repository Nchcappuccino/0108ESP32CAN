#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

CAN_device_t CAN_cfg;
CAN_frame_t tx_frame;
CAN_frame_t rx_frame;
int i;

void setup() {
  Serial.begin(115200);
  Serial.println("iotsharing.com CAN demo");
  CAN_cfg.speed=CAN_SPEED_1000KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;
  CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
  //start CAN Module
  ESP32Can.CANInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(i<250){
    i++;
  }else{
    i=0;
  }
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 1110;
  tx_frame.FIR.B.DLC = 8;
  tx_frame.data.u8[0] = i;
  tx_frame.data.u8[1] = 100;
  tx_frame.data.u8[2] = 200;
  tx_frame.data.u8[3] = 0;
  tx_frame.data.u8[4] = 0;
  tx_frame.data.u8[5] = 0;
  tx_frame.data.u8[6] = 0;
  tx_frame.data.u8[7] = 0;
  tx_frame.data.u8[8] = 0;

  ESP32Can.CANWriteFrame(&tx_frame);
  
      if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

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
          printf("%c\t", (char)rx_frame.data.u8[i]);
        }
        printf("\n");
      }
    }

}