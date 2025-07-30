#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <stdbool.h>
#include "game.h"

// Overlay configuration
#define OVERLAY_WINDOW_CLASS L"KBDTrainerOverlay"
#define OVERLAY_WINDOW_TITLE L"KBD Trainer Overlay"

// Overlay state
typedef struct {
    HWND target_window;
    HWND overlay_window;
    ID3D11Device* d3d_device;
    ID3D11DeviceContext* d3d_context;
    IDXGISwapChain* swap_chain;
    ID3D11RenderTargetView* render_target_view;
    bool is_active;
    bool show_overlay;
} OverlayState;

extern OverlayState g_overlay;

// Core overlay functions
bool InitializeOverlay(HWND target_window);
void ShutdownOverlay();
void RenderOverlay();
void UpdateOverlayPosition();

// DirectX hooking functions
bool InitializeDirectXHook();
void ShutdownDirectXHook();

// DLL injection functions
bool InjectIntoProcess(DWORD process_id, const wchar_t* dll_path);
DWORD FindProcessByName(const wchar_t* process_name);

// Input monitoring
void MonitorGameInputs();
bool IsKBDInput(int input_sequence[], int length);
bool IsWavedashInput(int input_sequence[], int length);
