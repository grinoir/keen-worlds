// chocolate-keen.c: Implements chocolate-keen for the app subsystem.

#include "engine/engine_api.h"

// Keen Worlds: data and saves are separate explicit arguments. The paths
// are private to this process; they never become shell commands.
const char *kw_arcadeDataPath;
const char *kw_arcadeSavePath;

int main(int argc, char **argv) {
    if (argc < 4) return 2;
    kw_arcadeDataPath = argv[1];
    kw_arcadeSavePath = argv[2];
    argc -= 2;
    argv += 2; // argv[0] is ignored by the original option parser.
    if (!CVort_engine_processArguments(argc, argv))
        return 0;
    /* NOTE: Relevant to an earlier C++ variant but is still worthy to mention.
     * An instance of the CVorticons class (i.e. the game) is a quite large
     * object to initialize, so having it on the stack is the path for
     * RANDOM PROBLEMS!!! (Memory related errors...)
     * Furthermore, we wish to apply polymorphism.
     *
     * Not so relevant with the migration to C code, though.
     */
    if (!CVort_engine_start()) {
        return 1;
    }
    CVort_engine_shutdown();
}
