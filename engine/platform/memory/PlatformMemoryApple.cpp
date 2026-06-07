#include <engine/Defines.h>

#if WPLATFORM_APPLE

#include <cstdlib>
#include <cstring>

namespace Walrus
{
    void* PlatformAllocate(u64 size, bool)
    {
        return std::malloc(size);
    }

    void PlatformFree(void* block, bool)
    {
        std::free(block);
    }

    void* PlatformZeroMemory(void* block, u64 size)
    {
        return std::memset(block, 0, size);
    }

    void* PlatformCopyMemory(void* dest, const void* source, u64 size)
    {
        return std::memcpy(dest, source, size);
    }

    void* PlatformSetMemory(void* dest, i32 value, u64 size)
    {
        return std::memset(dest, value, size);
    }
} // namespace Walrus

#endif
