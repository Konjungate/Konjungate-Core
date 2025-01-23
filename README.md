# Konjungate Core v1.1.6.3

Name: Konjungate<br/>
Ticker: KONJ<br/>
Type: Hybrid, POS / Masternodes --> POW ends on Block 700000<br/>
Algorithm: bmw512<br/>
Total Coin Supply: 21 billion<br/>
BlockSize (depending on block saturation): 1.525612 MB min - 15.256128 MB max<br/>
BlockTime: ~ 30 sec.

Rewards per Block:
Dynamic to Block 6800888 (see image below)
From Block 6800889: 245 KONJ / BLOCK (shared as follow)<br/><br/>

Dynamic reward structure.<br/><br/>

<img width="964" alt="Screen Shot 2021-06-13 at 22 44 53" src="https://user-images.githubusercontent.com/55807677/121821413-0cb23400-cc99-11eb-8be6-591b71cc9133.png">

Masternode Collateral: 2750000 KONJ


Charity fee: 25 KONJ to support contemporary ART.<br/>
Please visit https://wendy.network to see our supported Projects.

Official Website:<br/>
https://www.konjungate.net

## What is Konjungate?

Shaping Legacy
–
for the networked society of the future.

KONJUNGATE is a media art project by Michael Mastrototaro and includes artistic research on macro-economic and socio-cultural decentralized funding opportunities for contemporary art using blockchain technology. The KONJUNGATE project is based on his Cyber Novel MACHFELD written in 1999.

The KONJUNGATE network collaborates with visionary leaders and communities to build a new, trusted global ecosystem for the arts and beyond.

Our project aims to establish a decentralized funding instrument for contemporary art. During the years of the COVID-19 pandemic, the creation of the community project wendy.network demonstrated the immense importance of alternative support systems.

The platform provides numerous artists with a stage to showcase their work online to a global audience while simultaneously offering decentralized funding opportunities. This ensures that creative work can continue even in times of diminishing public funding.

### Demi-Nodes
Our network now operates by using "Demi-nodes" to help the wallet make informed decisions on how to treat a peer in the network or even other nodes that aren't trusted. Demi-nodes are a list of trusted nodes a user can define inside of the wallet. These user-defined trusted nodes then can be queried for specific data such as asking the Demi-node network wether or not a reorganization request from another peer is a valid one or something that should be rejected and then banned off the network to protect other peers. An adaptive self cleaning network as this continiously defends itself from any possible intrusion or attack while still keeping decentralization as the underlying focus by allowing users to define their own lists. This feature compliments the Velocity security system which goes beyond other blockchain's security methods to ensure no possibility of malformed blocks making it onto the chain even with something like a 51% attack.

Official Website:<br/>
https://www.konjungate.net/

Block Explorer:
https://chainz.cryptoid.info/konj/<br><br>

## Social Media:<br/>
Discord: https://discord.gg/K7hAfXg 

X: https://x.com/konjungate

YouTube: https://www.youtube.com/channel/UCnm-eA9EGNwSHu3KKzkm8Zg

Instagram: https://www.instagram.com/konjungate/

Reddit: https://www.reddit.com/user/KONJUNGATE/


---------------->>>-----<<<<<<---------------------

## EXCHANGES

Comming Soon! 

﻿

---------------->>>-----<<<<<<---------------------

### BVAC (Bits Visualized As Color)
BVAC is a unique system that we developed and created in house just for KONJ, Espers and other associated projects. This offers us the ability to store ANY data as a PNG or JPG, similarly to a QR code, with only three files being required as apposed to three entire libraries that QR codes require and the data storage is denser. If you would like to learn more about this feature feel free to reach out to CryptoCoderz or SaltineChips. The current proof of concept implementation is the ability to store and read a public receiving address as a 16x16 BVAC image. Users can share their public keys this way by simply sending each other the BVAC image of the pubkey created from the wallet and then the receiving part is able to load the image using the wallet and decode it into the pubkey once again.

## Blockchain Technology
The Konjungate [KONJ] Blockchain is an experimental smart contract platform protocol that enables instant payments to anyone, anywhere in the world in a private, secure manner. Konjungate [KONJ] uses peer-to-peer blockchain technology developed by Konjungate to operate with no central authority: managing transactions, execution of contracts, and issuing money are carried out collectively by the network. Konjungate [KONJ] is the name of open source software which enables the use of this protocol.

## Custom Difficulty Retarget Algorithm “VRX”
VRX is designed from the ground up to integrate properly with the Velocity parameter enforcement system to ensure users no longer receive orphan blocks.

## Velocity Block Constraint System
Ensuring Insane stays as secure and robust as possible the CryptoCoderz team have implemented what's known as the Velocity block constraint system. This system acts as third and final check for both mined and peer-accepted blocks ensuring that all parameters are strictly enforced.
Wish (bmw512) Proof-of-Work Algorithm

## Wish or bmw512 hashing algorithm
is utilized for the Proof-of-Work function and also replaces much of the underlying codebase hashing functions as well that normally are SHA256. By doing so this codebase is able to be both exponentially lighter and more secure in comparison to reference implementations.

## Specifications and General info

Konjungate uses

```libsecp256k1,
libgmp,
Boost1.74  
Openssl1.02u,
Berkeley DB 6.2.38,
QT5.15.2,
to compile
```

