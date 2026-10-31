#pragma once
#include <cstdint>

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct ViewMatrix {
    float matrix[4][4];
};

namespace Offsets {
    // client.dll offsets
    constexpr uintptr_t dwLocalPlayerPawn = 0x25698C8;
    constexpr uintptr_t dwEntityList = 0x271EB28;
    constexpr uintptr_t dwViewMatrix = 0x256F1D0;

    // C_BaseEntity
    constexpr uintptr_t m_iHealth = 0x34C;
    constexpr uintptr_t m_lifeState = 0x354;
    constexpr uintptr_t m_iTeamNum = 0x3E7;
    constexpr uintptr_t m_pGameSceneNode = 0x330;

    // CGameSceneNode
    constexpr uintptr_t m_vecAbsOrigin = 0xC8;

    // CCSPlayerController
    constexpr uintptr_t m_hPlayerPawn = 0x92C;
}
