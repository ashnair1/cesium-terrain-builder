#ifndef GDAL_COMPAT_HPP
#define GDAL_COMPAT_HPP

/*
 * GDAL Compatibility Header for Cesium Terrain Builder
 * 
 * This header provides compatibility macros and functions to handle
 * differences between GDAL versions, particularly 3.10+ changes.
 * 
 * Usage: Include this header in source files that use GDAL APIs
 */

#include <gdal.h>
#include <gdal_version.h>

// Only include C++ specific features if C++ headers are available
#ifdef __cplusplus
  // Forward declare GDALDataset if not already included
  #ifndef GDAL_PRIV_H_INCLUDED
    class GDALDataset;
  #else
    #include <gdal_priv.h>
  #endif
#endif

// Version detection macros
#define CTB_GDAL_VERSION_MAJOR GDAL_VERSION_MAJOR
#define CTB_GDAL_VERSION_MINOR GDAL_VERSION_MINOR
#define CTB_GDAL_VERSION_NUM GDAL_VERSION_NUM

// GDAL 3.11+ specific compatibility checks
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3,11,0)
  #define CTB_GDAL_HAS_GRID_PREFIX 1
  #define CTB_GDAL_HAS_NEW_COORDINATE_TRANSFORM 1
#else
  #define CTB_GDAL_HAS_GRID_PREFIX 0
  #define CTB_GDAL_HAS_NEW_COORDINATE_TRANSFORM 0
#endif

// GDAL 3.10+ specific compatibility checks
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3,10,0)
  #define CTB_GDAL_HAS_IMPROVED_WARPER 1
#else
  #define CTB_GDAL_HAS_IMPROVED_WARPER 0
#endif

// GDAL 3.9+ C++17 requirement check
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3,9,0) && __cplusplus < 201703L
  #error "GDAL 3.9+ requires C++17. Please compile with -std=c++17 or later."
#endif

// Forward compatibility warnings for future versions
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(4,0,0)
  #pragma message("GDAL 4.x detected. Manual compatibility review may be required.")
#endif

// Compatibility function declarations
namespace ctb {
  namespace gdal_compat {
    
    /**
     * Get GDAL version as a formatted string
     * @return GDAL version string (e.g., "3.11.0")
     */
    inline const char* getVersionString() {
      return GDALVersionInfo("RELEASE_NAME");
    }
    
    /**
     * Check if current GDAL version supports a specific feature
     * @param major Major version number
     * @param minor Minor version number
     * @return true if current version >= specified version
     */
    inline bool hasVersionAtLeast(int major, int minor) {
      return GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(major, minor, 0);
    }
    
    /**
     * Get compatibility warning message for unsupported versions
     * @return Warning message or nullptr if compatible
     */
    inline const char* getCompatibilityWarning() {
      #if GDAL_VERSION_NUM < GDAL_COMPUTE_VERSION(3,1,0)
        return "Warning: GDAL version is older than recommended minimum 3.1.0";
      #elif GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(4,0,0)
        return "Warning: GDAL 4.x detected - compatibility review may be needed";
      #else
        return nullptr;
      #endif
    }
  }
}

// Compatibility macros for common GDAL operations
#ifdef __cplusplus
  #ifdef GDAL_PRIV_H_INCLUDED
    // Use RAII for GDAL dataset management (only when gdal_priv.h is included)
    #include <memory>
    
    namespace ctb {
      namespace gdal_compat {
        // Smart pointer type for GDAL datasets
        using GDALDatasetPtr = std::unique_ptr<GDALDataset, decltype(&GDALClose)>;
        
        /**
         * Create a smart pointer for GDAL dataset with automatic cleanup
         */
        inline GDALDatasetPtr makeDatasetPtr(GDALDataset* dataset) {
          return GDALDatasetPtr(dataset, &GDALClose);
        }
      }
    }
  #endif
#endif

#endif /* GDAL_COMPAT_HPP */