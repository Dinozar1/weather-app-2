#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>



using namespace std;

class StationData {
    public:
    struct Station {
        int id;
        string stationName;
        string gegrLat;
        string gegrLon;
        int cityId;
        string cityName;
        string communeName;
        string districtName;
        string provinceName;
        string addressStreet;
    };

    struct GeoCoordinates {
        double latitude = 0.0f;
        double longitude = 0.0f;
        bool valid = false;
    };

    static vector<Station> stations;

    static string FetchStations(const string& uri);

    static void ParseStations(const string& jsonStr);

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *user);

    static double CalculateDistance(double lat1, double lon1, double lat2, double lon2);

    static GeoCoordinates GeocodeAddress(const std::string &address);

    friend class SensorData;
};