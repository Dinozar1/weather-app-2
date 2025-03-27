#pragma once

#include <iostream>
#include <vector>
#include "StationData.h"

using namespace std;

class SensorsData {
public:
     struct SensorValue {
        string data;
        double value;
    };

    struct Sensor {
        int id;
        int stationId;
        string paramName;
        string paramFormula;
        string paramCode;
        int idParam;
        vector<SensorValue> sensorsValues;
    };

    static vector<Sensor> sensors;

    static bool ParseSensorData(const string &jsonStr);
    static bool ParseSensorValues(const string &jsonStr);
};