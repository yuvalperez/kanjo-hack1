#include "pch.h"
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Injected successfully6\n";

    uintptr_t modulebase = (uintptr_t)GetModuleHandle(L"ac_client.exe");

    bool bhp = false;
    bool bammo = false;
    bool brecoil = false;
    bool teleport = false;

    while (true) {
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }
        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
            bhp = !bhp;
            std::cout << "hp hack " << (bhp ? "enable" : "disable") << std::endl;
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            bammo = !bammo;
            std::cout << "ammo hack " << (bammo ? "enable" : "disable") << std::endl;
        }
        if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
            brecoil = !brecoil;
            std::cout << "recoil  hack " << (brecoil ? "enable" : "disable") << std::endl;
            if (brecoil) {
                mem::Patch((BYTE*)(0x004C8BA0), (BYTE*)"\xC2\x08\x00\x53\x55\x8B\x6C\x24\x30\x56", 10);
            }
            else {
                mem::Patch((BYTE*)(0x004C8BA0), (BYTE*)"\x83\xEC\x28\x53\x55\x8B\x6C\x24\x30\x56", 10);
            }
        }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
            teleport = !teleport;
            std::cout << "teleport to enemy  hack " << (teleport ? "enable" : "disable") << std::endl;
        }

        uintptr_t localPlayer = mem::FindDMAAddy(modulebase + 0x0018AC00, { 0x0 });
        if (localPlayer) {
            if (bhp) {
                *(int*)(localPlayer + 0XEC) = 2000;
            }
            if (bammo) {
                uintptr_t ammoAddr = mem::FindDMAAddy(modulebase + 0x17E0A8, { 0x364, 0x14, 0x0 });
                int* ammo = (int*)ammoAddr;
                    *ammo = 2000;
            }
            if (teleport) {
                for (uintptr_t i = 0x0; i <= 0x32; i += 0x4) {
                    uintptr_t player_pointer = mem::FindDMAAddy(0x0058AC04, {0x4, i });;
                    std::cout << player_pointer<<std::hex<<std::endl;
                    if (player_pointer == NULL) {
                        std::cout << "pointer = null\n";
                        Sleep(10);
                        continue;
                    }
                    bool is_dead = *(bool*)(player_pointer + 0x318);
                    if (is_dead == true) {
                        std::cout << "hes dead\n";
                        Sleep(10);
                        continue;
                    }
                    bool playerteam = *(bool*)+(player_pointer + 0x030C);
                    if (playerteam == true) {
                        std::cout << "same team\n";
                        Sleep(10);
                        continue;
                    }  //name print
                    uintptr_t nameptr = (player_pointer + 0x0205);
                    char32_t* name = (char32_t*)nameptr;
                    std::cout << "Player name: " << name << std::endl;
                    //
                    *(Vector3*)(localPlayer + 0x28)= *(Vector3*)(player_pointer + 0x28);
                    std::cout << "Teleported to enemy successfully." << std::endl;
                    Sleep(1000);
                    break;
                   
                }
            }

            Sleep(5);
        }
    }

        fclose(f);
        FreeConsole();
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
        switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
            if (hThread == NULL) {
                // Handle the error case, e.g., log an error message or display a message box
                MessageBox(NULL, L"Failed to create thread", L"Error", MB_OK | MB_ICONERROR);
            }
            else {
                CloseHandle(hThread);
            }
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
        }
        return TRUE;
    }