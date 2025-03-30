/**
* @file StationData.cpp
 * @brief Implementation of StationData class methods
 * @details Contains functions for fetching, parsing, and processing station data.
 */

#include "StationData.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;
using namespace nlohmann;


/**
 * @brief Helper function to clean up curl resources
 * @param curl Pointer to the curl handle to clean up
 */
void curlCleanup(CURL *curl) {
    curl_easy_cleanup(curl); // Clean up the current curl
    curl_global_cleanup(); //Perform global curl cleanup
}

vector<StationData::Station> StationData::stations;


/**
 * @brief Fetches data from the specified URI
 * @details Uses libcurl to make an HTTP request to the given URI
 *          and returns the response as a string
 * @param uri The URI string endpoint to fetch data from
 * @return JSON string containing station data or empty string on failure
 */
string StationData::FetchStations(const string& uri) {
    string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT); // init curl

    if (CURL *curl = curl_easy_init()) {  //create curl handle
        curl_easy_setopt(curl, CURLOPT_URL, uri.c_str()); // set the url
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
            cout << "Nie udało się pobrać danych z api" << curl_easy_strerror(res) << endl;
            curlCleanup(curl);
            return "";
        }

        curlCleanup(curl);

        return readBuffer;
    }
    return "";
}

/**
 * @brief Callback function for libcurl to handle received data
 * @details When libcurl makes an HTTP request, data isn't received all at once.
 *          It comes in chunks. This callback function:
 *          - Collects these chunks
 *          - Stores them in a single string
 *          - Allows processing the entire response
 * @param contents Pointer to the received data
 * @param size Size of each data element
 * @param nmemb Number of data elements
 * @param userp Pointer to string buffer
 * @return Total size of processed data
 */

size_t StationData::WriteCallback(void *contents, const size_t size, const size_t nmemb, void *userp) {

    //Cast the user pointer to a string pointer
    const auto output = static_cast<string *>(userp);

    //Calculate total size
    const size_t totalSize = size * nmemb;

    //Append received data to the string
    output->append(static_cast<char *>(contents), totalSize);

    // Return numer of bytes to processed
    return totalSize;
}


/**
 * @brief Parses JSON string containing station data
 * @details Processes the JSON response from the fetch method and populates
 *          the stations vector with Station objects
 * @param jsonStr The JSON string to parse
 */
void StationData::ParseStations(const string& jsonStr) {
    try {
        //Parse string
        auto json = json::parse(jsonStr);

        //Clear vector to update stations data
        stations.clear();

        for ( const auto& stationJson : json) {
            Station station;

            /**
              * @brief Example JSON object structure:
              * @code
              * {
              *     "id": 741,
              *     "stationName": "Malbork, ul. Mickiewicza",
              *     "gegrLat": "54.031247",
              *     "gegrLon": "19.032899",
              *     "city": {
              *         "id": 527,
              *         "name": "Malbork",
              *         "commune": {
              *             "communeName": "Malbork",
              *             "districtName": "malborski",
              *             "provinceName": "POMORSKIE"
              *         }
              *     },
              *     "addressStreet": "ul. Mickiewicza" // might be null!
              * }
              * @endcode
              */

            station.id = stationJson.value("id", 0);
            station.stationName = stationJson.value("stationName", "");
            station.gegrLat = stationJson.value("gegrLat", "");
            station.gegrLon = stationJson.value("gegrLon", "");

            station.cityId = stationJson["city"].value("id", 0);
            station.cityName = stationJson["city"].value("name", "");

            station.communeName = stationJson["city"]["commune"].value("communeName", "");
            station.districtName = stationJson["city"]["commune"].value("districtName", "");
            station.provinceName = stationJson["city"]["commune"].value("provinceName", "");

            // might be null
            station.addressStreet = stationJson.contains("addressStreet") && stationJson["addressStreet"].is_string()
                ? stationJson["addressStreet"].get<std::string>() : "";

            stations.push_back(station);
        }
    }

    catch (const json::exception& e) {
        cout << "JSON parsing error: "<< e.what() << endl;
    }
}
