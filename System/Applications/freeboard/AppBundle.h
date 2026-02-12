/*
 * AppBundle.h
 * PocketDarwin freeboard - app bundle info for SpringBoard clone
 */

#ifndef FREEBOARD_APPBUNDLE_H
#define FREEBOARD_APPBUNDLE_H

#include <string>
#include <vector>

namespace freeboard {

struct AppInfo {
    std::string displayName;
    std::string bundlePath;
    std::string executablePath;
};

/** Scan /Applications for .app bundles; return list of AppInfo. */
std::vector<AppInfo> scanApplications(const std::string& applicationsPath = "/Library/CoreServices");

} // namespace freeboard

#endif /* FREEBOARD_APPBUNDLE_H */
