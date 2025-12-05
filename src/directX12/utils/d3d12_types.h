//
// Created by fightinghawks18 on 12/4/2025.
//

#pragma once

#include "common.h"
#include "../../graphics/utils/descriptors.h"

namespace Cocoa::D3D12
{
    inline D3D12_COMMAND_LIST_TYPE TranslateToD3D12CommandListType( const Graphics::GPUQueueType queueType )
    {
        switch (queueType)
        {
            case Graphics::GPUQueueType::Unknown:   return D3D12_COMMAND_LIST_TYPE_NONE;
            case Graphics::GPUQueueType::Graphics:  return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case Graphics::GPUQueueType::Compute:   return D3D12_COMMAND_LIST_TYPE_COMPUTE;
            case Graphics::GPUQueueType::Transfer:  return D3D12_COMMAND_LIST_TYPE_COPY;
        }
    }
}
