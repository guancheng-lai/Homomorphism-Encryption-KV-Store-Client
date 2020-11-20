# Homomorphism-Encryption-KV-Store-Client

## Install

### Make sure you clone all the submodules (SEAL and rpclib)
```
git clone --recurse-submodules https://github.com/laiKwunSing/Homomorphism-Encryption-KV-Store-Client.git
```

### Compile and build
```
mkdir build && cd build
cmake .. && make -j
```

## Execution
You can execute thoses 4 different clients in different terminals.

```
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

#### get Product_Name(string)
Example: 

```
get NintendoSwitch
```
