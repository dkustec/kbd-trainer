#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <MinHook.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Forward declarations - avoid including overlay.h to prevent duplicate symbols
extern void RenderOverlay();
extern void MonitorGameInputs();
extern struct OverlayState {
    HWND target_window;
    HWND overlay_window;
    ID3D11Device* d3d_device;
    ID3D11DeviceContext* d3d_context;
    IDXGISwapChain* swap_chain;
    ID3D11RenderTargetView* render_target_view;
    bool is_active;
    bool show_overlay;
} g_overlay;

// Forward declarations
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Global state
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
static bool g_bInitialized = false;

// Hook function pointers
typedef HRESULT(WINAPI* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(WINAPI* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

static Present_t g_OriginalPresent = nullptr;
static ResizeBuffers_t g_OriginalResizeBuffers = nullptr;

// Window procedure hook
static WNDPROC g_OriginalWndProc = nullptr;
static HWND g_hWnd = nullptr;

void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Handle ImGui input when overlay is visible
    if (g_overlay.show_overlay && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    // Toggle overlay with F1
    if (msg == WM_KEYDOWN && wParam == VK_F1) {
        g_overlay.show_overlay = !g_overlay.show_overlay;
    }

    // Monitor game inputs for training
    MonitorGameInputs();

    return CallWindowProcW(g_OriginalWndProc, hWnd, msg, wParam, lParam);
}

HRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!g_bInitialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice)))) {
            g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
            
            // Get window handle
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_hWnd = sd.OutputWindow;

            // Hook window procedure
            g_OriginalWndProc = (WNDPROC)SetWindowLongPtrW(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

            CreateRenderTarget(pSwapChain);
            
            // Initialize ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            io.IniFilename = nullptr; // Disable imgui.ini

            // Setup ImGui style
            ImGui::StyleColorsDark();
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowBorderSize = 1.0f;
            style.WindowPadding = ImVec2(8, 8);
            style.WindowRounding = 6.0f;
            style.FrameRounding = 4.0f;
            style.Alpha = 0.95f;
            
            // Custom colors for overlay
            ImVec4* colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
            colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

            // Initialize ImGui backends
            ImGui_ImplWin32_Init(g_hWnd);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
            
            g_bInitialized = true;
        }
    }

    if (g_bInitialized) {
        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render overlay content
        RenderOverlay();

        // Render ImGui
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return g_OriginalPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    CleanupRenderTarget();
    HRESULT result = g_OriginalResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    CreateRenderTarget(pSwapChain);
    return result;
}

bool InitializeDirectXHook() {
    // Initialize MinHook
    if (MH_Initialize() != MH_OK) {
        return false;
    }

    // Create a dummy D3D11 device to get the Present function address
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* pSwapChain = nullptr;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, nullptr, &pContext
    );

    if (FAILED(hr)) {
        return false;
    }

    // Get Present function address from vtable
    void** pVTable = *reinterpret_cast<void***>(pSwapChain);
    void* pPresentFunc = pVTable[8]; // Present is at index 8
    void* pResizeBuffersFunc = pVTable[13]; // ResizeBuffers is at index 13

    // Clean up dummy objects
    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();

    // Hook Present function
    if (MH_CreateHook(pPresentFunc, &HookedPresent, reinterpret_cast<LPVOID*>(&g_OriginalPresent)) != MH_OK) {
        return false;
    }

    // Hook ResizeBuffers function
    if (MH_CreateHook(pResizeBuffersFunc, &HookedResizeBuffers, reinterpret_cast<LPVOID*>(&g_OriginalResizeBuffers)) != MH_OK) {
        return false;
    }

    // Enable hooks
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return false;
    }

    return true;
}

void ShutdownDirectXHook() {
    if (g_bInitialized) {
        // Restore original window procedure
        if (g_hWnd && g_OriginalWndProc) {
            SetWindowLongPtrW(g_hWnd, GWLP_WNDPROC, (LONG_PTR)g_OriginalWndProc);
        }

        // Cleanup ImGui
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupRenderTarget();

        if (g_pd3dDeviceContext) {
            g_pd3dDeviceContext->Release();
            g_pd3dDeviceContext = nullptr;
        }
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = nullptr;
        }

        g_bInitialized = false;
    }

    // Disable and remove hooks
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
