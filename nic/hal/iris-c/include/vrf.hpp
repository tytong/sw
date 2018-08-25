
#ifndef __VRF_HPP__
#define __VRF_HPP__

#include <vector>

#include "sdk/indexer.hpp"

#include "hal.hpp"


class Vrf : public HalObject
{
public:
  static Vrf *Factory();
  static void Destroy(Vrf *vrf);


  uint64_t GetId();
  uint64_t GetHandle();

  static void Probe();

private:
  Vrf();
  ~Vrf();

  uint32_t id;
  uint64_t handle;

  // For vrf id
  static sdk::lib::indexer *allocator;
  static constexpr uint64_t max_vrfs = 8;
};

#endif
