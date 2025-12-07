#pragma once

#include <optional>
#include <vector>
#include "../../common.h"
#include "../../macros.h"

namespace Cocoa::Graphics
{
    struct Handle
    {
        uint64_t id = u64Max;

        Handle() : id( 0 ) {}
        explicit Handle( const u64 id ) : id( id ) {}

        [[nodiscard]] bool IsValid() const { return id != u64Max; }
        void Invalidate() { id = u64Max; }
    };

    template<typename T>
    struct ResourceSlot
    {
        explicit ResourceSlot( uint64_t id ) { this->id = id; }
        bool active = false;
        std::optional<T> resource;
        uint64_t id;
    };

    class IResourceManager
    {
        public:
            virtual ~IResourceManager() = default;
    };

    template<typename T>
    class ResourceManager final : public IResourceManager
    {
        public:
            static u32 GetHandleGeneration( const u64 id )
            {
                return static_cast<u32>( id >> 32 );
            }

            static u32 GetHandleIndex( const u64 id )
            {
                return static_cast<u32>( id );
            }

            explicit ResourceManager( usize poolSize = 1000 )
            {
                _slots.reserve( poolSize );
            }

            ~ResourceManager() override = default;

            template<typename ... Args>
            Handle Create( Args&& ... args )
            {
                usize index;
                if ( _freedList.empty() )
                {
                    index   = _slots.size();
                    auto id = CreateHandleID( 0 , index );
                    _slots.emplace_back( id );
                } else
                {
                    index = _freedList.back();
                    _freedList.pop_back();
                }

                ResourceSlot<T>& slot = _slots[ index ];
                slot.resource.emplace( std::forward<Args>( args ) ... );
                slot.active = true;
                return Handle( slot.id );
            }

            void Destroy( Handle& handle )
            {
                if ( !ValidateHandle( handle ) )
                {
                    handle.Invalidate();
                    return;
                }

                ResourceSlot<T>& slot = _slots[ GetHandleIndex( handle.id ) ];
                slot.resource.reset();
                slot.active = false;

                auto newGen = GetHandleGeneration( slot.id ) + 1;
                slot.id     = CreateHandleID( newGen , GetHandleIndex( slot.id ) );
                handle.Invalidate();
            }

            T* Get( Handle& handle )
            {
                if ( !ValidateHandle( handle ) )
                {
                    handle.Invalidate();
                    return nullptr;
                }

                ResourceSlot<T>& slot = _slots[ GetHandleIndex( handle.id ) ];
                return &*slot.resource;
            }
        private:
            std::vector<ResourceSlot<T>> _slots;
            std::vector<size_t> _freedList;

            bool ValidateHandle( Handle& handle )
            {
                if ( !handle.IsValid() ) return false;
                auto idx = GetHandleIndex( handle.id );
                if ( idx >= _slots.size() ) return false;
                return _slots[ idx ].id == handle.id;
            }

            static u64 CreateHandleID( const u32 generation, const u32 index )
            {
                return static_cast<u64>( generation ) << 32 | index;
            }
    };
}
