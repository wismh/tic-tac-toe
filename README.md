# Tic-tac-toe (Wind)

Remake of the childhood GreenEngine **Tic Tac Toe**. Window is 3× the original (576×696). Neon board / X / O art, looping music, step cue, and X/O scores in the original HUD font.

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

Run `build/bin/Debug/tic-tac-toe.exe`. Mixer is on (`ENGINE_WITH_AUDIO`). Wind lives at `external/engine`. Engine gaps: [docs/engine-limits.md](docs/engine-limits.md).

## Test

Domain logic (`Board`, `choose_bot_move`, `MatchController`) is covered by GoogleTest and runs without launching a window:

```bash
cmake --build build --target tic_tac_toe_tests --config Debug
ctest --test-dir build -C Debug -R "BoardTest|BotAiTest|MatchControllerTest" --output-on-failure
```

CI (`.github/workflows/ci.yml`) runs the same build-and-test steps, plus a full build of the `tic-tac-toe` target, on every push and pull request.

## Architecture

`Board` owns the 3×3 grid, turn/outcome rules, and scores behind a type-safe `Mark`-based API. `choose_bot_move` (`include/game/bot_ai.h`) is the PvE move-selection heuristic, kept separate from `Board`'s rules and independently testable. `MatchController` orchestrates a match on top of `Board`: menu/play screen state, click handling, and the post-victory pause before a round resets — plain game logic with no engine dependency. `MenuViewModel` and `PlayViewModel` each bridge one screen's state to its own declarative UI document (`assets/ui/menu.xml`, `assets/ui/play.xml`) via bindables and commands; `PlayPresenter` is the only place that knows how `Board` state maps onto `PlayViewModel`'s bindable text/images. `Game` is the thin engine adapter: it wires Wind's ECS/UI/audio systems, swaps the active UI document/view-model per screen, calls `PlayPresenter` to refresh the play screen, and fires audio cues.
