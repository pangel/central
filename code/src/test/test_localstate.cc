#include <SFML/Graphics.hpp>
#include "main.h"
#include "test.h"
#include "HScenario.h"
#include "localState.h"
#include "geography.h"
#include "tilemap.h"
#define DEBUG false
#include "debug.h"

namespace test {
  int localstate() {
    std::string seed = "424242";
    auto nbPlayers = 1;
    Geography geo = Generation1(seed);
    LOG(debug) << "address of geo " << &geo;
    LocalState loc = LocalState(&geo,nbPlayers, 1);
    LOG(debug) << "address of map in LocalState " << loc.getMap();
    LOG(info) << "Local State testing done.";

    return 0;
  }
}
