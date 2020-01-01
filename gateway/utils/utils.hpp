#pragma once

#include "../interfaces/eosio.token.hpp"
#include <cmath>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

eosio::extended_asset
getBalanceForAccount(const eosio::name &account,
                     const eosio::extended_symbol symbol) {
  return {eosio::token::get_balance_or_default(symbol.get_contract(), account,
                                               symbol.get_symbol())
              .amount,
          symbol};
}

vector<string> split(const string &str, const string &delim) {
  vector<string> tokens;
  size_t start = 0, found = 0;
  do {
    found = str.find(delim, start);
    if (found == string::npos)
      found = str.length();
    string token = str.substr(start, found - start);
    tokens.push_back(token);
    start = found + delim.length();
  } while (found < str.length() && start < str.length());

  return tokens;
}

eosio::extended_asset parseMemo(const string &memo) {
  // memo example: 4,EOS@eosio.token;1046912750
  check(memo.length() > 0 && memo.length() <= 50, "memo exceeds max size");
  vector<string> parts = split(memo, ";");

  check(parts.size() == 2,
        "invalid memo - expected pattern: '4,EOS@eosio.token;10000'");

  vector<string> extSymbolParts = split(parts[0], "@");
  check(extSymbolParts.size() == 2,
        "invalid symbol - expected pattern: 4,EOS@eosio.token");

  vector<string> symbolParts = split(extSymbolParts[0], ",");
  check(symbolParts.size() == 2,
        "invalid symbol code - expected pattern: 4,EOS@eosio.token");

  // parse symbol
  std::string &precision = symbolParts[0];
  check((precision.size() == 1 || precision.size() == 2) &&
            std::isdigit(precision[0]) &&
            (precision.size() != 2 || isdigit(precision[1])),
        "precision in symbol is not a number: " + precision);
  const eosio::symbol sym = symbol(symbolParts[1], stoull(precision));
  check(sym.is_valid(), "memo contains invalid symbol");

  const eosio::name contract = name(extSymbolParts[1]);
  for (auto &c : parts[1]) {
    check(isdigit(c), "expected return is not a number: " + parts[1]);
  }
  int64_t expectedReturn = stoull(parts[1]);

  return extended_asset(asset(expectedReturn, sym), contract);
}