#pragma once

#include <iostream>
#include <vector>
#include "StationData.h"

using namespace std;

class SensorsData {
public:
    struct Sensor {
        int id;
        int stationId;
        string paramName;
        string paramFormula;
        string paramCode;
        int idParam;
    };

    static vector<Sensor> sensors;

    static bool ParseSensorData(const string &jsonStr);
};