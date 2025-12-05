//
// Created by fightinghawks18 on 12/4/2025.
//

#pragma once

#include "../../macros.h"

#define THROW_IF_HR_FAILED(hr, ...) \
    if ( FAILED(hr) ) THROW_IF_FAILED( true __VA_OPT__(,) __VA_ARGS__ )

#include <directx/d3dx12.h>
#include <D3D12MemAlloc.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;