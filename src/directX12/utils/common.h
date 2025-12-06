//
// Created by fightinghawks18 on 12/4/2025.
//

#pragma once

#include <comdef.h>
#include "../../macros.h"

#define THROW_IF_HR_FAILED(hr, ...) \
    do { \
        if ( FAILED( hr ) ) { \
            _com_error err( hr ); \
            THROW_IF_FAILED( true, "HRESULT failed with reason: %s \n %s", err.ErrorMessage() __VA_OPT__(,) __VA_ARGS__ ); \
        } \
    } while ( 0 )

#include <D3D12MemAlloc.h>
#include <dxgi1_4.h>
#include <directx/d3dx12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;