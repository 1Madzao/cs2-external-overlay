#include "Overlay.hpp"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

Overlay::Overlay() {}

Overlay::~Overlay() {
    if (m_hdcBuffer) DeleteDC(m_hdcBuffer);
    if (m_bitmap) DeleteObject(m_bitmap);
    if (m_overlayWindow) DestroyWindow(m_overlayWindow);
}

bool Overlay::Create(const wchar_t* targetWindowName) {
    m_gameWindow = FindWindowW(nullptr, targetWindowName);
    if (!m_gameWindow) return false;

    RECT gameRect{};
    GetClientRect(m_gameWindow, &gameRect);
    POINT gamePos = { 0, 0 };
    ClientToScreen(m_gameWindow, &gamePos);

    m_width = gameRect.right - gameRect.left;
    m_height = gameRect.bottom - gameRect.top;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"MadzaoOverlay";
    RegisterClassExW(&wc);

    m_overlayWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"MadzaoOverlay",
        L"Madzao Academy Overlay",
        WS_POPUP,
        gamePos.x, gamePos.y, m_width, m_height,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );

    if (!m_overlayWindow) return false;

    SetLayeredWindowAttributes(m_overlayWindow, RGB(0, 0, 0), 0, LWA_COLORKEY);

    ShowWindow(m_overlayWindow, SW_SHOW);
    UpdateWindow(m_overlayWindow);

    return true;
}

bool Overlay::ProcessMessages() {
    MSG msg{};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return true;
}

void Overlay::BeginPaint() {
    m_hdc = GetDC(m_overlayWindow);
    m_hdcBuffer = CreateCompatibleDC(m_hdc);
    m_bitmap = CreateCompatibleBitmap(m_hdc, m_width, m_height);
    SelectObject(m_hdcBuffer, m_bitmap);

    RECT rect = { 0, 0, m_width, m_height };
    FillRect(m_hdcBuffer, &rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
}

void Overlay::EndPaint() {
    BitBlt(m_hdc, 0, 0, m_width, m_height, m_hdcBuffer, 0, 0, SRCCOPY);

    DeleteObject(m_bitmap);
    DeleteDC(m_hdcBuffer);
    ReleaseDC(m_overlayWindow, m_hdc);
    m_hdcBuffer = nullptr;
    m_bitmap = nullptr;
}

void Overlay::DrawBox(float x, float y, float width, float height, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HBRUSH brush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));

    SelectObject(m_hdcBuffer, pen);
    SelectObject(m_hdcBuffer, brush);

    Rectangle(m_hdcBuffer, static_cast<int>(x), static_cast<int>(y),
        static_cast<int>(x + width), static_cast<int>(y + height));

    DeleteObject(pen);
}

bool Overlay::WorldToScreen(const Vector3& worldPos, const ViewMatrix& vm,
    int screenWidth, int screenHeight, Vector2& screenPos) {

    float w = vm.matrix[3][0] * worldPos.x +
        vm.matrix[3][1] * worldPos.y +
        vm.matrix[3][2] * worldPos.z +
        vm.matrix[3][3];

    if (w < 0.001f) return false;

    float ndcX = (vm.matrix[0][0] * worldPos.x + vm.matrix[0][1] * worldPos.y +
        vm.matrix[0][2] * worldPos.z + vm.matrix[0][3]) / w;

    float ndcY = (vm.matrix[1][0] * worldPos.x + vm.matrix[1][1] * worldPos.y +
        vm.matrix[1][2] * worldPos.z + vm.matrix[1][3]) / w;

    screenPos.x = (screenWidth * 0.5f) + (ndcX * screenWidth * 0.5f);
    screenPos.y = (screenHeight * 0.5f) - (ndcY * screenHeight * 0.5f);

    return true;
}