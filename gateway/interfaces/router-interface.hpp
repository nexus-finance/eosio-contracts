#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

namespace router {
static constexpr eosio::name ROUTER_ACCOUNT = eosio::name("nxsdexrouter");

class [[eosio::contract("nxsdexrouter")]] router : public eosio::contract {
  public:
  void trade(const eosio::name &user, const eosio::name &trader,
             const eosio::extended_asset &fromAsset,
             const eosio::extended_asset &toMinReturn);
};

using tradeAction = eosio::action_wrapper<"trade"_n, &router::trade>;
}; // namespace router
