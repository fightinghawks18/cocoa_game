#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

namespace Cocoa::Tools {
    template <typename T>
    struct Handle {
        uint64_t id = UINT64_MAX;
        
        [[nodiscard]] bool IsValid() { return id != UINT64_MAX; }
        void Invalidate() { id = UINT64_MAX; }
    };

    template <typename T>
    class ResourceManager {
        struct Slot {
            uint32_t generation = 0;
            bool occupied = false;
            union Data {
                char dummy;
                T resource;
                uint32_t nextFreeIndex;

                Data() : dummy{} {}
                ~Data() {}
            } data;

            Slot() {
                data.nextFreeIndex = UINT32_MAX;
            }

            ~Slot() {
                if (occupied) {
                    data.resource.~T();
                }
            }

            Slot(const Slot&) = delete;
            Slot& operator=(const Slot&) = delete;

            Slot(Slot&& other) noexcept 
                : generation(other.generation), occupied(other.occupied), data() 
            {
                if (occupied) {
                    new (&data.resource) T(std::move(other.data.resource));
                    other.data.resource.~T();
                    other.occupied = false;
                } else {
                    data.nextFreeIndex = other.data.nextFreeIndex;
                }
            }

            Slot& operator=(Slot&& other) noexcept {
                if (this != &other) {
                    if (occupied) {
                        data.resource.~T();
                    }
                    generation = other.generation;
                    occupied = other.occupied;
                    if (occupied) {
                        new (&data.resource) T(std::move(other.data.resource));
                        other.data.resource.~T();
                        other.occupied = false;
                    } else {
                        data.nextFreeIndex = other.data.nextFreeIndex;
                    }
                }
                return *this;
            }
        };
    public:
        ResourceManager(size_t reserveSize = 1024) {
            _slots.reserve(reserveSize);
        }
        ~ResourceManager() = default;
        
        template <typename... Args>
        requires std::is_constructible_v<T, Args...>
        Handle<T> Create(Args&&... args) {
            uint32_t slotIndex = UINT32_MAX;
            if (_baseFreedIndex != UINT32_MAX) {
                slotIndex = _baseFreedIndex;
                _baseFreedIndex = _slots[slotIndex].data.nextFreeIndex;
            } else {
                slotIndex = _slots.size();
                _slots.emplace_back();
            }

            Slot& slot = _slots[slotIndex];
            new (&slot.data.resource) T(std::forward<Args>(args)...);
            slot.occupied = true;
            return {
                .id = CreateID(slotIndex, slot.generation)
            };
        }

        void Free(Handle<T> handle) {
            if (!handle.IsValid()) return;
            auto index = GetIndexFromID(handle.id);
            if (index > _slots.size() - 1) return;
            if (GetGenerationFromID(handle.id) != _slots[index].generation) return;

            Slot& slot = _slots[index];
            slot.data.resource.~T();
            slot.occupied = false;
            slot.generation++;
            slot.data.nextFreeIndex = _baseFreedIndex;
            _baseFreedIndex = index;
        }

        T* Get(Handle<T> handle) {
            if (!handle.IsValid()) return nullptr;
            auto index = GetIndexFromID(handle.id);
            if (index > _slots.size() - 1) return nullptr;
            if (GetGenerationFromID(handle.id) != _slots[index].generation) return nullptr;
            
            Slot& slot = _slots[index];
            if (!slot.occupied) return nullptr;
            return &slot.data.resource;
        }
    private:
        std::vector<Slot> _slots;
        uint32_t _baseFreedIndex = UINT32_MAX;

        uint64_t CreateID(uint32_t index, uint32_t generation) {
            return static_cast<uint64_t>(generation) << 32 | index;
        }

        uint32_t GetIndexFromID(uint64_t id) {
            return id & 0xFFFFFFFF;
        }

        uint32_t GetGenerationFromID(uint64_t id) {
            return id >> 32;
        }
    };
}