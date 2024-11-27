# Setup Instructions

1. Clone or pull the repository from your GitHub.  
2. Get/update SFML by running: `git submodule update --init --recursive`.  
3. Navigate to the `lib/vcpkg` folder and execute the following commands:  
   - For Windows: `./bootstrap-vcpkg.bat` and `./vcpkg install gtest`.  
   - For macOS: `./bootstrap-vcpkg.sh` and `./vcpkg install gtest`.  
4. Run CMake to generate your build folder.