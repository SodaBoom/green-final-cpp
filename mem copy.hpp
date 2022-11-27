#include <string>
#include <atomic>
<<<<<<< HEAD
#include <utility>
#include <vector>
#include <unordered_map>

#define EMPTY 1
#define COLLECTED_BY_OTHER 2
#define ALL_COLLECTED 3

class MemTotalEnergy {
public:
    MemTotalEnergy(std::string userId, int totalEnergy) {
        userId_ = std::move(userId);
        totalEnergy_ = totalEnergy;
    }

    std::string userId_;
    std::atomic_uint32_t totalEnergy_{};

    MemTotalEnergy() = default;
};

class MemToCollect {
public:
    std::string user_id_;
    std::atomic_uint8_t status_{};//0:EMPTY, 1:COLLECTED_BY_OTHER, 2:ALL_COLLECTED;
    std::atomic_uint32_t total_energy_{};
    bool modified_{};

    MemToCollect(std::string user_id, int total_energy) {
        user_id_ = std::move(user_id);
        total_energy_ = total_energy;
        status_ = 0;
        modified_ = false;
    }

    MemToCollect() = default;
};

MemToCollect *memToCollects[100 * 10000 + 1] = {nullptr};
MemTotalEnergy *memTotalEnergies[10 * 1000 + 1] = {nullptr};
std::unordered_map<std::string, int> memTotalEnergyMap;
=======
#include <vector>
#include <unordered_map>

enum Status {
    EMPTY, 
    COLLECTED_BY_OTHER, 
    ALL_COLLECTED
};

class MemTotalEnergy {
  public:
    MemTotalEnergy() = default;
    MemTotalEnergy(std::string userId, int totalEnergy) {
        userId_ = userId;
        totalEnergy_ = totalEnergy;
    }
    
    std::string userId_;
    std::atomic<int> totalEnergy_;
};

class MemToCollect {
  public:
    MemToCollect() = default;
    MemToCollect(std::string user_id, int total_energy) {
        user_id_ = user_id;
        total_energy_ = total_energy;
        status_ = Status::EMPTY;
        modified_ = false;
    }

    std::string user_id_;
    Status status_;
    int total_energy_;
    bool modified_;
};
>>>>>>> 添加sql相关的逻辑