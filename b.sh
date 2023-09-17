sudo apt update
sudo apt-get install pkg-config
sudo snap install tree
sudo apt-get install curl zip unzip tar
sudo apt install make cmake g++
sudo apt-get install libgflags-dev
sudo apt-get install nlohmann-json3-dev

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install

./vcpkg install nlohmann-json
./vcpkg install gflags

./vcpkg remove gflags

cd ..


wget https://download.libsodium.org/libsodium/releases/LATEST.tar.gz
tar -xzvf LATEST.tar.gz
cd libsodium-stable
./configure
make
sudo make install
cd ..

full_path=$(realpath "./vcpkg/installed/x64-linux")
export CMAKE_PREFIX_PATH="$full_path"


mkdir build
cd build
cmake ..
make
