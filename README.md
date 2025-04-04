# Polish Air Quality Monitoring Application

## Project Description

This application allows users to:
- Fetch all measurement stations across Poland
- Filter stations by city name
- Search for stations within a given radius using OpenStreetMap's Nominatim geocoding service
- View measurement sensors for selected stations
- Display and analyze measurement data with statistical processing and graphical representation

## Supported Platforms
- Linux (Debian)
- Linux (Arch Linux)
- Windows

## System Requirements
- C++17 or later
- CMake (3.12 or later)
- vcpkg package manager



## Setup and Installation (Linux)

### Debian Setup

1. Update system packages
```bash
sudo apt update
sudo apt upgrade
```

2. Install required build tools
```bash
sudo apt install build-essential cmake git wget curl unzip software-properties-common g++ gcc
```


3. Install vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

4. Install project dependencies via vcpkg
```bash
./vcpkg install
```

### Arch Linux Setup

1. Update system packages
```bash
sudo pacman -Syu
```

2. Install build tools
```bash
sudo pacman -S base-devel cmake git wget curl unzip gcc
```

3. Install vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
```

4. Install project dependencies via vcpkg
```bash
./vcpkg install
```


## IDE configuration
For cmake add path to vcpkg.cmake in build options
```
-DCMAKE_TOOLCHAIN_FILE=/path/to/project/vcpkg/scripts/buildsystems/vcpkg.cmake
```

## API References
- GIOS Air Quality API: https://powietrze.gios.gov.pl/pjp/content/api
- OpenStreetMap Nominatim: https://nominatim.openstreetmap.org/
  
