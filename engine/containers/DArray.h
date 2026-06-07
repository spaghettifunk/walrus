#pragma once

#include <engine/Defines.h>
#include <engine/core/Logger.h>
#include <engine/core/Memory.h>

#include <new>
#include <utility>

namespace Walrus
{
    template<typename T>
    class DArray
    {
    public:
        static constexpr u64 DefaultCapacity = 1;
        static constexpr u64 ResizeFactor = 2;

        DArray() = default;

        explicit DArray(u64 capacity)
        {
            Reserve(capacity);
        }

        DArray(const DArray& other)
        {
            Reserve(other.m_Length);
            for (u64 i = 0; i < other.m_Length; ++i)
            {
                ConstructAt(i, other.m_Data[i]);
            }
            m_Length = other.m_Length;
        }

        DArray(DArray&& other) noexcept
            : m_Data(other.m_Data)
            , m_Length(other.m_Length)
            , m_Capacity(other.m_Capacity)
        {
            other.m_Data = nullptr;
            other.m_Length = 0;
            other.m_Capacity = 0;
        }

        DArray& operator=(const DArray& other)
        {
            if (this == &other)
            {
                return *this;
            }

            DArray copy(other);
            Swap(copy);
            return *this;
        }

        DArray& operator=(DArray&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            DestroyStorage();
            m_Data = other.m_Data;
            m_Length = other.m_Length;
            m_Capacity = other.m_Capacity;

            other.m_Data = nullptr;
            other.m_Length = 0;
            other.m_Capacity = 0;
            return *this;
        }

        ~DArray()
        {
            DestroyStorage();
        }

        void PushBack(const T& value)
        {
            EnsureCapacityForOneMore();
            ConstructAt(m_Length, value);
            ++m_Length;
        }

        void PushBack(T&& value)
        {
            EnsureCapacityForOneMore();
            ConstructAt(m_Length, std::move(value));
            ++m_Length;
        }

        bool PopBack(T* outValue = nullptr)
        {
            if (m_Length == 0)
            {
                WERROR("Cannot pop from an empty DArray.");
                return false;
            }

            const u64 index = m_Length - 1;
            if (outValue)
            {
                *outValue = std::move(m_Data[index]);
            }

            m_Data[index].~T();
            --m_Length;
            return true;
        }

        bool InsertAt(u64 index, const T& value)
        {
            return InsertAtImpl(index, value);
        }

        bool InsertAt(u64 index, T&& value)
        {
            return InsertAtImpl(index, std::move(value));
        }

        bool RemoveAt(u64 index, T* outValue = nullptr)
        {
            if (index >= m_Length)
            {
                WERROR("Index outside the bounds of this DArray! Length: {}, index: {}", m_Length, index);
                return false;
            }

            if (outValue)
            {
                *outValue = std::move(m_Data[index]);
            }

            m_Data[index].~T();
            for (u64 i = index; i + 1 < m_Length; ++i)
            {
                ConstructAt(i, std::move(m_Data[i + 1]));
                m_Data[i + 1].~T();
            }

            --m_Length;
            return true;
        }

        void Clear()
        {
            DestroyElements();
            m_Length = 0;
        }

        void Reserve(u64 capacity)
        {
            if (capacity <= m_Capacity)
            {
                return;
            }

            T* newData = Allocate(capacity);
            for (u64 i = 0; i < m_Length; ++i)
            {
                new (newData + i) T(std::move(m_Data[i]));
                m_Data[i].~T();
            }

            FreeStorage();
            m_Data = newData;
            m_Capacity = capacity;
        }

        T& operator[](u64 index)
        {
            return m_Data[index];
        }

        const T& operator[](u64 index) const
        {
            return m_Data[index];
        }

        T* Data()
        {
            return m_Data;
        }

        const T* Data() const
        {
            return m_Data;
        }

        u64 Length() const
        {
            return m_Length;
        }

        u64 Capacity() const
        {
            return m_Capacity;
        }

        bool IsEmpty() const
        {
            return m_Length == 0;
        }

        void Swap(DArray& other) noexcept
        {
            std::swap(m_Data, other.m_Data);
            std::swap(m_Length, other.m_Length);
            std::swap(m_Capacity, other.m_Capacity);
        }

    private:
        template<typename U>
        bool InsertAtImpl(u64 index, U&& value)
        {
            if (index > m_Length)
            {
                WERROR("Index outside the bounds of this DArray! Length: {}, index: {}", m_Length, index);
                return false;
            }

            EnsureCapacityForOneMore();

            for (u64 i = m_Length; i > index; --i)
            {
                ConstructAt(i, std::move(m_Data[i - 1]));
                m_Data[i - 1].~T();
            }

            ConstructAt(index, std::forward<U>(value));
            ++m_Length;
            return true;
        }

        void EnsureCapacityForOneMore()
        {
            if (m_Length < m_Capacity)
            {
                return;
            }

            Reserve(m_Capacity == 0 ? DefaultCapacity : m_Capacity * ResizeFactor);
        }

        template<typename... Args>
        void ConstructAt(u64 index, Args&&... args)
        {
            new (m_Data + index) T(std::forward<Args>(args)...);
        }

        static T* Allocate(u64 capacity)
        {
            return static_cast<T*>(Memory::Allocate(sizeof(T) * capacity, MemoryTag::MEMORY_TAG_DARRAY));
        }

        void DestroyStorage()
        {
            DestroyElements();
            FreeStorage();
            m_Length = 0;
            m_Capacity = 0;
        }

        void DestroyElements()
        {
            for (u64 i = 0; i < m_Length; ++i)
            {
                m_Data[i].~T();
            }
        }

        void FreeStorage()
        {
            if (!m_Data)
            {
                return;
            }

            Memory::Free(m_Data, sizeof(T) * m_Capacity, MemoryTag::MEMORY_TAG_DARRAY);
            m_Data = nullptr;
        }

        T* m_Data = nullptr;
        u64 m_Length = 0;
        u64 m_Capacity = 0;
    };
} // namespace Walrus
