# DigiAsset Core Instuctions for Raspberry Pi 4/5 (arm64)

**Note**: Go [here](readme.md) for instructions for x86_64 devices - e.g. AMD/Intel

**Tip**: You can use [DigiNode Tools](https://diginode.tools) for a fully automated setup.

## Table of Contents
1. [Install Ubuntu](#install-ubuntu)
2. [Increase Swap Size](#increase-swap-size)
3. [Install DigiByte](#install-digibyte)
4. [Install Dependencies](#install-dependencies)
5. [Install VCPKG](#install-vcpkg)
6. [Install Standard C++ Dependencies](#install-standard-c-dependencies)
7. [Update CMAKE](#update-cmake)
8. [Install IPFS Desktop](#install-ipfs-desktop)
9. [Set IPFS to Run on Boot](#set-ipfs-to-run-on-boot)
10. [Build DigiAsset Core](#build-digiasset-core)
11. [Configure DigiAsset Core](#configure-digiasset-core)
12. [Set DigiAsset Core to Run at Boot](#set-digiasset-core-to-run-at-boot)
13. [Upgrading DigiAsset Core](#upgrading-digiasset-core)
14. [Other Notes](#other-notes)
15. [Special Thanks](#special-thanks)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


## Install Ubuntu

Ideally this should work on all OS. So far it has only been tested on:

- Ubuntu 20.04LTS - app works but google tests don't compile
- Ubuntu 22.04LTS - all functions

The instructions below are specifically writen for Ubuntu 22.04 LTS any other OS may have slightly different
instructions needed.

Install ubuntu server using default settings.

## Increase swap size

Default install had a 4GB swap file but DigiByte core kept crashing during sync so I increased it to 8GB

```bash
sudo swapoff /swap.img
sudo dd if=/dev/zero bs=1M count=8192 oflag=append conv=notrunc of=/swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
sudo swapon --show
sudo nano /etc/fstab
```

place the following at the end(if swap.img is already there replace it)

```
/swapfile       none    swap    sw      0       0
```

## Install DigiByte Core

```bash
wget https://github.com/digibyte/digibyte/releases/download/v7.17.3/digibyte-7.17.3-aarch64-linux-gnu.tar.gz
tar -xf digibyte-7.17.3-aarch64-linux-gnu.tar.gz
rm digibyte-7.17.3-aarch64-linux-gnu.tar.gz
mkdir .digibyte
nano .digibyte/digibyte.conf
```

```
rpcuser=user
rpcpassword=pass11
rpcbind=127.0.0.1
rpcport=14022
whitelist=127.0.0.1
rpcallowip=127.0.0.1
listen=1
server=1
txindex=1
addnode=191.81.59.115
addnode=175.45.182.173
addnode=45.76.235.153
addnode=24.74.186.115
addnode=24.101.88.154
addnode=8.214.25.169
addnode=47.75.38.245
```

to get digibyte to run on boot do the following

```bash
sudo nano /etc/systemd/system/digibyted.service
```

```
[Unit]
Description=DigiByte's distributed currency daemon
After=network.target

[Service]
User=<your-username>
Group=<your-username>

Type=forking
PIDFile=/home/<your-username>/.digibyte/digibyted.pid
ExecStart=/home/<your-username>/digibyte-7.17.3/bin/digibyted -daemon -pid=/home/<your-username>/.digibyte/digibyted.pid \
-conf=/home/<your-username>/.digibyte/digibyte.conf -datadir=/home/<your-username>/.digibyte -disablewallet

Restart=always
PrivateTmp=true
TimeoutStopSec=60s
TimeoutStartSec=2s
StartLimitInterval=120s
StartLimitBurst=5

[Install]
WantedBy=multi-user.target
```

replace <your-username>

Enable the service on boot

```bash
sudo systemctl enable digibyted.service
```

Start the service

```bash
sudo systemctl start digibyted.service
```

## Install Dependencies

```bash
sudo apt update
sudo apt upgrade
sudo apt-get install cmake libcurl4-openssl-dev libjsoncpp-dev golang-go libjsonrpccpp-dev libjsonrpccpp-tools libsqlite3-dev build-essential pkg-config zip unzip libssl-dev ninja-build
sudo apt install libboost-all-dev
```

## Update CMAKE

```bash
cd ~
sudo apt-get remove cmake
wget https://github.com/Kitware/CMake/releases/download/v3.27.7/cmake-3.29.0-linux-aarch64.sh
sudo mkdir /opt/cmake
sudo sh cmake-3.29.0-linux-aarch64.sh --prefix=/opt/cmake --skip-license
sudo ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
echo 'export PATH=/usr/local/cmake-3.29.0-linux-aarch64/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
rm cmake-3.29.0-linux-aarch64.sh
```

## Install VCPKG

```bash
wget -qO vcpkg.tar.gz https://github.com/microsoft/vcpkg/archive/master.tar.gz
sudo mkdir /opt/vcpkg
sudo tar xf vcpkg.tar.gz --strip-components=1 -C /opt/vcpkg
rm vcpkg.tar.gz
sudo /opt/vcpkg/bootstrap-vcpkg.sh
sudo ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg
```
Note: After compiling you may see the following message: 
Environment variable VCPKG_FORCE_SYSTEM_BINARIES must be set on arm, s390x, ppc64le and riscv platforms.

If so, enter the commands below:
```bash
echo 'export VCPKG_FORCE_SYSTEM_BINARIES=1' >> ~/.bashrc
echo 'VCPKG_FORCE_SYSTEM_BINARIES=1' | sudo tee -a /etc/environment > /dev/null
source ~/.bashrc
```

## Install Standard C++ Dependencies

Warning: The following steps build a lot of code and can take a long time to complete

```bash
sudo -E vcpkg install cryptopp
sudo mkdir /usr/local/include/cryptopp870
sudo cp /opt/vcpkg/packages/cryptopp_arm64-linux/include/cryptopp/* /usr/local/include/cryptopp870/
sudo -E vcpkg install sqlite3
sudo apt install libcrypto++-dev
```

## Install IPFS Desktop

```bash
wget https://dist.ipfs.tech/kubo/v0.22.0/kubo_v0.22.0_linux-arm64.tar.gz
tar -xvzf kubo_v0.22.0_linux-arm64.tar.gz
cd kubo
sudo bash install.sh
ipfs init
ipfs daemon
```

this step will list out a lot of data of importance is the line that says "RPC API server listening on" it is usually
port 5001 note it down if it is not. You can now see IPFS usage at localhost:5001/webui in your web browser(if not
headless).
Press Ctrl+C to stop the daemon

## Set IPFS to run on boot

```bash
cd ~
sudo nano /etc/systemd/system/ipfs.service
```

edit the file to look like this

```
[Unit]
Description=IPFS Daemon
After=network.target

[Service]
ExecStart=/usr/local/bin/ipfs daemon
User=<your-username>
Restart=always

[Install]
WantedBy=multi-user.target
```

replace <your-username>

```bash
sudo systemctl daemon-reload
sudo systemctl enable ipfs.service
sudo systemctl start ipfs.service
```

## Build DigiAsset Core

```bash
git clone -b master --recursive https://github.com/DigiAsset-Core/DigiAsset_Core.git
cd DigiAsset_Core
git submodule update --init --recursive
mkdir build
cd build
cmake -B . -S .. -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
mv src/digiasset_core ../bin
mv cli/digiasset_core-cli ../bin
cd ../bin
```

## Configure DigiAsset Core

```bash
nano config.cfg
```

Set your config settings in here. At minimum, you need

```
rpcuser=user
rpcpassword=pass11
```

for a full list of config options see example.cfg

---

## Set DigiAsset Core to run at boot

```bash
sudo nano /etc/systemd/system/digiasset_core.service
```

```
[Unit]
Description=DigiAsset Core
After=network.target digibyted.service

[Service]
User=<your-username>
Group=<your-username>

Type=simple
ExecStart=/home/<your-username>/DigiAsset_Core/bin/digiasset_core
WorkingDirectory=/home/<your-username>/DigiAsset_Core/bin

Restart=always
PrivateTmp=true
TimeoutStopSec=60s
TimeoutStartSec=2s
StartLimitInterval=120s
StartLimitBurst=5

[Install]
WantedBy=multi-user.target
```

replace <your-username>

Enable the service on boot

```bash
sudo systemctl enable digiasset_core.service
```

Start the service

```bash
sudo systemctl start digiasset_core.service
```

## Upgrading DigiAsset Core

When a new version is available you can upgrade by running the following commands
```bash
cd DigiAsset_Core/bin
./digiasset_core-cli shutdown
sudo systemctl stop digiasset_core.service
cd ..
git pull
git submodule update --init --recursive
cd build
cmake -B . -S .. -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
mv src/digiasset_core ../bin
mv cli/digiasset_core-cli ../bin
cd ../bin
sudo systemctl start digiasset_core.service
```


### Other Notes

- If submitting pull requests please utilize the .clang-format file to keep things standardized.


---

# Special Thanks

### Major Financial Support:

RevGenetics [Longevity Supplements](https://www.RevGenetics.com)