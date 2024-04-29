
#include "Mora.h"

#include <json/json.hpp>

namespace our {
    void Mora::deserialize(const nlohmann::json &data) {
        if(!data.is_object()) return;
        name = data.value("name", "test_mora");
    }
} // our