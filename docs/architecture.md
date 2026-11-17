# Project architecture

```text
BrilliantChessEngine/
├── engine/
│   ├── include/bce/       # Evaluation and search public headers
│   └── src/               # Implementations mirroring include/bce/
├── apps/
│   ├── uci/               # UCI protocol executable
│   ├── cli/               # Local developer command-line executable
│   ├── lichess/           # Future Lichess bot client
│   └── web/               # Future C++ to WebAssembly adapter
├── tests/
│   ├── unit/              # Small isolated tests
│   ├── perft/             # Legal-move-generation regression positions
│   ├── integration/       # End-to-end UCI and application tests
│   └── fixtures/          # FEN, PGN, and other test data
├── third_party/           # Notes about dependencies fetched by CMake
├── web/
│   ├── assets/            # CSS, browser JS, images, and generated WASM
│   └── index.html         # GitHub Pages entry point
└── docs/                  # Design notes and developer documentation
```

`chess-library` is the single source of truth for chess positions and legal
moves. The `engine` library must not depend on a protocol, GUI, Lichess, or
browser code. Applications depend on `engine`, never the reverse. This keeps
tests fast and lets the same evaluation and search logic power every interface.
