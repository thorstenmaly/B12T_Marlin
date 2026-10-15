# PlatformIO writes idedata.json before SCons creates build output directories.
# Marlin's normal build scripts are skipped for IDE integration dumps.
from pathlib import Path

Import("env")

if env.IsIntegrationDump():
    Path(env.subst("$BUILD_DIR")).mkdir(parents=True, exist_ok=True)
