#include <Arduino_FreeRTOS.h>
#include <queue.h>

#define DHT_PIN 3  //PWM pin connected to sensor data pin

//Task Priorities (higher num = higher priority)
#define PRIORITY_SENSOR_READ 2
#define PRIORITY_UART_TRANSMIT 1

//global vars
volatile bool new_data_available = false; //used to track whether or not to print data

typedef struct {
  uint8_t bytes[5];
} SensorData;

//create a queue to safely share sensor data between tasks
QueueHandle_t xQueue_one;

void print_uart_task(void *params) {

  SensorData sensor_data;  // used to read in data from the queue

    while (1) {

      
      xQueueReceive(
      xQueue_one,  //queue name
      &sensor_data, //buffer pointer
      pdMS_TO_TICKS(200)
      );


      Serial.print(sensor_data.bytes[0]);  // Humidity integer
      Serial.print("%  |  Temp: ");
      Serial.print(sensor_data.bytes[2]);  // Temperature integer
      Serial.println("°C");

      vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

//  while (1) {
//    
//    taskENTER_CRITICAL();
//
//    if (new_data_available) {
//      new_data_available = false;
//
//      Serial.print(sensor_data[0]);  // Humidity integer
//      Serial.print("%  |  Temp: ");
//      Serial.print(sensor_data[2]);  // Temperature integer
//      Serial.println("°C");
//    }
//
//    taskEXIT_CRITICAL();
//
//    vTaskDelay(pdMS_TO_TICKS(2000));  // Print every 2 seconds

void read_sensor_task(void *params) {

  while (1) {
    
    Serial.println("Task is alive...");
  
  
    uint8_t data[5] = {0};
    bool good_checksum = true;
  
    //-------------------------------------------------
    //STEP1: Master pulls line low to initiate read
    //-------------------------------------------------
    
    DDRD |= (1 << 3); //set pin to output mode
    PORTD &= ~(1 << 3);
    
    //wait 20 millisec
    unsigned long starttime = micros();
    while (micros() - starttime <= 20000) { }
  
    //set high and wait 20-40 microsec
    PORTD |= (1 << 3);
    starttime = micros();
    while (micros() - starttime <= 40) {}
    
    noInterrupts();
    //-------------------------------------------------
    //STEP2: Slave Responds to start signal
    //-------------------------------------------------
    
    DDRD &= ~(1 << 3); //switch the PWM pin to input mode

    //wait for sensor low high response
    unsigned long timeout = micros();
    while ( (PIND & (1 << 3)) ) {
      if (micros() - timeout > 100) {
        Serial.println("Timeout waiting for sensor LOW response");
        vTaskDelay(pdMS_TO_TICKS(2000));
        return;
      }
    }
    
    //wait for sensor high response
    timeout = micros();
    while ( !(PIND & (1 << 3)) ) {
      if (micros() - timeout > 100) {
        Serial.println("Timeout waiting for sensor HIGH response");
        vTaskDelay(pdMS_TO_TICKS(2000));
        return;
      }
    }

    
    //-------------------------------------------------
    //STEP3: Slave sends 40-bits of data
    //-------------------------------------------------

      taskENTER_CRITICAL();
      for (int i=0 ; i<40 ; i++ ) {

        while ( !(PIND & (1 << 3)) ) {
     
        }
  
        unsigned long high_start = micros();
  
        while ( (PIND & (1 << 3)) ) {
          
        }
  
        unsigned long high_duration = micros() - high_start;

        data[i / 8] <<= 1;
        if (high_duration > 50) {
          data[i / 8] |= 1; //high bit
        }
      }
      taskEXIT_CRITICAL();
      interrupts();
    
    //-------------------------------------------------
    //STEP4: Master validates data using checksum
    //-------------------------------------------------
  
    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF; //ignore overflow

    //validate data
    if (checksum != data[4]) {
      good_checksum = false; 
    }

    if (good_checksum) {

      SensorData send_data;
      for (int i=0 ; i<5 ; i++ ) {
        send_data.bytes[i] = data[i];
      }

      xQueueSend(xQueue_one, &send_data, pdMS_TO_TICKS(200));
    }

    

    vTaskDelay(pdMS_TO_TICKS(2000));  // Read every 2 seconds
    

  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  // wait for Serial to be ready

  xTaskCreate(
    read_sensor_task,
    "read_sensor",
    256,
    NULL,
    PRIORITY_SENSOR_READ,
    NULL
  );

  xTaskCreate(
    print_uart_task,
    "print_uart",
    256,
    NULL,
    PRIORITY_UART_TRANSMIT,
    NULL
  );

  xQueue_one = xQueueCreate( 1, sizeof(SensorData) ); //holds 5 bytes = 40 bits
}


void loop() {
  // put your main code here, to run repeatedly:

}
