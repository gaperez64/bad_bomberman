#pragma once

#include <vector>
#include "Event.h"
#include "Observer.h"

namespace logic {

class Subject {
public:
    virtual ~Subject() = default;

    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);

protected:
    void notify(const Event& event);

private:
    std::vector<Observer*> m_observers;
};

}  // namespace logic
