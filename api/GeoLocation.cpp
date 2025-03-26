#include "StationData.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace nlohmann;
using namespace std;

double StationData::CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    constexpr double R = 6371.0; // Earth radius in km

    // Convert degrees to radians
    auto toRad = [](const double deg) { return deg * M_PI / 180.0; }; //lambda function that will convert to radians
    lat1 = toRad(lat1);
    lon1 = toRad(lon1);
    lat2 = toRad(lat2);
    lon2 = toRad(lon2);

    // Haversine formula
    // 1. difference in distance
    const double dlat = lat2 - lat1;
    const double dlon = lon2 - lon1;

    // 2. Haversine Calculation
    const double a = std::sin(dlat/2) * std::sin(dlat/2) +
              std::cos(lat1) * std::cos(lat2) *
              std::sin(dlon/2) * std::sin(dlon/2);

    // 3. Central Angle Calculation
    const double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));

    // 4. Multiplies the central angle by the Earth's radius
    return R * c;
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

    // Debug: print the encoded address
    cout << "Encoded Address: " << encoded_address << endl;

    // Construct URL - use full URL encoding
    string url = "https://nominatim.openstreetmap.org/search?";
    url += "q=" + string(encoded_address);
    url += "&format=json&limit=1";

    // Debug: print the full URL
    cout << "Full URL: " << url << endl;

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
