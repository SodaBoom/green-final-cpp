#include <string>
#include <atomic>
#include <utility>
#include <vector>
#include <unordered_map>

#define EMPTY 0
#define COLLECTED_BY_OTHER 1
#define ALL_COLLECTED 2

//class MemTotalEnergy {
//public:
//    MemTotalEnergy(std::string userId, int totalEnergy) {
//        userId_ = std::move(userId);
//        totalEnergy_ = totalEnergy;
//    }
//
//    std::string userId_;
//    std::atomic_uint32_t totalEnergy_{};
//
//    MemTotalEnergy() = default;
//};

class MemToCollect {
public:
    std::string user_id_;
    std::uint8_t status_ = EMPTY;//0:EMPTY, 1:COLLECTED_BY_OTHER, 2:ALL_COLLECTED;
    std::uint32_t to_collect_energy_;
    bool modified_ = false;
    std::mutex mutex;
};

MemToCollect *memToCollects[100 * 10000 + 1] = {nullptr};
std::unordered_map<std::string, std::atomic_uint32_t> memTotalEnergyMap;
