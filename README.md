# Homomorphism-Encryption-KV-Store-Client

## Install
Make sure you have the latest version of gcc and cmake before proceeding

### Make sure you clone all the submodules (SEAL and rpclib)
```
git clone --recurse-submodules https://github.com/laiKwunSing/Homomorphism-Encryption-KV-Store-Client.git
```

### Comile and build the submodules (Skip this if you have done this in the server repo)
```
cd SEAL && cmake . && make -j && sudo make install
```
Then, go back to the root directory of the repository and execute the following command
```
cd rpclib && mkdir build && cd build 
cmake .. && make -j && sudo make install
```

### Compile and build
Again, go back to the root directory and execute the following commands
```
mkdir build && cd build
cmake .. && make -j
```

## Execution
You can execute thoses 4 different clients in different terminals.

```
cd build
./HE_kv_store_client Gamestop 111111
./HE_kv_store_client Walmart 222222
./HE_kv_store_client Bestbuy 333333
./HE_kv_store_client Target 444444
```

## Operations

#### set Product_Name(string) Amount(int)
Example: 

```
set NintendoSwitch 100
```

#### avg Product_Name(string)
Example: 

```
avg NintendoSwitch
```
