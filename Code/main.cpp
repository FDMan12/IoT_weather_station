#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AM2320.h>
#include <BluetoothSerial.h>

Adafruit_BMP280 bmp;
Adafruit_AM2320 am2320 = Adafruit_AM2320();
BluetoothSerial SerialBT;

const int gasSensorPin = 36;

struct DataPoint {
  float temperature;
  float humidity;
  float pressure;
  float gas;
};

#define NUM_DATA_POINTS 8
DataPoint dataPoints[NUM_DATA_POINTS];
int dataIndex = 0;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_MeteoStation"); // Название Bluetooth устройства
  Wire.begin();

  if (!bmp.begin(0x76)) {
    Serial.println("Не удалось найти BMP280!");
    while (1);
  }

  if (!am2320.begin()) {
    Serial.println("Не удалось найти AM2320!");
    while (1);
  }
}

void loop() {
  float temperature = am2320.readTemperature();
  float humidity = am2320.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;
  int gasValue = analogRead(gasSensorPin);
  float gasConcentration = (float)gasValue / 4095.0 * 100;

  dataPoints[dataIndex] = {temperature, humidity, pressure, gasConcentration};
  dataIndex = (dataIndex + 1) % NUM_DATA_POINTS;

  if (dataIndex == 0) {
    float tempSum = 0, humSum = 0, presSum = 0, gasSum = 0;
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
      tempSum += dataPoints[i].temperature;
      humSum += dataPoints[i].humidity;
      presSum += dataPoints[i].pressure;
      gasSum += dataPoints[i].gas;
    }

    float tempAvg = tempSum / NUM_DATA_POINTS;
    float humAvg = humSum / NUM_DATA_POINTS;
    float presAvg = presSum / NUM_DATA_POINTS;
    float gasAvg = gasSum / NUM_DATA_POINTS;

    float tempForecast = dataPoints[NUM_DATA_POINTS - 1].temperature + (dataPoints[NUM_DATA_POINTS - 1].temperature - dataPoints[0].temperature);
    float humForecast = dataPoints[NUM_DATA_POINTS - 1].humidity + (dataPoints[NUM_DATA_POINTS - 1].humidity - dataPoints[0].humidity);
    float presForecast = dataPoints[NUM_DATA_POINTS - 1].pressure + (dataPoints[NUM_DATA_POINTS - 1].pressure - dataPoints[0].pressure);
    float gasForecast = dataPoints[NUM_DATA_POINTS - 1].gas + (dataPoints[NUM_DATA_POINTS - 1].gas - dataPoints[0].gas);

    SerialBT.print("Average Temperature: ");
    SerialBT.print(tempAvg);
    SerialBT.print(" °C, Forecast: ");
    SerialBT.print(tempForecast);
    SerialBT.println(" °C");

    SerialBT.print("Average Humidity: ");
    SerialBT.print(humAvg);
    SerialBT.print(" %, Forecast: ");
    SerialBT.print(humForecast);
    SerialBT.println(" %");

    SerialBT.print("Average Pressure: ");
    SerialBT.print(presAvg);
    SerialBT.print(" hPa, Forecast: ");
    SerialBT.print(presForecast);
    SerialBT.println(" hPa");

    SerialBT.print("Average Gas Concentration: ");
    SerialBT.print(gasAvg);
    SerialBT.print(" %, Forecast: ");
    SerialBT.print(gasForecast);
    SerialBT.println(" %");
  }

  delay(10800000);
}
