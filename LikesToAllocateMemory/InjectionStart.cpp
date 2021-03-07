#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>

// by Vx <3
using namespace std;

bool file_exists(string file_name) 
{
    struct stat buffer;
    return (stat(file_name.c_str(), &buffer) == 0);
}

void get_Proc_ID(const char* window_title, DWORD& process_id) 
{
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &process_id);
}

void error(const char* error_title, const char* error_message) 
{
    MessageBox(NULL, error_message, error_title, NULL);
    exit(-1);
}

int main()
{
    string ctitle = "Injector";
    DWORD proc_id = NULL;
    char dll_path[MAX_PATH];
    const char* dll_name = "Test.dll";
    const char* window_title = "Target Process";

    SetConsoleTitleA(ctitle.c_str());

    if (!file_exists(dll_name))
    {
        error("[File]", "File doesn't exist.");
    }

    if (!GetFullPathName(dll_name, MAX_PATH, dll_path, nullptr))
    {
        error("[File]", "Failed to get full path name.");
    }

    get_Proc_ID(window_title, proc_id);
    if (proc_id == NULL)
    {
        error("Process", "Failed to get process ID.");
    }

    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, proc_id);
    if (!h_process) 
    {
        error("Handle", "Failed to open a handle to process.");
    }

    void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocated_memory) 
    {
        error("Allocation", "Failed to allocate memory.");
    }

    if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr)) 
    {
        error("{Write]", "Failed to Write to process memory.");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
    if (!h_thread) 
    {
        error("[Thread]", "Failed to creat thread.");
    }

    CloseHandle(h_process);
    VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);

    MessageBox(NULL, "Injected!", "Success", NULL);
}