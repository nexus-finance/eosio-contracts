#include "gateway.hpp"
#include "./utils/utils.hpp"
#include "interfaces/router-interface.hpp"
#include <eosio/time.hpp>

void gateway::init(const name &traderAccount) {
  require_auth(get_self());
  config_s config = _config.get_or_default();
  config.traderAccount = traderAccount;
  _config.set(config, get_self());
}

void gateway::on_transfer(name from, name to, asset quantity, string memo) {
  if (to != get_self())
    return;

  config_s config = _config.get_or_default();
  check(config.traderAccount.value != 0, "trader account not set");

  extended_asset fromAsset = extended_asset(quantity, get_first_receiver());
  extended_asset toMinReturn = parseMemo(memo);
  registry::check_symbol_supported(fromAsset.get_extended_symbol());
  registry::check_symbol_supported(toMinReturn.get_extended_symbol());

  // forward tokens to bank account
  token::transfer_action forward(fromAsset.contract, {get_self(), "active"_n});
  forward.send(get_self(), config.traderAccount, fromAsset.quantity, "");

  // record user's current from balance
  savebalanceAction saveBal(get_self(), {get_self(), "active"_n});
  saveBal.send(from, toMinReturn.get_extended_symbol());

  // perform the trade through the dex router contract
  router::tradeAction trade(router::ROUTER_ACCOUNT, {get_self(), "active"_n});
  trade.send(from, config.traderAccount, fromAsset, toMinReturn);

  // check if user received at least toMinReturn funds
  checkbalanceAction checkBal(get_self(), {get_self(), "active"_n});
  checkBal.send(from, toMinReturn);
}

void gateway::savebalance(const name &account,
                          const eosio::extended_symbol &symbol) {
  require_auth(get_self());
  check(!_balance.exists(), "balance exists but shouldn't");
  _balance.set({.balance = getBalanceForAccount(account, symbol)}, get_self());
}

void gateway::checkbalance(const name &account,
                           const eosio::extended_asset &minReturn) {
  require_auth(get_self());
  check(_balance.exists(), "balance should exist but doesn't");
  auto balanceRow = _balance.get();

  auto actualBalance =
      getBalanceForAccount(account, minReturn.get_extended_symbol());
  auto expectedBalance = balanceRow.balance + minReturn;

  if (actualBalance < expectedBalance) {
    asset diff = (expectedBalance - actualBalance).quantity;
    check(false, "below min return by " + diff.to_string());
  }

  _balance.remove();
}

// called by router contract
void gateway::traderesult(const name &traderAccount,
                          const eosio::extended_asset &sent,
                          const std::vector<eosio::extended_asset> &received,
                          const eosio::extended_asset &minReturn,
                          const double &tradeValueInUsd,
                          const double &eosUsdPrice) {
  require_auth(router::ROUTER_ACCOUNT);

  stats_s stats = _stats.get_or_default();
  stats.totalVolume += tradeValueInUsd;
  _stats.set(stats, get_self());
}