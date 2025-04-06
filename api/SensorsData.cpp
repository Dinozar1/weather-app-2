#include "SensorsData.h"
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>


using namespace std;
using namespace nlohmann;



vector<SensorsData::Sensor> SensorsData::sensors;

bool SensorsData::ParseSensorData(const string &jsonStr) {
    try {
        //Parse string
        auto json = json::parse(jsonStr);

        //Clear vector to update stations data
        sensors.clear();

        for ( const auto& stationJson : json) {
            Sensor sensor;
            /**
            * @brief Example JSON format:
            * @code
            * [
            *   {
            *     "id": 3486,
            *     "stationId": 515,
            *     "param": {
            *       "paramName": "benzen",
            *       "paramFormula": "C6H6",
            *       "paramCode": "C6H6",
            *       "idParam": 10
            *     }
            *   }
            * ]
            * @endcode
            */

            sensor.id = stationJson.value("id", 0);
            sensor.stationId = stationJson.value("stationId", 0);
            sensor.paramName = stationJson["param"].value("paramName", "");
            sensor.paramFormula = stationJson["param"].value("paramFormula", "");

            sensor.idParam = stationJson["param"].value("idParam", 0);
            sensor.paramCode = stationJson["param"].value("paramCode", "");

            sensors.push_back(sensor);
        }

        return true;
    }

    catch (const json::exception& e) {
        cout << "JSON parsing error: "<< e.what() << endl;
        return false;
    }
}

bool SensorsData::ParseSensorValues(const string &jsonStr) {
    try {
        //Parse string
        auto json = json::parse(jsonStr);

        //Clear all previous sensor values
        for (auto& sensor : sensors) {
            sensor.sensorsValues.clear();
        }
        /**
            * @brief Example JSON format:
            * @code
            * {
         *   "key": "PM10",
         *   "values": [
         *     {
         *       "date": "2025-03-27 12:00:00",
         *       "value": 7.0
         *     },
         *     {
         *       "date": "2025-03-27 11:00:00",
         *       "value": 6.9
         *     }
         *   ]
         * }
        * @endcode
        */




            //Extract paramKey
            string paramKey = json.value("key", "");

            //Extract all vales to auto var
            const auto& valuesArray = json["values"];

            // Iterate sensors
            for (auto& sensor : sensors) {

                if (sensor.paramCode == paramKey) {
                    //Iterate values and add all values to sensorValue struct
                    for (const auto& valueEntry : valuesArray) {
                        SensorValue sensorValue;

                        sensorValue.data = valueEntry.value("date", "");

                        //might be null
                        if (valueEntry.contains("value") && !valueEntry["value"].is_null()) {
                            try {
                                sensorValue.value = valueEntry["value"].get<double>();
                            }
                            catch (const std::exception& e) {
                                // Fallback to 0.0 if conversion fails
                                sensorValue.value = 0.0;
                                cout << "Warning: Could not convert value: " << e.what() << endl;
                            }
                        }
                        else {
                            // Set to 0.0 if value is null or missing
                            sensorValue.value = 0.0;
                        }

                        //add struct to sensors vector
                        sensor.sensorsValues.push_back(sensorValue);
                    }

                    break;
                }
            }


        return true;
    }

    catch (const json::exception& e) {
        cout << "JSON parsing error: "<< e.what() << endl;
        return false;
    }
}