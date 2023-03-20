#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>

using namespace std;

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
                if (!_wcsicmp(modEntry.szModule, modName))
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
uintptr_t GetPointerAddress(HWND hwnd, uintptr_t gameBaseAddr, uintptr_t address, vector<uintptr_t> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    uintptr_t offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    uintptr_t pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}
int main() {
    bool Quit = false;

    do {
        HWND hwnd = FindWindowA(NULL, "Kanjozoku Game");
        if (hwnd == NULL) {
            cout << "Can't find Process.\n";
            Sleep(2000);
            exit(-1);
        }
        else {
            DWORD procID;
            GetWindowThreadProcessId(hwnd, &procID);
            HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

            if (procID == NULL) {
                cout << "Can't find Process.\n";
                Sleep(2000);
                exit(-1);
            }
            else {
                HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, procID);
                if (handle == NULL) {
                    cout << "Cannot open process.\n";
                    Sleep(3000);
                    return -1;
                }
                else {

                    int CorrectMoney = 0;
                    int AddedMoney = 0;
                    int choices;
                    uintptr_t baseAddress1 = GetModuleBaseAddress(procID, L"GameAssembly.dll");
                    uintptr_t BaseOff = 0X0208F1D0;
                    vector<uintptr_t> MoneyOffsets{ 0xA0,0X0,0XB8,0X90, 0x78, 0x18, 0x10 };
                    uintptr_t MoneyPointer = GetPointerAddress(hwnd, baseAddress1, BaseOff, MoneyOffsets);

                    cout << "Menu\n";
                    cout << "1)Money Hack\n";
                    cout << "2)Show current money\n";
                    cout << "3)EXIT\n";
                    cin >> choices;
                    switch (choices) {
                    case 1:
                        cout << "Edit money\n";
                        cin >> AddedMoney;
                        WriteProcessMemory(handle, (LPVOID*)MoneyPointer, &AddedMoney, sizeof(AddedMoney), 0);
                        if (AddedMoney != -0) {
                            cout << "money added\n";
                            CloseHandle(handle);
                        }
                        break;
                    case 2:
                        ReadProcessMemory(handle, (VOID*)MoneyPointer, &CorrectMoney, sizeof(CorrectMoney), 0);
                        ReadProcessMemory(handle, (LPVOID*)MoneyPointer, &CorrectMoney, sizeof(CorrectMoney), 0);
                        if (CorrectMoney == 0)
                        {
                            cout << "faild read\n";
                            Sleep(300);
                            exit(-1);
                            CloseHandle(handle);
                        }
                        else {
                            cout << "ur money\n" << CorrectMoney << endl;
                        }
                        break;
                    case 3:
                        cout << "bye\n";
                        Quit = true;
                        break;
                    default:
                        cout << "Invalid choice\n";
                        break;
                    }

                }
            }
        }

    } while (Quit == false);
    return 0;
}
