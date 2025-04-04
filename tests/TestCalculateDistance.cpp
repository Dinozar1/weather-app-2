#include <gtest/gtest.h>
#include <cmath>
#include "../api/StationData.h"

using namespace testing;

constexpr double EPSILON = 1e-6;

// Test case for same location
TEST(StationData, SameLocationReturnsZero) {
    static double lat = 52.400351099999995;  // Polanka 3 Wiit Poznań
    static double lon = 16.955804809402274;
    
    static double distance = StationData::CalculateDistance(lat, lon, lat, lon);
    
    EXPECT_NEAR(0.0, distance, EPSILON);
}

// Test case for known distance between poznan and wrocław
TEST(StationData, DistanceBetweenPoznanAndWroclaw) {
    // poznan
    static double lat1 = 52.4006632;
    static double lon1 = 16.91973259178088;
    
    // wroclaw
    static double lat2 = 51.1089776;
    static double lon2 = 17.0326689;
    
    static double distance = StationData::CalculateDistance(lat1, lon1, lat2, lon2);
    
    // Expected distance is approximately 3935.9 km
    EXPECT_NEAR(144, distance, 1.0);
}

// Test case for known distance between London and Warsaw
TEST(StationData, DistanceBetweenLondonAndSydney) {
    // London
    static double lat1 = 51.5074;
    static double lon1 = -0.1278;
    
    // Warszawa
    static double lat2 = 52.2337172;
    static double lon2 = 21.071432235636493;
    
    static double distance = StationData::CalculateDistance(lat1, lon1, lat2, lon2);
    
    // Expected distance is approximately 16989.6 km
    EXPECT_NEAR(1450, distance, 5.0);
}

// points on opposite sides of the Earth
TEST(StationData, AntipodalPoints) {
    // A point
    static double lat1 = 40.0;
    static double lon1 = 120.0;
    
    // opposite point on Earth
    static double lat2 = -40.0;
    static double lon2 = -60.0;
    
    static double distance = StationData::CalculateDistance(lat1, lon1, lat2, lon2);
    
    // Antipodal points should be approximately Earth's half-circumference apart
    // Earth's circumference is around 40030 km, so half is about 20015 km
    EXPECT_NEAR(20015.0, distance, 10.0);
}


// Test with invalid input
TEST(StationData, InvalidLatitudes) {
    static double lat1 = 100.0;  // Invalid latitude (beyond 90)
    static double lon1 = 0.0;
    
    static double lat2 = 0.0;
    static double lon2 = 0.0;
    
    // The function doesn't validate input, but the result will be mathematically incorrect
    // This test checks if the function handles these inputs without crashing
    static double distance = StationData::CalculateDistance(lat1, lon1, lat2, lon2);
    
    // The result should be a number (not NaN or infinity)
    EXPECT_FALSE(isnan(distance)); //https://en.cppreference.com/w/cpp/numeric/math/isnan
    EXPECT_FALSE(isinf(distance));//https://en.cppreference.com/w/cpp/numeric/math/isinf
}

// Test with invalid input
TEST(StationData, InvalidLongitudes) {
    static double lat1 = 0.0;
    static double lon1 = 200.0;  // Invalid longitude (beyond 180°)
    
    static double lat2 = 0.0;
    static double lon2 = 0.0;
    
    // This test checks if the function handles these inputs without crashing
    static double distance = StationData::CalculateDistance(lat1, lon1, lat2, lon2);
    
    // The result should be a number (not NaN or infinity)
    EXPECT_FALSE(isnan(distance));
    EXPECT_FALSE(isinf(distance));
}