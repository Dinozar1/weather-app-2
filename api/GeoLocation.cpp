
/**
 * @file StationData.cpp
 * @brief Implementation of the StationData class for geocoding and distance calculations of stations
 * @details This file contains source of geolocations methods of StationData class, it provides distance calculator between
 *  two objects and method that provides geolocation of a address
 */

#include "StationData.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace nlohmann;
using namespace std;


double StationData::CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    //https://www.geeksforgeeks.org/haversine-formula-to-find-distance-between-two-points-on-a-sphere/

    // distance between latitudes and longitudes
    double dLat = (lat2 - lat1) * M_PI / 180.0; // fi2 - fi1
    double dLon = (lon2 - lon1) * M_PI / 180.0; // lambda2 - lambda1

    // convert to radians
    lat1 = (lat1) * M_PI / 180.0;
    lat2 = (lat2) * M_PI / 180.0;

    // apply formulae
    double a = pow(sin(dLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);

    double rad = 6371.0; // Earth radius in kilometers

    return rad * 2 * asin(sqrt(a));
}

StationData::GeoCoordinates StationData::GeocodeAddress(const string &address) {
    GeoCoordinates coords; // Default constructor sets valid=false

    if (address.empty()) return coords; // Return invalid coords

    CURL* curl = curl_easy_init(); // init curl
    if (!curl) {
        cerr << "Failed to initialize CURL" << endl;
        return coords;
    }

    // URL encoding
    char* encoded_address = curl_easy_escape(curl, address.c_str(), 0);
    if (!encoded_address) {
        cerr << "Failed to URL encode address" << endl;
        curl_easy_cleanup(curl);
        return coords;
    }

    // Construct URL - use full URL encoding
    string url = "https://nominatim.openstreetmap.org/search?";
    url += "q=" + string(encoded_address);
    url += "&format=json&limit=1";

    // Prepare headers
    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "User-Agent: AirQualityMonitor/1.0");

    // Response storage
    string response;

    // Set curl options with careful checking
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);


    // Perform request
    const CURLcode res = curl_easy_perform(curl);

    // Clean up
    curl_slist_free_all(headers);
    curl_free(encoded_address);
    curl_easy_cleanup(curl);

    // Check for request failure
    if (res != CURLE_OK) {
        cerr << "Curl request failed: " << curl_easy_strerror(res) << endl;
        return coords;
    }

    // Rest of your JSON parsing code remains the same
    try {
        auto json = nlohmann::json::parse(response);
        if (json.empty()) {
            cerr << "No results found for the address" << endl;
            return coords;
        }

        coords.latitude = stod(json[0]["lat"].get<string>());
        coords.longitude = stod(json[0]["lon"].get<string>());
        coords.valid = true;
        return coords;
    }
    catch (const json::exception& e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        cerr << "Response received: " << response << endl;
    }

    return coords;
}
