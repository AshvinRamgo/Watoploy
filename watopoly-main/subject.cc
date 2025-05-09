#include "subject.h"
#include "observer.h"

void Subject::attach(Observer* observer) {
    observers.push_back(observer);
}

void Subject::notifyObservers() const {
    for (auto observer : observers) {
        observer->notify(const_cast<Subject*>(this)); 
    }
}
