#include <engine/Defines.h>

// Windows platform layer.
#if WPLATFORM_WINDOWS

#include <cstdlib>
#include <cstring>

namespace Walrus
{
    void* PlatformAllocate(u64 size, bool aligned)
    {
        (void)aligned;
        return malloc(size);
    }

    void PlatformFree(void* block, bool aligned)
    {
        (void)aligned;
        free(block);
    }

    void* PlatformZeroMemory(void* block, u64 size)
    {
        return memset(block, 0, size);
    }

    void* PlatformCopyMemory(void* dest, const void* source, u64 size)
    {
        return memcpy(dest, source, size);
    }

    void* PlatformSetMemory(void* dest, i32 value, u64 size)
    {
        return memset(dest, value, size);
    }
} // namespace Walrus

#endif
