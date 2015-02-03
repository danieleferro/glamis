#include "CentralManager.h"
#include <Arduino.h>

#define DEBUG                1

#ifdef DEBUG
#define dbg(fmt, args...)         printf("CentralManager: "fmt, ## args)
#else
#define dbg(fmt, args...)
#endif

CentralManager::CentralManager(void)
{
}

CentralManager::~CentralManager(void)
{
}

