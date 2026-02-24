#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

class MemoryManager
{
private:
    HANDLE hProcess;
    DWORD processId;
    uintptr_t gameAssemblyBase;

    struct ResourceInfo
    {
        std::vector<uintptr_t> offsets;
        int value;
        bool freeze;
    };

    ResourceInfo resources[8];
    bool godMode;
    bool infiniteHealth;
    bool infiniteMana;
    bool oneHitKill;
    bool maxExperience;

    void InitializeAddresses()
    {
        resources[0].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x70, 0x18, 0xDC };
        resources[1].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x70, 0x18, 0xCC };
        resources[2].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x70, 0x18, 0xEC };
        resources[3].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x60, 0x18, 0x5C };
        resources[4].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x70, 0x18, 0x10C };
        resources[5].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x60, 0x18, 0x8C };
        resources[6].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x60, 0x18, 0x7C };
        resources[7].offsets = { 0xB8, 0x20, 0xA0, 0x48, 0x58 };

        for (int i = 0; i < 8; i++)
        {
            resources[i].value = 0;
            resources[i].freeze = false;
        }

        godMode = false;
        infiniteHealth = false;
    }

    DWORD GetProcessId(const wchar_t* processName)
    {
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof(PROCESSENTRY32W);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return 0;

        if (Process32FirstW(snapshot, &entry))
        {
            do
            {
                if (_wcsicmp(entry.szExeFile, processName) == 0)
                {
                    CloseHandle(snapshot);
                    return entry.th32ProcessID;
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return 0;
    }

    uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
    {
        uintptr_t modBaseAddr = 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
        
        if (hSnap != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32W modEntry;
            modEntry.dwSize = sizeof(modEntry);
            
            if (Module32FirstW(hSnap, &modEntry))
            {
                do
                {
                    if (_wcsicmp(modEntry.szModule, modName) == 0)
                    {
                        modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                        break;
                    }
                } while (Module32NextW(hSnap, &modEntry));
            }
        }
        CloseHandle(hSnap);
        return modBaseAddr;
    }

    uintptr_t GetPointerAddress(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets)
    {
        uintptr_t addr = baseAddress;
        
        for (size_t i = 0; i < offsets.size(); i++)
        {
            if (i == 0)
            {
                addr += offsets[i];
            }
            else
            {
                if (!ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(addr), nullptr))
                    return 0;
                addr += offsets[i];
            }
        }
        
        return addr;
    }

public:
    MemoryManager() : hProcess(NULL), processId(0), gameAssemblyBase(0)
    {
        InitializeAddresses();
    }

    ~MemoryManager()
    {
        if (hProcess)
            CloseHandle(hProcess);
    }

    bool AttachToProcess(const wchar_t* processName)
    {
        processId = GetProcessId(processName);
        if (processId == 0)
            return false;

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (!hProcess)
            return false;

        // Получаем базовый адрес GameAssembly.dll
        gameAssemblyBase = GetModuleBaseAddress(processId, L"GameAssembly.dll");
        if (gameAssemblyBase == 0)
        {
            CloseHandle(hProcess);
            hProcess = NULL;
            return false;
        }

        return true;
    }

    bool SetResourceValue(int resourceType, int value)
    {
        if (resourceType < 0 || resourceType >= 8)
            return false;

        // Базовый адрес зависит от типа ресурса
        uintptr_t baseOffset = (resourceType <= 2) ? 0x045F1AD0 : 
                               (resourceType == 3) ? 0x045F1AD0 :
                               (resourceType >= 4 && resourceType <= 6) ? 0x045F1AD0 :
                               0x045F1AD0;

        uintptr_t address = GetPointerAddress(gameAssemblyBase + baseOffset, resources[resourceType].offsets);
        
        if (address == 0)
            return false;

        resources[resourceType].value = value;
        return WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), nullptr);
    }

    bool SetFreeze(int resourceType, bool freeze)
    {
        if (resourceType < 0 || resourceType >= 7) // Навыки нельзя заморозить
            return false;

        resources[resourceType].freeze = freeze;
        return true;
    }

    bool SetGodMode(bool enabled)
    {
        godMode = enabled;
        return true;
    }

    bool SetInfiniteHealth(bool enabled)
    {
        infiniteHealth = enabled;
        return true;
    }

    bool SetInfiniteMana(bool enabled)
    {
        infiniteMana = enabled;
        return true;
    }

    bool SetOneHitKill(bool enabled)
    {
        oneHitKill = enabled;
        return true;
    }

    bool SetMaxExperience(bool enabled)
    {
        maxExperience = enabled;
        return true;
    }

    void Update()
    {
        for (int i = 0; i < 7; i++)
        {
            if (resources[i].freeze)
            {
                SetResourceValue(i, resources[i].value);
            }
        }

        if (godMode)
        {
        }

        if (infiniteHealth)
        {
        }

        if (infiniteMana)
        {
        }

        if (oneHitKill)
        {
        }

        if (maxExperience)
        {
        }
    }
};
