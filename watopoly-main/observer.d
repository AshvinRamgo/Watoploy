observer.o gcm.cache/observer.gcm: observer.cc
observer.c++m: gcm.cache/observer.gcm
.PHONY: observer.c++m
gcm.cache/observer.gcm:| observer.o
