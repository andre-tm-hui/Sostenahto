/*
  ==============================================================================

    DeviceIDGetter.h
    Created: 31 Mar 2023 3:05:54pm
    Author:  andre

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Salt.h"

// Include system-specific headers
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/utsname.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#endif

std::string getSystemId();