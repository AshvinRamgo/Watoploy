#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include "observer.h"

class Subject {
protected:
    std::vector<Observer*> observers;

public:
    virtual void attach(Observer* observer);
    virtual void notifyObservers() const;
    virtual ~Subject() = default;
};

#endif
