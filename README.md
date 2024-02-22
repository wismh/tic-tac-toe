# Tic-tac-toe (Wind)

Sibling remake hosted on **Wind**. This drop is a **windowed smoke app**: XML/CSS HUD (MVVM + `ICommand`) and a bouncing quad via `Get` of builtin mesh/material. The 3×3 board is next.

`Engine::Init` loads `assets/engine/catalog.toml` and `assets/catalog.toml`. The game does not parse catalogs in C++.

## Build

```bash
git submodule update --init --recursive
cmake --preset vs
cmake --build build --target tic-tac-toe --config Debug
```

Run `build/bin/Debug/tic-tac-toe.exe`. **Nudge** kicks the quad; **Quit** closes the window.

Wind is the git submodule at `external/engine` (`url = ../engine`). Game CMake is `add_subdirectory` + `engine_add_game` (codegen + runtime asset copy). Mixer stays off until a cue is needed.
