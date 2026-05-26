#pragma once

#include "Event.h"

namespace logic {

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onNotify(const Event& event) = 0;
};

}  // namespace logic
