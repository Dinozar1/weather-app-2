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

            // parse station details to object
            // Example object:
            // {
            //     "id": 3486,
            //     "stationId": 515,
            //     "param": {
            //          "paramName": "benzen",
            //          "paramFormula": "C6H6",
            //          "paramCode": "C6H6",
            //          "idParam": 10
            //      }
            // }

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