[![compile](https://github.com/nexus-finance/eosio-contracts/workflows/compile/badge.svg)](https://github.com/nexus-finance/eosio-contracts/actions?query=workflow%3Acompile)

# Nexus Finance Smart Contracts

## Compilation

The smart contracts have been built with EOSIO.CDT 1.7

```bash
eosio-cpp gateway/gateway.cpp -o gateway.wasm
```


## Verifying the code hash

The latest smart contract code is automatically compiled by GitHub.
The resulting smart contracts and [code hash are uploaded](https://github.com/nexus-finance/eosio-contracts/actions?query=workflow%3Acompile).

The code hash can be compared to the one on chain using cleos:

```bash
cleos -u https://api.eosn.io get code nxsgatewayxx
# code hash: ...
```
