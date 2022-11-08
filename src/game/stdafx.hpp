#pragma once

//System
#include <cstdio>
#include <string>
#include <Windows.h>

//Deps
#include "MinHook.h"

#define print(__FMT__, ...) std::printf(__FMT__, __VA_ARGS__);
#define println(__FMT__, ...) print(__FMT__ "\n", __VA_ARGS__);