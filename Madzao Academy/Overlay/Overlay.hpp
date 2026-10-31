#pragma once
#include <windows.h>
#include "../Entity/Entity.hpp"

class Overlay {
public:
    Overlay();
    ~Overlay();

    bool Create(const wchar_t* targetWindowName);
    bool ProcessMessages();

    void BeginPaint();
    void EndPaint();

    void DrawBox(float x, float y, float width, float height, COLORREF color);

    static bool WorldToScreen(const Vector3& worldPos, const ViewMatrix& vm,
        int screenWidth, int screenHeight, Vector2& screenPos);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    HWND m_overlayWindow = nullptr;
    HWND m_gameWindow = nullptr;
    HDC  m_hdc = nullptr;
    HDC  m_hdcBuffer = nullptr;
    HBITMAP m_bitmap = nullptr;
    PAINTSTRUCT m_ps{};
    int m_width = 0;
    int m_height = 0;
};