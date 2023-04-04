/*
  ==============================================================================

    LicenseManager.h
    Created: 3 Apr 2023 5:46:48pm
    Author:  andre

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

const std::string appName = "Sustenato Pedal";
const std::string fileName = "license.cfg";

namespace LicenseManager {
    void saveLicense(std::string);
    std::string loadLicense();
}

