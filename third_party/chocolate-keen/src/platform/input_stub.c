// input_stub.c: Implements input stub for the platform subsystem.

#include "platform/platform.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

extern const char *kw_arcadeDataPath, *kw_arcadeSavePath;

void CK_PlatformPrepareInput(void) {
}

bool CK_PlatformPollInputState(CK_PlatformInputState_T *state) {
    (void)state;
    return false;
}

void CK_PlatformApplyInputPolicy(const CK_PlatformInputState_T *state, bool isWaitingForCharInput) {
    (void)state;
    (void)isWaitingForCharInput;
}

uint16_t CK_PlatformPreferredPlayer1ControlType(uint16_t currentControlType, int numOfJoysticks) {
    (void)numOfJoysticks;
    return currentControlType;
}

void CK_PlatformApplyDefaultJoystickCalibration(int16_t joystickCtrl[4][3]) {
    (void)joystickCtrl;
}

void CK_PlatformEnsureRwMiscDir(void) {
}

bool CK_PlatformBuildRoDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    const char *base = strrchr(relativePath, '/');
    base = base ? base + 1 : relativePath;
    // Classic archives vary in filename case on Linux. Resolve within the
    // one supplied game folder; never search another episode's data.
    DIR *dir = opendir(kw_arcadeDataPath);
    if (!dir) return false;
    struct dirent *entry;
    bool found = false;
    while ((entry = readdir(dir))) {
        const char *a = base, *b = entry->d_name;
        while (*a && *b && toupper((unsigned char)*a) == toupper((unsigned char)*b)) { ++a; ++b; }
        if (*a || *b) continue;
        int written = snprintf(outPath, outPathSize, "%s/%s", kw_arcadeDataPath, entry->d_name);
        found = written > 0 && (size_t)written < outPathSize;
        break;
    }
    closedir(dir);
    return found;
}

bool CK_PlatformBuildExternalGameDataPath(const char *relativePath, char *outPath, size_t outPathSize) {
    (void)relativePath;
    (void)outPath;
    (void)outPathSize;
    return false;
}

bool CK_PlatformBuildRwMiscPath(const char *filename, char *outPath, size_t outPathSize) {
    int written = snprintf(outPath, outPathSize, "%s/%s", kw_arcadeSavePath, filename);
    return (written > 0) && ((size_t)written < outPathSize);
}

bool CK_PlatformShouldAutoShowLauncher(int availableEpisodes) {
    (void)availableEpisodes;
    return false;
}
