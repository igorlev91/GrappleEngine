#include "FileWatcher.h"

#include "GrapplePlatform/Windows/WindowsFileWatcher.h"

namespace Grapple
{
    FileWatcher* FileWatcher::Create(const std::filesystem::path& directoryPath, EventsMask eventsMask)
    {
#ifdef Grapple_PLATFORM_WINDOWS
        return new WindowsFileWatcher(directoryPath, eventsMask);
#endif
        return nullptr;
    }
}
