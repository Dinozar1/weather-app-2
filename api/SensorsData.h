/**
* @file SensorsData.h
 * @brief Manages sensor data
 * @details Provides functionality for parsing and managing sensor data.
 */

#pragma once

#include <iostream>
#include <vector>
#include "StationData.h"

using namespace std;


/**
 * @class SensorsData
 * @brief Provides a class for storing data of the selected station
 * @details Provides structs and vectors representing GIOŚ api and parsing methods
 */

class SensorsData {
public:

    /**
     * @struct SensorValue
     * @brief Represents a single measurement from a sensor
     * @details Contains the timestamp and the measured value
     */
     struct SensorValue {
        string data;
        double value;
    };

    /**
     * @struct Sensor
     * @brief Represents an sensor
     * @details Contains information about the sensor and its values
     */
    struct Sensor {
        int id;
        int stationId;
        string paramName;
        string paramFormula;
        string paramCode;
        int idParam;
        vector<SensorValue> sensorsValues;
    };

    /** @brief Collection of all sensors */
    static vector<Sensor> sensors;

    /**
     * @brief Parses JSON string containing sensor data
     * @param jsonStr The JSON string to parse
     * @return true if parsing was successful, false otherwise
     */
    static bool ParseSensorData(const string &jsonStr);

    /**
     * @brief Parses JSON string containing sensor measurement values
     * @param jsonStr The JSON string to parse
     * @return true if parsing was successful, false otherwise
     */
    static bool ParseSensorValues(const string &jsonStr);
};