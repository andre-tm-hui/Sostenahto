/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   donateButton_png;
    const int            donateButton_pngSize = 9328;

    extern const char*   donatedAlreadyButton_png;
    const int            donatedAlreadyButton_pngSize = 8026;

    extern const char*   pedalDown_png;
    const int            pedalDown_pngSize = 264654;

    extern const char*   pedalUp_png;
    const int            pedalUp_pngSize = 283578;

    extern const char*   skipButton_png;
    const int            skipButton_pngSize = 2533;

    extern const char*   splashScreen_png;
    const int            splashScreen_pngSize = 26392;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
