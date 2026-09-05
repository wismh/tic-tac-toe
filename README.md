# Tic-tac-toe (Wind)

[![CI](https://github.com/wismh/tic-tac-toe/actions/workflows/ci.yml/badge.svg)](https://github.com/wismh/tic-tac-toe/actions/workflows/ci.yml)

A neon tic-tac-toe game built on **[Wind](https://github.com/wismh/wind-engine)**, a small custom 2D engine (ECS, declarative XML/CSS UI, command-buffer renderer). Looping music, step cue, and X/O scores in a neon HUD font.

Start on the menu: **Play PvP**, **Play PvE** (O is a bot), or **Exit**. During a match, **Back** returns to the menu. After a win or draw the board pauses one second, then clears.

## Prerequisites

- CMake 3.16 or newer.
- Windows with Visual Studio and C++23 support. The checked-in `vs` preset targets `Visual Studio 18 2026`; if you have a different version installed (e.g. VS2022), configure directly instead of via the preset — `cmake -S . -B build` lets CMake pick your installed generator, which is also how CI builds it (see `.github/workflows/ci.yml`).
- Git, with submodules: this repo pulls in the Wind engine as `external/engine`, which itself pulls in SDL3, googletest, and other dependencies as submodules.

## Build

```bash
git submodule update --init --recursive
cmake --preset vs
cmake --build build --target tic-tac-toe --config Debug
```

Run `build/bin/Debug/tic-tac-toe.exe`. Mixer is on by default (`ENGINE_WITH_AUDIO`, defaulted ON by Wind for any game's `add_subdirectory`). Wind lives at `external/engine`. Engine gaps: [docs/engine-limits.md](docs/engine-limits.md).

## Build for web / Android

Wind's `web` (Emscripten/WebGL2) and `android-arm64` (NDK/GLES3) profiles carry over via the `web` / `android-arm64` presets in this repo's own [CMakePresets.json](CMakePresets.json). Both platforms build assets with a **native** `asset_codegen` first — the cross compiler can't run the cook tool as a host binary:

```bash
cmake -S . -B build-native -DENGINE_BUILD_TESTS=OFF
cmake --build build-native --target asset_codegen --config Debug
```

**Web** (install [emsdk](https://emscripten.org/docs/getting_started/downloads.html) and activate it first). Audio (SDL3_mixer's WAV backend) is on by default here too — confirmed working (menu, gameplay, and sound) in-browser:

```bash
emcmake cmake --preset web -DENGINE_HOST_ASSET_CODEGEN="$PWD/build-native/Debug/asset_codegen.exe"
cmake --build --preset web
python3 -m http.server -d build-web/bin   # serve over HTTP, file:// blocks WASM
```

(pass `-DENGINE_WITH_AUDIO=OFF` to the configure step above to leave the mixer out.)

**Android** (set `ANDROID_NDK_HOME`; the `android-arm64` CMake preset compile-checks `libmain.so`, it doesn't produce an APK on its own). For a real APK, build `asset_codegen` natively as above, then drive Wind's Gradle template in `external/engine/cmake/android/` — it defaults `sdk.dir` from `local.properties`/`ANDROID_HOME`:

```bash
cd external/engine/cmake/android
gradle :app:assembleDebug \
  -PENGINE_SOURCE_DIR="$(pwd)/../.." \
  -PENGINE_ANDROID_CMAKE="$(pwd)/../../../../CMakeLists.txt" \
  -PENGINE_HOST_ASSET_CODEGEN="$(pwd)/../../../../build-native/Debug/asset_codegen.exe"
```

(audio is on by default; pass `-PENGINE_WITH_AUDIO=OFF` to leave the mixer out.) Confirmed working end-to-end on a physical device (menu, touch input, gameplay, and sound) via `adb install`.

## Test

Domain logic (`Board`, `choose_bot_move`, `MatchController`) is covered by GoogleTest and runs without launching a window:

```bash
cmake --build build --target tic_tac_toe_tests --config Debug
ctest --test-dir build -C Debug -R "BoardTest|BotAiTest|MatchControllerTest" --output-on-failure
```

CI (`.github/workflows/ci.yml`) runs the same build-and-test steps, plus a full build of the `tic-tac-toe` target, on every push and pull request.

## Architecture

`Board` owns the 3×3 grid, turn/outcome rules, and scores behind a type-safe `Mark`-based API. `choose_bot_move` (`include/game/bot_ai.h`) is the PvE move-selection heuristic, kept separate from `Board`'s rules and independently testable. `MatchController` orchestrates a match on top of `Board`: menu/play screen state, click handling, and the post-victory pause before a round resets — plain game logic with no engine dependency. `MenuViewModel` and `PlayViewModel` each bridge one screen's state to its own declarative UI document (`assets/ui/menu.xml`, `assets/ui/play.xml`) via bindables and commands; `PlayPresenter` is the only place that knows how `Board` state maps onto `PlayViewModel`'s bindable text/images. `Game` is the thin engine adapter: it wires Wind's ECS/UI/audio systems, swaps the active UI document/view-model per screen, calls `PlayPresenter` to refresh the play screen, and fires audio cues.
