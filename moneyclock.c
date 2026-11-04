/*
 * moneyclock.c — 上班攒钱时钟 桌面启动器
 *
 * 做法：把 index.html 写到 %LOCALAPPDATA%\MoneyMoneyClock\index.html，
 * 然后用 Edge 的 --app 模式打开，得到一个没有地址栏的独立窗口。
 *
 * 编译（MinGW-w64）：
 *   gcc -O2 -o MoneyClock.exe moneyclock.c
 *   或运行 build.bat / make
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlobj.h>

/* ------------------------------------------------------------------ */
/* 由 build.py / makefile 生成的 index_html 数组                        */
/* ------------------------------------------------------------------ */
#include "index_html.h"

#define APP_DIR_NAME   L"\\MoneyMoneyClock"
#define APP_TITLE      L"上班攒钱时钟"

static void log_error(const wchar_t *msg)
{
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        size_t n = wcslen(path);
        wcscpy(path + n, APP_DIR_NAME);
        CreateDirectoryW(path, NULL);
        wcscat(path, L"\\error.log");
        HANDLE h = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            const wchar_t *nl = L"\r\n";
            WriteFile(h, msg, wcslen(msg) * sizeof(wchar_t), NULL, NULL);
            WriteFile(h, nl, 2 * sizeof(wchar_t), NULL, NULL);
            CloseHandle(h);
        }
    }
}

/* 把 index.html 写到 %LOCALAPPDATA%\MoneyMoneyClock\index.html */
static int write_index(wchar_t *out_path, size_t out_size)
{
    wchar_t dir[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, dir))) {
        log_error(L"无法获取 LOCALAPPDATA 路径。");
        return 0;
    }
    wcscat(dir, APP_DIR_NAME);
    if (!CreateDirectoryW(dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        log_error(L"无法创建程序文件夹，请检查文件夹权限。");
        return 0;
    }

    wcscpy(out_path, dir);
    wcscat(out_path, L"\\index.html");

    HANDLE h = CreateFileW(out_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        log_error(L"无法保存程序文件，请检查文件夹权限。");
        return 0;
    }

    /* 写成 UTF-8 无 BOM */
    const unsigned char *p = (const unsigned char *)index_html;
    DWORD written = 0;
    BOOL ok = WriteFile(h, p, (DWORD)index_html_len, &written, NULL);
    CloseHandle(h);

    if (!ok || written != index_html_len) {
        log_error(L"写入 index.html 失败。");
        return 0;
    }
    return 1;
}

/* 找到 Edge 可执行文件；找不到返回 0 */
static int find_edge(wchar_t *out, size_t out_size)
{
    static const wchar_t *rel[] = {
        L"Microsoft\\Edge\\Application\\msedge.exe",
        L"Microsoft\\Edge Beta\\Application\\msedge.exe",
        L"Microsoft\\Edge Dev\\Application\\msedge.exe",
    };
    static const wchar_t *envs[] = {
        L"%ProgramFiles(x86)%",
        L"%ProgramFiles%",
        L"%LOCALAPPDATA%",
    };

    for (size_t e = 0; e < sizeof(envs) / sizeof(envs[0]); e++) {
        wchar_t base[MAX_PATH];
        if (!ExpandEnvironmentStringsW(envs[e], base, MAX_PATH)) continue;
        for (size_t i = 0; i < sizeof(rel) / sizeof(rel[0]); i++) {
            wchar_t path[MAX_PATH];
            wcscpy(path, base);
            wcscat(path, L"\\");
            wcscat(path, rel[i]);
            if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
                wcsncpy(out, path, out_size - 1);
                out[out_size - 1] = L'\0';
                return 1;
            }
        }
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmd, int nShow)
{
    (void)hInst; (void)hPrev; (void)lpCmd; (void)nShow;

    wchar_t index_path[MAX_PATH];
    if (!write_index(index_path, MAX_PATH)) {
        MessageBoxW(NULL, L"无法保存程序文件，请检查文件夹权限。", APP_TITLE, MB_OK | MB_ICONERROR);
        return 1;
    }

    wchar_t edge[MAX_PATH];
    if (!find_edge(edge, MAX_PATH)) {
        MessageBoxW(NULL,
                    L"未找到 Microsoft Edge。\n\n"
                    L"请直接用浏览器打开以下文件：\n" ,
                    APP_TITLE, MB_OK | MB_ICONWARNING);
        wchar_t msg[1200];
        swprintf(msg, 1200, L"未找到 Microsoft Edge。\n\n请直接用浏览器打开：\n%s", index_path);
        MessageBoxW(NULL, msg, APP_TITLE, MB_OK | MB_ICONWARNING);
        return 1;
    }

    /* file:///C:/path/to/index.html  →  斜杠化 */
    wchar_t url[MAX_PATH * 2];
    swprintf(url, MAX_PATH * 2, L"file:///%s", index_path);
    for (wchar_t *p = url; *p; p++) {
        if (*p == L'\\') *p = L'/';
    }

    wchar_t cmd[2048];
    swprintf(cmd, 2048,
             L"\"%s\" --app=\"%s\" --window-size=1120,850", edge, url);

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t workdir[MAX_PATH];
    wcscpy(workdir, index_path);
    wchar_t *slash = wcsrchr(workdir, L'\\');
    if (slash) *slash = L'\0';

    if (!CreateProcessW(edge, cmd, NULL, NULL, FALSE, 0, NULL, workdir, &si, &pi)) {
        wchar_t msg[1400];
        swprintf(msg, 1400, L"启动 Edge 失败。\n\n请手动打开：\n%s", index_path);
        MessageBoxW(NULL, msg, APP_TITLE, MB_OK | MB_ICONERROR);
        return 1;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
