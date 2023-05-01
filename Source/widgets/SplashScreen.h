/*
  ==============================================================================

    SplashScreen.h
    Created: 31 Mar 2023 2:55:45pm
    Author:  andre

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "util/DeviceIDGetter.h"

//==============================================================================
/*
*/
class DonateSplashScreen  : public juce::Component
{
public:
    DonateSplashScreen(SustainPedalAudioProcessor& audioProcessor) : audioProcessor(audioProcessor)
    {
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        deviceId = getSystemId();
        if (preLaunchCheck()) {
            audioProcessor.start();
            delete this;
            return;
        }

        setSize(400, 680);
        bg = ImageCache::getFromMemory(BinaryData::splashScreen_png, BinaryData::splashScreen_pngSize);
        donateImg = ImageCache::getFromMemory(BinaryData::donateButton_png, BinaryData::donateButton_pngSize);
        generateImg = ImageCache::getFromMemory(BinaryData::donatedAlreadyButton_png, BinaryData::donatedAlreadyButton_pngSize);
        skipImg = ImageCache::getFromMemory(BinaryData::skipButton_png, BinaryData::skipButton_pngSize);
        
        addAndMakeVisible(donate);
        donate.setBounds(40, 168, 320, 106);
        donate.setImages(
            true, true, true, 
            donateImg, 1.0, Colour(0x00000000),
            donateImg, 1.0, Colour(0x40FFFFFF), 
            donateImg, 1.0, Colour(0x00000000), 
            0
        );
        donate.onClick = [this] { URL("https://www.buymeacoffee.com/ahdio/e/126269").launchInDefaultBrowser(); };

        addAndMakeVisible(generate);
        generate.setBounds(40, 360, 320, 100);
        generate.setImages(
            true, true, true,
            generateImg, 1.0, Colour(0x00000000),
            generateImg, 1.0, Colour(0x40FFFFFF),
            generateImg, 1.0, Colour(0x00000000),
            0
        );
        generate.onClick = [this] { URL("https://andre-tm-hui.github.io/ahdio/sostenahto/license-manager").launchInDefaultBrowser(); };

        addAndMakeVisible(skip);
        skip.setBounds(130, 591, 140, 48);
        skip.setImages(
            true, true, true,
            skipImg, 1.0, Colour(0x00000000),
            skipImg, 1.0, Colour(0x40FFFFFF),
            skipImg, 1.0, Colour(0x00000000),
            0
        );
        skip.onClick = [this] { this->audioProcessor.start(); delete this; };

        addAndMakeVisible(reg);
        reg.setBounds(150, 542, 100, 24);
        reg.setButtonText("REGISTER");
        reg.onClick = [this] { registerLicenseKey(); };

        addAndMakeVisible(deviceIdButton);
        deviceIdButton.setBounds(46, 511, 140, 26);
        deviceIdButton.setButtonText(deviceId);
        deviceIdButton.onClick = [this] { SystemClipboard::copyTextToClipboard(deviceId); addAndMakeVisible(copied); };
        copied.setBounds(46, 539, 140, 12);
        copied.setText("Copied!", dontSendNotification);
        copied.setFont(Font(12));

        addAndMakeVisible(licenseKeyEditor);
        licenseKeyEditor.setBounds(206, 511, 140, 26);
        addAndMakeVisible(licenseErr);
        licenseErr.setBounds(206, 539, 140, 12);
        licenseErr.setText("", dontSendNotification);
        licenseErr.setFont(Font(12));
    }

    ~DonateSplashScreen() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.drawImageWithin(bg, 0, 0, 400, 680, RectanglePlacement::Flags::stretchToFit); // draw some placeholder text
    }

    void resized() override
    {
        // This method is where you should set the bounds of any child
        // components that your component contains..

    }

private:
    String getResultText(const URL& url)
    {
        StringPairArray responseHeaders;
        int statusCode = 0;

        if (auto stream = url.createInputStream(URL::InputStreamOptions(URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(10000)
            .withResponseHeaders(&responseHeaders)
            .withStatusCode(&statusCode)))
        {
            return (statusCode != 0 ? "Status code: " + String(statusCode) + newLine : String())
                + "Response headers: " + newLine
                + responseHeaders.getDescription() + newLine
                + "----------------------------------------------------" + newLine
                + stream->readEntireStreamAsString();
        }

        if (statusCode != 0)
            return "Failed to connect, status code = " + String(statusCode);

        return "Failed to connect!";
    }

    bool preLaunchCheck() {
        std::string licenseKey = audioProcessor.getLicenseKey();
        if (licenseKey == "") return false;
        
        std::stringstream ss(getResultText(
            URL(
                "https://us-central1-sustenato-verification.cloudfunctions.net/app/verify?deviceId=" +
                deviceId +
                "&licenseKey=" +
                licenseKey
            )).toStdString()
        );
        std::string segment;
        std::vector<std::string> seglist;
        while (std::getline(ss, segment, '\n'))
            seglist.push_back(segment);

        if (seglist[0].find("200") >= 0 && segment == "false") {
            audioProcessor.setLicenseKey("");
            return false;
        }

        return true;
        
    }

    void registerLicenseKey() {
        String licenseKey = licenseKeyEditor.getText();
        if (licenseKey.length() != 15) {
            licenseErr.setText("Wrong key length", dontSendNotification);
            return;
        }
        
        String res = getResultText(URL("https://us-central1-sustenato-verification.cloudfunctions.net/app/verify?deviceId=" + deviceId + "&licenseKey=" + licenseKey.toStdString()));
        std::stringstream ss(getResultText(
            URL(
                "https://us-central1-sustenato-verification.cloudfunctions.net/app/verify?deviceId=" + 
                deviceId + 
                "&licenseKey=" + 
                licenseKey.toStdString()
            )).toStdString()
        );
        std::string segment;
        std::vector<std::string> seglist;
        while (std::getline(ss, segment, '\n'))
            seglist.push_back(segment);
        
        if (seglist[0].find("200") >= 0) {
            if (segment == "true") {
                audioProcessor.setLicenseKey(licenseKey.toStdString());
                audioProcessor.start();
                delete this;
                return;
            }
            else {
                licenseErr.setText("Invalid key", dontSendNotification);
            }
        }
        else if (seglist[0].find("400") >= 0) {
            licenseErr.setText("Enter a key", dontSendNotification);
        }
        else {
            licenseErr.setText("Network error", dontSendNotification);
        }
    }

    SustainPedalAudioProcessor& audioProcessor;
    ImageButton donate, generate, skip;
    TextButton deviceIdButton, reg;
    Label copied, licenseErr;
    TextEditor licenseKeyEditor;
    Image bg, donateImg, generateImg, skipImg;

    std::string deviceId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DonateSplashScreen)
};
