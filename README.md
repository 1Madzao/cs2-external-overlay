# cs2-external-overlay

A lightweight C++17 external memory reader and transparent GDI overlay for Counter-Strike 2.

Demonstrates process memory reading (`ReadProcessMemory`), entity list traversal, and 3D-to-2D perspective projection (World-to-Screen) using the game's view matrix.

## Features

- External process memory reading using Win32 API (`OpenProcess`, `ReadProcessMemory`).
- Dynamic base address resolution via `CreateToolhelp32Snapshot`.
- Transparent borderless GDI overlay using double buffering (`BitBlt`) and color-key transparency (`LWA_COLORKEY`).
- Entity iteration through CS2's segmented entity list.
- World-to-Screen projection using normalized device coordinates (NDC).

## Project Structure

- `App/Main.cpp` - Entry point and main render loop.
- `Memory/Memory.hpp`, `Memory.cpp` - Process attachment and memory wrapper.
- `Overlay/Overlay.hpp`, `Overlay.cpp` - Win32 transparent window and GDI drawing.
- `Entity/Entity.hpp` - Vector structures and memory offsets.

## Build

1. Open `Madzao Academy.sln` in Visual Studio 2022.
2. Select **Release | x64**.
3. Build the solution (`Ctrl + Shift + B`).

Output binary: `x64/Release/Madzao Academy.exe`.

## Usage

1. Launch CS2 in offline/insecure mode:
   ```cmd
   cs2.exe -insecure
   ```
2. Set display mode to **Fullscreen Windowed** (Borderless).
3. Run `Madzao Academy.exe`.

## Disclaimer

This project is for educational and reverse-engineering research purposes only. It is intended for offline matches against bots (`-insecure`). It does not bypass VAC or any other anti-cheat system.
