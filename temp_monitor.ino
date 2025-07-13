#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <SimpleDHT.h>

#define DHT_PIN 3

//higher num == higher priority
//read_sensor task is timing-sensitive
#define PRIORITY_SENSOR_READ 2
#define PRIORITY_UART_TRANSMIT 1

//init sensor
SimpleDHT11 temp_sensor;

//use this struct as the message format for the queue
typedef struct {
  byte temperature;
  byte humidity;
} SensorData;

QueueHandle_t xQueue_one;

void print_uart_task(void *params) {
  SensorData sensor_data;
  while (1) {
    //transmit data if received
    if (xQueueReceive(xQueue_one, &sensor_data, pdMS_TO_TICKS(200)) == pdTRUE) {
      Serial.print("Humidity: ");
      Serial.print(sensor_data.humidity);
      Serial.print("%  |  Temp: ");
      Serial.print(sensor_data.temperature);
      Serial.println("°C");
    } else {
      Serial.println("No data received");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void read_sensor_task(void *params) {
  SensorData data;
  while (1) {

    //read sensor and send to queue
    int error = temp_sensor.read(DHT_PIN, &data.temperature, &data.humidity, NULL);
    if (error == SimpleDHTErrSuccess) {
      xQueueSend(xQueue_one, &data, 0);
    } else {
      Serial.println("Failed to read from sensor");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup() {
  Serial.begin(9600);
  while( !Serial );
  delay(100); 

  xQueue_one = xQueueCreate(5, sizeof(SensorData));
  if (xQueue_one == NULL) {
    Serial.println("Failed to create queue");
    while (1);
  }

  xTaskCreate(read_sensor_task, "read_sensor", 256, NULL, PRIORITY_SENSOR_READ, NULL);
  xTaskCreate(print_uart_task, "print_uart", 256, NULL, PRIORITY_UART_TRANSMIT, NULL);

  Serial.println("All set");
}

void loop() {
  
}
