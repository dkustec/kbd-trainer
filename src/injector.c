#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdbool.h>

DWORD FindProcessByName(const wchar_t* process_name) {
    PROCESSENTRY32W pe32;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (!Process32FirstW(snapshot, &pe32)) {
        CloseHandle(snapshot);
        return 0;
    }
    
    do {
        if (wcscmp(pe32.szExeFile, process_name) == 0) {
            CloseHandle(snapshot);
            return pe32.th32ProcessID;
        }
    } while (Process32NextW(snapshot, &pe32));
    
    CloseHandle(snapshot);
    return 0;
}

bool InjectIntoProcess(DWORD process_id, const wchar_t* dll_path) {
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (!process) {
        printf("Failed to open target process. Error: %lu\n", GetLastError());
        return false;
    }
    
    SIZE_T path_size = (wcslen(dll_path) + 1) * sizeof(wchar_t);
    LPVOID remote_memory = VirtualAllocEx(process, NULL, path_size, MEM_COMMIT, PAGE_READWRITE);
    
    if (!remote_memory) {
        printf("Failed to allocate memory in target process. Error: %lu\n", GetLastError());
        CloseHandle(process);
        return false;
    }
    
    if (!WriteProcessMemory(process, remote_memory, dll_path, path_size, NULL)) {
        printf("Failed to write DLL path to target process. Error: %lu\n", GetLastError());
        VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC load_library = GetProcAddress(kernel32, "LoadLibraryW");
    
    HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, remote_memory, 0, NULL);
    
    if (!thread) {
        printf("Failed to create remote thread. Error: %lu\n", GetLastError());
        VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    WaitForSingleObject(thread, INFINITE);
    
    VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
    CloseHandle(thread);
    CloseHandle(process);
    
    printf("Successfully injected DLL into process %lu\n", process_id);
    return true;
}

int main(int argc, char* argv[]) {
    printf("KBD Trainer Overlay Injector\n");
    printf("============================\n\n");
    
    // Target processes (common fighting games)
    const wchar_t* target_processes[] = {
        L"TekkenGame-Win64-Shipping.exe",  // Tekken 8
        L"Tekken7-Win64-Shipping.exe",     // Tekken 7
        L"TEKKEN 7.exe",                   // Tekken 7 (Steam)
        L"StreetFighterV.exe",             // Street Fighter V
        L"SF6.exe",                        // Street Fighter 6
        NULL
    };
    
    // Get DLL path
    wchar_t dll_path[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, dll_path);
    wcscat_s(dll_path, MAX_PATH, L"\\KBDTrainerOverlay.dll");
    
    // Check if DLL exists
    if (GetFileAttributesW(dll_path) == INVALID_FILE_ATTRIBUTES) {
        printf("Error: KBDTrainerOverlay.dll not found in current directory!\n");
        printf("Expected path: %ls\n", dll_path);
        system("pause");
        return 1;
    }
    
    printf("Searching for target processes...\n");
    
    DWORD target_pid = 0;
    const wchar_t* found_process = NULL;
    
    // Search for target processes
    for (int i = 0; target_processes[i] != NULL; i++) {
        target_pid = FindProcessByName(target_processes[i]);
        if (target_pid != 0) {
            found_process = target_processes[i];
            break;
        }
    }
    
    if (target_pid == 0) {
        printf("No supported fighting game found running.\n");
        printf("Supported games:\n");
        for (int i = 0; target_processes[i] != NULL; i++) {
            wprintf(L"  - %s\n", target_processes[i]);
        }
        printf("\nPlease start a supported game and try again.\n");
        system("pause");
        return 1;
    }
    
    wprintf(L"Found target process: %s (PID: %lu)\n", found_process, target_pid);
    printf("Injecting overlay DLL...\n");
    
    if (InjectIntoProcess(target_pid, dll_path)) {
        printf("\nOverlay injected successfully!\n");
        printf("Press F1 in-game to toggle the overlay.\n");
        printf("Controls:\n");
        printf("  F1 - Toggle overlay visibility\n");
        printf("  Overlay will automatically detect KBD/Wavedash inputs\n");
    } else {
        printf("Failed to inject overlay. Make sure you're running as administrator.\n");
    }
    
    system("pause");
    return 0;
}
