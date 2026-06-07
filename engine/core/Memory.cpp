#include <engine/core/Logger.h>
#include <engine/core/Memory.h>

#include <array>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace Walrus
{
    void* PlatformAllocate(u64 size, bool aligned);
    void PlatformFree(void* block, bool aligned);
    void* PlatformZeroMemory(void* block, u64 size);
    void* PlatformCopyMemory(void* dest, const void* source, u64 size);
    void* PlatformSetMemory(void* dest, i32 value, u64 size);

    static constexpr u64 MemoryTagCount = static_cast<u64>(MemoryTag::MEMORY_TAG_MAX_TAGS);

    static constexpr u64 ToIndex(MemoryTag tag)
    {
        return static_cast<u64>(tag);
    }

    struct MemoryStats
    {
        u64 TotalAllocated = 0;
        std::array<u64, MemoryTagCount> TaggedAllocations = {};
    };

    static constexpr std::array<std::string_view, MemoryTagCount> MemoryTagStrings = {
        "UNKNOWN    ", "ARRAY      ", "DARRAY     ", "DICT       ", "RING_QUEUE ", "BST        ",
        "STRING     ", "APPLICATION", "JOB        ", "TEXTURE    ", "MAT_INST   ", "RENDERER   ",
        "GAME       ", "TRANSFORM  ", "ENTITY     ", "ENTITY_NODE", "SCENE      ",
    };

    static MemoryStats stats;

    void Memory::Initialize()
    {
        PlatformZeroMemory(&stats, sizeof(stats));
    }

    void Memory::Shutdown()
    {
    }

    void* Memory::Allocate(u64 size, MemoryTag tag)
    {
        if (tag == MemoryTag::MEMORY_TAG_UNKNOWN)
        {
            WWARN("wallocate called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
        }

        stats.TotalAllocated += size;
        stats.TaggedAllocations[ToIndex(tag)] += size;

        // TODO: Memory alignment
        void* block = PlatformAllocate(size, false);
        PlatformZeroMemory(block, size);
        return block;
    }

    void Memory::Free(void* block, u64 size, MemoryTag tag)
    {
        if (tag == MemoryTag::MEMORY_TAG_UNKNOWN)
        {
            WWARN("wfree called using MEMORY_TAG_UNKNOWN. Re-class this allocation.");
        }

        stats.TotalAllocated -= size;
        stats.TaggedAllocations[ToIndex(tag)] -= size;

        // TODO: Memory alignment
        PlatformFree(block, false);
    }

    void* Memory::ZeroMemory(void* block, u64 size)
    {
        return PlatformZeroMemory(block, size);
    }

    void* Memory::CopyMemory(void* dest, const void* source, u64 size)
    {
        return PlatformCopyMemory(dest, source, size);
    }

    void* Memory::SetMemory(void* dest, i32 value, u64 size)
    {
        return PlatformSetMemory(dest, value, size);
    }

    std::string Memory::GetMemoryUsageStr()
    {
        const u64 gib = 1024 * 1024 * 1024;
        const u64 mib = 1024 * 1024;
        const u64 kib = 1024;

        char buffer[8000] = "System memory use (tagged):\n";
        u64 offset = strlen(buffer);
        for (u64 i = 0; i < MemoryTagCount; ++i)
        {
            char unit[4] = "XiB";
            float amount = 1.0f;
            if (stats.TaggedAllocations[i] >= gib)
            {
                unit[0] = 'G';
                amount = stats.TaggedAllocations[i] / (float)gib;
            }
            else if (stats.TaggedAllocations[i] >= mib)
            {
                unit[0] = 'M';
                amount = stats.TaggedAllocations[i] / (float)mib;
            }
            else if (stats.TaggedAllocations[i] >= kib)
            {
                unit[0] = 'K';
                amount = stats.TaggedAllocations[i] / (float)kib;
            }
            else
            {
                unit[0] = 'B';
                unit[1] = 0;
                amount = (float)stats.TaggedAllocations[i];
            }

            i32 length = snprintf(buffer + offset, sizeof(buffer) - offset, "  %s: %.2f%s\n", MemoryTagStrings[i].data(), amount, unit);
            offset += length;
        }
        return std::string(buffer);
    }
} // namespace Walrus
