#define BARE_BUILD_VERSION "0.239"
#define BARE_VCS_REVISION "unknown"
extern const char bare_build_version[];
extern const char bare_vcs_revision[];
extern const char build_version[];
const char bare_build_version[] = BARE_BUILD_VERSION;
const char bare_vcs_revision[] = BARE_VCS_REVISION;
const char build_version[] = BARE_BUILD_VERSION " (" BARE_VCS_REVISION ")";
