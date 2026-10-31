#pragma once
#include <windows.h>
#include <string_view>
#include <optional>
#include <cstdint>

class Memory {
public:
    Memory();
    ~Memory();

    bool Attach(std::wstring_view processName);
    std::optional<uintptr_t> GetModuleBase(std::wstring_view moduleName) const;

    template <typename T>
    std::optional<T> Read(uintptr_t address) const {
        T buffer{};
        SIZE_T bytesRead = 0;

        if (ReadProcessMemory(m_processHandle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), &bytesRead) &&
            bytesRead == sizeof(T)) {
            return buffer;
        }
        return std::nullopt;
    }

private:
    std::optional<DWORD> m_pid;
    HANDLE m_processHandle;

    static std::optional<DWORD> FindProcessId(std::wstring_view targetName);
};