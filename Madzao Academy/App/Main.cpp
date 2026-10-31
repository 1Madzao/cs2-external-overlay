#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <clocale>
#include "../Memory/Memory.hpp"
#include "../Entity/Entity.hpp"
#include "../Overlay/Overlay.hpp"

int main() {
    setlocale(LC_ALL, "Portuguese");

    std::cout << "=====================================================\n";
    std::cout << "   MADZAO ACADEMY - OVERLAY VISUAL (W2S)             \n";
    std::cout << "=====================================================\n\n";

    Memory mem;

    std::cout << "[*] Conectando ao CS2...\n";
    if (!mem.Attach(L"cs2.exe")) {
        std::cout << "[-] CS2 nao encontrado!\n";
        system("pause");
        return 1;
    }

    auto clientBase = mem.GetModuleBase(L"client.dll");
    if (!clientBase) {
        std::cout << "[-] client.dll nao encontrada.\n";
        system("pause");
        return 1;
    }
    std::cout << "[+] client.dll: 0x" << std::hex << clientBase.value() << std::dec << "\n";

    Overlay overlay;
    std::cout << "[*] Criando overlay transparente...\n";
    if (!overlay.Create(L"Counter-Strike 2")) {
        std::cout << "[-] Janela do CS2 nao encontrada! O jogo precisa estar em tela cheia janelada.\n";
        system("pause");
        return 1;
    }
    std::cout << "[+] Overlay ativo! Resolucao: " << overlay.GetWidth() << "x" << overlay.GetHeight() << "\n";
    std::cout << "[*] Desenhando caixas nos bots...\n";
    std::cout << "[*] Pressione CTRL+C no console para encerrar.\n\n";

    int frameCount = 0;

    while (overlay.ProcessMessages()) {
        frameCount++;

        auto viewMatrix = mem.Read<ViewMatrix>(clientBase.value() + Offsets::dwViewMatrix);
        if (!viewMatrix) continue;

        auto localPawn = mem.Read<uintptr_t>(clientBase.value() + Offsets::dwLocalPlayerPawn);
        uint8_t localTeam = 0;
        if (localPawn && localPawn.value() != 0) {
            auto team = mem.Read<uint8_t>(localPawn.value() + Offsets::m_iTeamNum);
            if (team) localTeam = team.value();
        }

        overlay.BeginPaint();

        auto entityList = mem.Read<uintptr_t>(clientBase.value() + Offsets::dwEntityList);
        if (!entityList || entityList.value() == 0) {
            overlay.EndPaint();
            continue;
        }

        int enemiesDrawn = 0;

        for (int i = 1; i <= 64; i++) {
            auto entry = mem.Read<uintptr_t>(entityList.value() + 0x8 * ((i & 0x7FFF) >> 9) + 0x10);
            if (!entry || entry.value() == 0) continue;

            auto controller = mem.Read<uintptr_t>(entry.value() + 0x70 * (i & 0x1FF));
            if (!controller || controller.value() == 0) continue;

            auto pawnHandle = mem.Read<DWORD>(controller.value() + Offsets::m_hPlayerPawn);
            if (!pawnHandle || pawnHandle.value() == 0 || pawnHandle.value() == 0xFFFFFFFF) continue;

            uint32_t pawnIndex = pawnHandle.value() & 0x7FFF;
            auto pawnEntry = mem.Read<uintptr_t>(entityList.value() + 0x8 * (pawnIndex >> 9) + 0x10);
            if (!pawnEntry || pawnEntry.value() == 0) continue;

            auto pawn = mem.Read<uintptr_t>(pawnEntry.value() + 0x70 * (pawnIndex & 0x1FF));
            if (!pawn || pawn.value() == 0) continue;

            auto health = mem.Read<int>(pawn.value() + Offsets::m_iHealth);
            if (!health || health.value() <= 0 || health.value() > 100) continue;

            auto team = mem.Read<uint8_t>(pawn.value() + Offsets::m_iTeamNum);
            if (!team || team.value() == localTeam) continue;

            auto sceneNode = mem.Read<uintptr_t>(pawn.value() + Offsets::m_pGameSceneNode);
            if (!sceneNode || sceneNode.value() == 0) continue;

            auto feetPos = mem.Read<Vector3>(sceneNode.value() + Offsets::m_vecAbsOrigin);
            if (!feetPos) continue;

            Vector3 headPos = {
                feetPos->x,
                feetPos->y,
                feetPos->z + 72.0f
            };

            Vector2 screenFeet, screenHead;
            if (!Overlay::WorldToScreen(feetPos.value(), viewMatrix.value(),
                overlay.GetWidth(), overlay.GetHeight(), screenFeet)) continue;
            if (!Overlay::WorldToScreen(headPos, viewMatrix.value(),
                overlay.GetWidth(), overlay.GetHeight(), screenHead)) continue;

            float boxHeight = screenFeet.y - screenHead.y;
            float boxWidth  = boxHeight * 0.40f;
            float boxX      = screenHead.x - (boxWidth * 0.5f);
            float boxY      = screenHead.y;

            overlay.DrawBox(boxX, boxY, boxWidth, boxHeight, RGB(255, 0, 0));
            enemiesDrawn++;
        }

        overlay.EndPaint();

        if (frameCount % 60 == 0) {
            std::cout << "\r[Status] Inimigos desenhados na tela: " << enemiesDrawn << "     " << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}