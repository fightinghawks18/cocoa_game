#include <vector>
#include <optional>
#include <cstdint>

namespace Cocoa::Tools {
    template <typename T>
    struct Handle {
        uint32_t index;
        uint32_t generation;
    };

    template<typename H, typename T>
    class ResourceManager {
    public:
        ResourceManager(size_t resourceCount) {
            _data.reserve(resourceCount);
        }

        H Add(T resource) {
            if (!_freeList.empty()) {
                uint32_t idx = _freeList.back();
                _freeList.pop_back();
                _data[idx] = std::move(resource);
                return {idx, _generations[idx]};
            }
            uint32_t idx = static_cast<uint32_t>(_data.size());
            _data.push_back(std::move(resource));
            _generations.push_back(0);
            return {idx, 0};
        }

        template <typename... Args>
        requires std::is_constructible_v<T, Args...>
        H Emplace(Args&&... args) {
            if (!_freeList.empty()) {
                uint32_t idx = _freeList.back();
                _freeList.pop_back();
                _data[idx].emplace(std::forward<Args>(args)...);
                return {idx, _generations[idx]};
            }
            uint32_t idx = static_cast<uint32_t>(_data.size());
            _data.emplace_back(std::in_place, std::forward<Args>(args)...);
            _generations.push_back(0);
            return {idx, 0};
        }
    
        void Remove(H h) {
            if (!Valid(h)) return;
            _data[h.index].reset();
            _generations[h.index]++;
            _freeList.push_back(h.index);
        }
    
        T* Get(H h) {
            if (!Valid(h)) return nullptr;
            return &(*_data[h.index]);
        }
    
        bool Valid(H h) const {
            return h.index < _data.size() 
                && _generations[h.index] == h.generation
                && _data[h.index].has_value();
        }
    
    private:
        std::vector<std::optional<T>> _data;
        std::vector<uint32_t> _generations;
        std::vector<uint32_t> _freeList;
    };
}