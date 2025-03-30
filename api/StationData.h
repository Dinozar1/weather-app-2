/**
 * @file StationData.h
 * @brief Manages station data
 * @details Provides functionality for fetching, parsing, and managing
 *          air quality monitoring station information.
 */

#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

/**
 * @class StationData
 * @brief Provides class for storing Station information and geolocation features
 * @details Provides struct for Station data and struct for geolocation data,
 * provides methods for fetching api and parsing data to json format and methods for geolocation purposes
 */
class StationData {
public:
    /**
     * @struct Station
     * @brief Represents a Station in GIOS api system
     * @details Contains identification and geographical information about a station
     */
    struct Station {
        int id;
        string stationName;
        string gegrLat;     /**< Geographical latitude as string */
        string gegrLon;     /**< Geographical longitude as string */
        int cityId;
        string cityName;
        string communeName;
        string districtName;
        string provinceName;
        string addressStreet;
    };

    /**
     * @struct GeoCoordinates
     * @brief Represents a geographical position of the station
     * @details Contains parsed latitude and longitude coordinates
     */
    struct GeoCoordinates {
        double latitude = 0.0f; /**< Latitude in decimal degrees */
        double longitude = 0.0f; /**< Longitude in decimal degrees */
        bool valid = false;     /**< Indicates if coordinates are valid */
    };

    /** @brief Collection of all stations */
    static vector<Station> stations;

    /**
     * @brief Fetches station data from the specified URI
     * @param uri The URI endpoint to fetch data from
     * @return JSON string containing station data or empty string on failure
     */
    static string FetchStations(const string& uri);

    /**
     * @brief Parses JSON string containing station data
     * @param jsonStr from the return of FetchStation method
     */
    static void ParseStations(const string& jsonStr);

    /**
     * @brief Callback function for libcurl to handle received data
     * @param contents Pointer to the received data
     * @param size Size of each data element
     * @param nmemb Number of data elements
     * @param user Pointer to user data (string buffer)
     * @return Total size of processed data
     */
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *user);

    /**
     * @brief Calculates the distance between two geographical points
     * @param lat1 Latitude of the first point
     * @param lon1 Longitude of the first point
     * @param lat2 Latitude of the second point
     * @param lon2 Longitude of the second point
     * @return Distance in kilometers
     */
    static double CalculateDistance(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Converts an address string to geographical coordinates using openstrees api
     * @param address The address from input
     * @return GeoCoordinates structure containing latitude, longitude and validation
     */
    static GeoCoordinates GeocodeAddress(const std::string &address);

    friend class SensorData; /**< Allows SensorData class to access private members */
};