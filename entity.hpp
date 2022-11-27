#include <string>

class ToCollectEnergy {
  public:
    ToCollectEnergy() = default;
    int id;
    std::string userId;
    int toCollectEnergy;
    std::string status;
};

class TotalEnergy {
 public:
    TotalEnergy() = default;
    int id;
    std::string userId;
    int totalEnergy;
};
