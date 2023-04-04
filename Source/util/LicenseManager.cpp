/*
  ==============================================================================

    LicenseManager.cpp
    Created: 3 Apr 2023 5:46:48pm
    Author:  andre

  ==============================================================================
*/

#include "LicenseManager.h"

void LicenseManager::saveLicense(std::string licenseKey) {
    File file = File::getSpecialLocation(File::commonApplicationDataDirectory).getChildFile(appName + "/" + fileName);
    if (!file.exists()) {
        file.create();
    }
    file.replaceWithText(licenseKey);
}

std::string LicenseManager::loadLicense() {
    File file = File::getSpecialLocation(File::commonApplicationDataDirectory).getChildFile(appName + "/" + fileName);
    if (!file.exists()) {
        return "";
    }
    StringArray out;
    file.readLines(out);
    if (out.size() <= 0) {
        return "";
    }
    else {
        return out[0].toStdString();
    }
}