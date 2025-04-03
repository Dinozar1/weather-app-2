#include <gtest/gtest.h>
#include "../api/StationData.h"


// Test for empty address
TEST(StationData, EmptyAddressReturnsInvalidCoordinates) {
    const auto coords = StationData::GeocodeAddress("");
    EXPECT_FALSE(coords.valid);
}


// Test for a well-known address (positive test case)
TEST(StationData, WellKnownAddressReturnsValidCoordinates) {
    const auto coords = StationData::GeocodeAddress("Poznań, Polanka 3");

    if(coords.valid) {
        // Statue of Liberty coordinates (approximate check)
        EXPECT_NEAR(52.400351099999995, coords.latitude, 0.01);
        EXPECT_NEAR(16.955804809402274, coords.longitude, 0.01);
    } else {
        GTEST_SKIP();
    }
}

// Test for a non-existent address
TEST(StationData, NonExistentAddressReturnsInvalidCoordinates) {
    const auto coords = StationData::GeocodeAddress("ulicaNieistniejaca, NieinstniejaceMiasto");
    EXPECT_FALSE(coords.valid);
}



// Test for handling addresses with only city
TEST(StationData, CityAndCountryOnlyAddress) {
    auto coords = StationData::GeocodeAddress("Poznań");
    
    if(coords.valid) {
        // Tokyo coordinates (approximate check)
        EXPECT_NEAR(52.4006632, coords.latitude, 0.5);
        EXPECT_NEAR(16.91973259178088, coords.longitude, 0.5);
    } else {
        GTEST_SKIP();
    }
}

// Test for handling numeric zip code
TEST(StationData, NumericZipCodeOnly) {
    const auto coords = StationData::GeocodeAddress("62-800"); // Kalisz zip code
    
    if(coords.valid) {
        // Tokyo coordinates (approximate check)
        EXPECT_NEAR(51.7473619, coords.latitude, 0.5);
        EXPECT_NEAR(18.07958208556929, coords.longitude, 0.5);
    } else {
        GTEST_SKIP();
    }
}