## BUILD LINUX
Compiling Konjungate "SatoshiCore" daemon on Ubunutu 18.04 LTS Bionic
Note: guide should be compatible with other Ubuntu versions from 14.04+

## Become poweruser

```sudo -i```


## CREATE SWAP FILE FOR DAEMON BUILD (if system has less than 2GB of RAM)

```cd ~; sudo fallocate -l 3G /swapfile; ls -lh /swapfile; sudo chmod 600 /swapfile; ls -lh /swapfile; sudo mkswap /swapfile; sudo swapon /swapfile; sudo swapon --show; sudo cp /etc/fstab /etc/fstab.bak; echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab```

## Dependencies install

```cd ~; sudo apt-get install -y ntp git build-essential libssl-dev libdb-dev libdb++-dev libboost-all-dev libqrencode-dev libcurl4-openssl-dev curl libzip-dev; apt-get update -y; apt-get install -y git make automake build-essential libboost-all-dev; apt-get install -y yasm binutils libcurl4-openssl-dev openssl libssl-dev; sudo apt-get install -y libgmp-dev; sudo apt-get install -y libtool;```


## Dependencies build and link

```cd ~; wget http://download.oracle.com/berkeley-db/db-6.2.32.NC.tar.gz; tar zxf db-6.2.32.NC.tar.gz; cd db-6.2.32.NC/build_unix; ../dist/configure --enable-cxx --disable-shared; make; sudo make install; sudo ln -s /usr/local/BerkeleyDB.6.2/lib/libdb-6.2.so /usr/lib/libdb-6.2.so; sudo ln -s /usr/local/BerkeleyDB.6.2/lib/libdb_cxx-6.2.so /usr/lib/libdb_cxx-6.2.so; export BDB_INCLUDE_PATH="/usr/local/BerkeleyDB.6.2/include"; export BDB_LIB_PATH="/usr/local/BerkeleyDB.6.2/lib"; cd ~;```


## GitHub pull (Source Download)

```git clone https://github.com/Konjungate/Konjungate-Core Konjungate```


## Build Konjungate daemon

```cd ~; cd ~/Konjungate/src; chmod a+x obj; chmod a+x leveldb/build_detect_platform; chmod a+x secp256k1; chmod a+x leveldb; chmod a+x ~/Konjungate/src; chmod a+x ~/Konjungate; make -f makefile.unix USE_UPNP=-; cd ~; cp -r ~/Konjungate/src/Konjungated /usr/local/bin/Konjungated;```


## (Optional) Build Konjungate-QT (GUI wallet) on Linux

All previous steps must be completed first.

If you recompiling some other time you don't have to repeat previous steps, but need to define those variables. Skip this command if this is your first build and previous steps were performed in current terminal session.

```export BDB_INCLUDE_PATH="/usr/local/BerkeleyDB.6.2/include"; export BDB_LIB_PATH="/usr/local/BerkeleyDB.6.2/lib"```


## With UPNP:

```cd ~; cd ~/Konjungate; qmake -qt=qt5; make```


## Recommended Without UPNP:

```cd ~; cd ~/Konjungate; qmake -qt=qt5 USE_UPNP=-; make```


## Create config file for daemo

```
cd ~; sudo ufw allow 19417/tcp; sudo ufw allow 18495/tcp; sudo ufw allow 22/tcp; sudo mkdir ~/.KONJ; cat << "CONFIG" >> ~/.KONJ/Konjungate.conf
listen=1
server=1
staking=1
maxconnections=150
rpcuser=yourusername
rpcpassword=SomeCrazyVeryVerySecurePasswordHere
port=19417
rpcport=18495
addnode=134.122.90.36
addnode=134.122.90.36:19417
addnode=188.166.108.110
addnode=188.166.108.110:19417
CONFIG
chmod 700 ~/.KONJ/Konjungate.conf; chmod 700 ~/.KONJ; ls -la ~/.KONJ
```

## Run Konjungate daemon

```cd ~; Konjungated; Konjungated getinfo```


## Troubleshooting
for basic troubleshooting run the following commands when compiling:
this is for minupnpc errors compiling

```make clean -f makefile.unix USE_UPNP=-
make -f makefile.unix USE_UPNP=-
```

## Updating daemon in bin directory

```cd ~; cp -r ~/Konjungate/src/Konjungated /usr/local/bin```


## License

Konjungate Core is released under the terms of the MIT license. See COPYING for more information or see https://opensource.org/licenses/MIT.

## Development Process

The master branch is meant to be stable. Development is normally done in separate branches. Tags are created to indicate new official, stable release versions of Konjungate Core.

The contribution workflow is described in CONTRIBUTING.md.

## Testing

Testing and code review is the bottleneck for development; we get more pull requests than we can review and test on short notice. Please be patient and help out by testing other people's pull requests, and remember this is a security-critical project where any mistake might cost people lots of money.

## Automated Testing

Developers are strongly encouraged to write unit tests for new code, and to submit new unit tests for old code. Unit tests can be compiled and run (assuming they weren't disabled in configure) with: make check

There are also regression and integration tests of the RPC interface, written in Python, that are run automatically on the build server. These tests can be run (if the test dependencies are installed) with: qa/pull-tester/rpc-tests.py

The Travis CI system makes sure that every pull request is built for Windows and Linux, OS X, and that unit and sanity tests are automatically run.

## Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the code. This is especially important for large or high-risk changes. It is useful to add a test plan to the pull request description if testing the changes is not straightforward.
