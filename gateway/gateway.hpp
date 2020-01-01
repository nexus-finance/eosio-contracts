#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/symbol.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>

#include "./interfaces/registry-interface.hpp"
#include "./interfaces/router-interface.hpp"

using namespace eosio;
using namespace std;

CONTRACT gateway : public contract {
public:
  using contract::contract;
  gateway(name receiver, name code, datastream<const char *> ds)
      : contract(receiver, code, ds), _config(receiver, receiver.value),
        _balance(receiver, receiver.value), _stats(receiver, receiver.value) {}

  struct [[eosio::table("config")]] config_s {
    name traderAccount = ""_n;
  };
  typedef eosio::singleton<"config"_n, config_s> config_t;

  struct balance_s {
    eosio::extended_asset balance;
  };
  typedef eosio::singleton<"balance"_n, balance_s> balance_t;

  struct [[eosio::table("stats")]]  stats_s {
    double totalVolume = 0;
  };
  typedef eosio::singleton<"stats"_n, stats_s> stats_t;

  ACTION init(const name &traderAccount);
  ACTION savebalance(const name &account, const eosio::extended_symbol &symbol);
  ACTION checkbalance(const name &account,
                      const eosio::extended_asset &minReturn);
  ACTION traderesult(const name &traderAccount,
                     const eosio::extended_asset &sent,
                     const std::vector<eosio::extended_asset> &received,
                     const eosio::extended_asset &minReturn,
                     const double &tradeValueInUsd, const double &eosUsdPrice);
  [[eosio::on_notify("*::transfer")]] void on_transfer(
      name from, name to, asset quantity, string memo);

  using savebalanceAction =
      eosio::action_wrapper<"savebalance"_n, &gateway::savebalance>;
  using checkbalanceAction =
      eosio::action_wrapper<"checkbalance"_n, &gateway::checkbalance>;

private:
  config_t _config;
  balance_t _balance;
  stats_t _stats;
};
