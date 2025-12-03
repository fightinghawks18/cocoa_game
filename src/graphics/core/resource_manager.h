#pragma once

#include <cstdint>
#include <vector>

namespace Cocoa::Graphics {
    template <typename T>
    struct Handle {
        uint64_t id = UINT64_MAX;

        [[nodiscard]] bool IsValid() { return id != UINT64_MAX; }
        void Invalidate() { id = UINT64_MAX; }
    };

    template <typename T>
    struct ResourceSlot {
        bool active = false;
        T resource;
        uint64_t id;
    };

    class IGFXResourceManager {
    public:
        virtual ~IGFXResourceManager() = default;
    };

    template <typename T>
    class GFXResourceManager : public IGFXResourceManager {
    public:
        GFXResourceManager(size_t poolSize) {
            _slots.reserve(poolSize);
        }
        ~GFXResourceManager() = default;

        template <typename... Args>
        Handle<T> Create(Args&&... args) {
            size_t index;
            if (_freedList.empty()) {
                index = _slots.size();
                _slots.push_back({
                    .id = CreateHandleID(0, index)
                });
            } else {
                index = _freedList.back();
                _freedList.pop_back();
            }

            ResourceSlot<T>& slot = _slots[index];
            new (&slot.resource) T(std::forward<Args>(args)...);
            slot.active = true;
            return {slot.id};
        }

        void Destroy(Handle<T>& handle) {
            if (!ValidateHandle(handle)) {
                handle.Invalidate();
                return;
            }

            ResourceSlot<T>& slot = _slots[GetHandleIndex(handle.id)];
            slot.resource.~T();
            slot.active = false;

            auto newGen = GetHandleGeneration(slot.id) + 1;
            slot.id = CreateHandleID(newGen, GetHandleIndex(slot.id));
            handle.Invalidate();
        }

        T* Get(Handle<T>& handle) {
            if (!ValidateHandle(handle)) {
                handle.Invalidate();
                return;
            }

            ResourceSlot<T>& slot = _slots[GetHandleIndex(handle.id)];
            return &(*slot.resource);
        }
    private:
        std::vector<ResourceSlot<T>> _slots;
        std::vector<size_t> _freedList;

        bool ValidateHandle(Handle<T>& handle) {
            if (!handle.IsValid()) return false;
            auto idx = GetHandleIndex(handle.id);
            if (idx >= _slots.size()) return false;
            return _slots[idx].id != handle.id;
        }

        uint64_t CreateHandleID(uint32_t generation, uint32_t index) {
            return (static_cast<uint64_t>(generation) << 32) | index;
        }

        uint32_t GetHandleGeneration(uint64_t id) {
            return (static_cast<uint32_t>(id >> 32));
        }

        uint32_t GetHandleIndex(uint64_t id) {
            return static_cast<uint32_t>(id);
        }
    };
}