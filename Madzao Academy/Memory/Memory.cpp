#include "Memory.hpp"
#include <tlhelp32.h>
#include <iostream>

Memory::Memory() : m_pid(std::nullopt), m_processHandle(nullptr) {}

Memory::~Memory() {
    if (m_processHandle && m_processHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_processHandle);
    }
}

std::optional<DWORD> Memory::FindProcessId(std::wstring_view targetName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(PROCESSENTRY32W);

    std::optional<DWORD> foundPid = std::nullopt;

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (targetName == entry.szExeFile) {
                foundPid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return foundPid;
}

bool Memory::Attach(std::wstring_view processName) {
    m_pid = FindProcessId(processName);
    if (!m_pid) {
        std::wcerr << L"[-] Processo nao encontrado: " << processName << L"\n";
        return false;
    }

    m_processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, m_pid.value());
    if (!m_processHandle) {
        std::cerr << "[-] Acesso negado pelo Windows. Execute como Administrador!\n";
        return false;
    }

    std::cout << "[+] Conectado com sucesso! PID: " << m_pid.value() << "\n";
    return true;
}

std::optional<uintptr_t> Memory::GetModuleBase(std::wstring_view moduleName) const {
    if (!m_pid) {
        return std::nullopt;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_pid.value());
    if (snapshot == INVALID_HANDLE_VALUE) {
        return std::nullopt;
    }

    MODULEENTRY32W entry{};
    entry.dwSize = sizeof(MODULEENTRY32W);

    std::optional<uintptr_t> baseAddress = std::nullopt;

    if (Module32FirstW(snapshot, &entry)) {
        do {
            if (moduleName == entry.szModule) {
                baseAddress = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
                break;
            }
        } while (Module32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return baseAddress;
}