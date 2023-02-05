#ifndef PLATFORMS_HPP
#define PLATFORMS_HPP

#include <cstdint>

enum class Platforms {
    MacOs = 0,
    Linux = 1,
};

// Define the current platform based on which platform macros exist.
//
// Using C++ constants is generally cleaner than using macros everywhere.
// For example, if we define some code conditionally for macOS:
// * With macros:
//
//   #if defined(__APPLE__) || defined(__MACH__)
//   mac_os_specific_method(1);
//   #else
//   linux_specific_method(2);
//   #endif
//
// * With C++ constants
//
//   if constexpr (Platforms::MacOs == cCurrentPlatform) {
//       mac_os_specific_method(1);
//   } else {
//       linux_specific_method(2);
//   }
//
//   * When using C++ constants, besides being more readable, both branches of
//     the condition are syntax-checked.
#if defined(__APPLE__) || defined(__MACH__)
constexpr Platform cCurrentPlatform = Platform::MacOs;
#else
constexpr Platforms cCurrentPlatform = Platforms::Linux;
#endif

#endif // PLATFORMS_HPP
