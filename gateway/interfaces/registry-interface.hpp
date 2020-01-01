#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

namespace registry {
static constexpr eosio::name REGISTRY_ACCOUNT = eosio::name("nxsregistryx");

struct [[eosio::table("tokens"), eosio::contract("nxsregistryx")]] tokens_s {
  uint64_t id;
  eosio::extended_symbol token;

  uint64_t primary_key() const { return id; }
  uint64_t by_symbolraw() const { return token.get_symbol().raw(); }
};
typedef eosio::multi_index<
    "tokens"_n, tokens_s,
    eosio::indexed_by<
        "bysymbolraw"_n,
        eosio::const_mem_fun<tokens_s, uint64_t, &tokens_s::by_symbolraw>>>
    tokens_t;

tokens_t tokens_table = tokens_t(REGISTRY_ACCOUNT, REGISTRY_ACCOUNT.value);

void check_symbol_supported(const eosio::extended_symbol &token) {
  auto tokens_by_sym = registry::tokens_table.get_index<eosio::name("bysymbolraw")>();
  bool found = false;
  auto key = token.get_symbol().raw();
  auto itr = tokens_by_sym.lower_bound(key);
  while (itr != tokens_by_sym.upper_bound(key)) {
    if (itr->token.get_contract() == token.get_contract()) {
      found = true;
      break;
    }
    itr++;
  }

  eosio::check(found, "symbol " + token.get_symbol().code().to_string() + "@" + token.get_contract().to_string() +  " not supported");
}
}; // namespace registry
