@echo off
REM ============================================================
REM  build.bat -- 一键编译上班攒钱时钟桌面版
REM  需要 MinGW-w64 (gcc) 在 PATH 中
REM ============================================================
where gcc >nul 2>nul
if errorlevel 1 (
    echo [ERROR] 未找到 gcc，请先安装 MinGW-w64 并加入 PATH。
    exit /b 1
)

echo [1/3] 生成 index_html.h ...
python build_gen.py index.html index_html.h

echo [2/3] 编译 moneyclock.c ...
gcc -O2 -o MoneyClock.exe moneyclock.c -lshlwapi

if errorlevel 1 (
    echo [ERROR] 编译失败。
    exit /b 1
)

echo [3/3] 完成！运行 MoneyClock.exe 即可启动。
