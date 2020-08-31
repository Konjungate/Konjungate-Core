# Konjungate Core v1.0.1.3

Name: Konjungate<br/>
Ticker: KONJ<br/>
Type: Hybrid, POS / POW / Masternodes<br/>
Algorithm: bmw512<br/>
Total Coin Supply: 21 billion<br/>

Rewards per Block:
250 KONJ / BLOCK (shared as follow)

POW 62.5 KONJ decreasing 10% every 6 month
POS 48.75 KONJ after 6 month increasing to 68.75 KONJ after this: decreasing 15% every 6 month 
MN. 113,75 KONJ after 6 month increasing to 153.75 KONJ after this: increasing 17% every 6

Masternode Collateral: 2750000 KONJ

Charity fee (taken from POW): 25 KONJ to support contemporary ART.<br/>
Please visit https://www.machfeld-foundation.net to see our supported Projects.

Official Website:<br/>
https://www.konjungate.net

## What is Konjungate?
KONJUNGATE - The Artbased Blockchain To change the funding of Artists and Institutions. Based on the cyber novel MACHFELD by Michael Mastrototaro © 1999

Konjungate will chance the funding of contemporary Art and Art-Institutions using blockchain technology to change the way of contemporary art funding.

See our projects at:<br/>
https://wendy.network<br/>
https://www.machfeld-foundation.net<br/>

KONJUNGATE is based on the cyber novel MACHFELD written 1999 by Michael Mastrototaro. Strangely, the main character of the book is called Nakomoto and thus resembles the pseudonym of the BITCOIN inventor Satoshi Nakomoto. What a coincidence, right? Anyway. In his cyber novel, Michael Mastrototaro describes 9 years before Bitcoin appears on the cryptocurrency horizon the mining of KONJUNGATE. Now 20 years after publishing the novel online, KONJUNGATE appears as a Media Art project to decentralize the funding of international contemporary Artists and Institutions and to give a strong artistic statement in times of budget cuts.

## LINKS

Official Website:<br/>
https://www.konjungate.net/

Official Mining Pool:<br/>
https://pool.konjungate.net/<br/>
https://ahamay.net/

Block Explorer:
http://explore.konjungate.net/<br/>

Bitcoin Talk announcment:
https://bitcointalk.org/index.php?topic=5148524.msg51263942#msg51263942<br/>

---------------->>>-----<<<<<<---------------------

## (So called) Social Media Links:

Telegram: https://t.me/joinchat/CZxxC03HTpvdGDWlXg0tgQ 

Facebook: https://www.facebook.com/konjungate/ 

Twitter: https://twitter.com/konjungate

YouTube: https://www.youtube.com/channel/UCnm-eA9EGNwSHu3KKzkm8Zg

---------------->>>-----<<<<<<---------------------

## EXCHANGES

CREX24:
https://crex24.com/exchange/KONJ-BTC<br/>
https://crex24.com/exchange/KONJ-ETH



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
Boost1.68, OR Boost1.58,  
Openssl1.02r,
Berkeley DB 6.2.32,
QT5.12.1,
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

```cd ~; wget http://download.oracle.com/berkeley-db/db-6.2.32.NC.tar.gz; tar zxf db-6.2.32.NC.tar.gz; cd db-6.2.32.NC/build_unix; ../dist/configure --enable-cxx --disable-shared; make; sudo make install; sudo ln -s /usr/local/BerkeleyDB.6.2/lib/libdb-6.2.so /usr/lib/libdb-6.2.so; sudo ln -s /usr/local/BerkeleyDB.6.2/lib/libdb_cxx-6.2.so /usr/lib/libdb_cxx-6.2.so; export BDB_INCLUDE_PATH="/usr/local/BerkeleyDB.6.2/include"; export BDB_LIB_PATH="/usr/local/BerkeleyDB.6.2/lib"```


## GitHub pull (Source Download)

```cd ~; git clone https://github.com/Konjungate-Core Konjungate```


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


## Create config file for daemon
```cd ~; sudo ufw allow 19417/tcp; sudo ufw allow 18495/tcp; sudo ufw allow 22/tcp; sudo mkdir ~/.KONJ; cat << "CONFIG" >> ~/.KONJ/Konjungate.conf
listen=1
server=1
daemon=1
testnet=0
rpcuser=KONJrpcuser
rpcpassword=SomeCrazyVeryVerySecurePasswordHere
rpcport=18495
port=19417
rpcconnect=127.0.0.1
rpcallowip=127.0.0.1
addnode=37.187.180.53
addnode=139.99.239.62
addnode=192.99.212.20
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
