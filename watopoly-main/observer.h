#ifndef OBSERVER_H
#define OBSERVER_H

// observer.h
class Subject;

class Observer {
public:
    virtual void notify(const Subject* whoNotified) = 0;
    virtual ~Observer() = default;
};

#endif
