# ubuntu
# 编译mariadb-conn
# sudo apt-get update
# sudo apt-get install -y git cmake make gcc libssl-dev
mkdir -p deps/mariadb-connector-cpp/build && cd deps/mariadb-connector-cpp/build
cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCONC_WITH_UNIT_TESTS=Off -DCMAKE_INSTALL_PREFIX=/usr/local -DWITH_SSL=OPENSSL
cmake --build . --config RelWithDebInfo
sudo make install

export LD_LIBRARY_PATH=/mariadb:$LD_LIBRARY_PATH
cd ../../../
