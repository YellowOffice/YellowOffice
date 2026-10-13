# YellowOffice

A native office suite for [Haiku OS](https://www.haiku-os.org/), built with
the Haiku API. The first application is **YellowDocs**, a word processor.

## Status

Work in progress, hosted development:

- The core document model, layout engine, import/export, and the YellowDocs
  app all compile and are covered by unit tests (see *Developing* below).
- Formats implemented: native **YDF** (zip + JSON), **Markdown**, plain text,
  and **OpenDocument Text** (ODF).
- The UI has been developed and syntax-checked against the Haiku API, but has
  not yet been run on a real Haiku system — expect rough edges.

## Layout

```
YellowOffice/
├── apps/YellowDocs/         Word processor application
│   ├── src/                 App, window, document canvas
│   └── resources/           rdef, generated icons (scripts/generate-icons.py)
├── libraries/
│   ├── YellowCommon/        Logger, JSON, error handling, defines
│   ├── YellowDocumentCore/  Document model, commands, styles, undo/redo
│   ├── YellowDocumentLayout/Layout engine, text/paragraph layout, selection
│   ├── YellowImportExport/  Format registry; YDF, Markdown, text, ODF I/O
│   └── YellowUI/            Shared UI bits (ToolBar, ActionManager)
├── build/haiku-stub/        Header stubs so non-UI code builds off-Haiku
├── docs/                    Architecture notes
├── packaging/hpkg/          .PackageInfo for the HPKG package
├── scripts/                 syntax-check.sh, package.sh, generate-icons.py
└── tests/                   Host-built unit tests
```

## Building

### On Haiku

Requires cmake, gcc, zlib headers. All libraries build static and link into
the app.

```sh
cmake -S . -B build/haiku -DCMAKE_BUILD_TYPE=Release
cmake --build build/haiku -j 4
# binary: build/haiku/apps/YellowDocs/YellowDocs
```

### Off-Haiku (development)

The non-UI libraries build against header stubs (`build/haiku-stub`) so the
model, layout, and I/O code can be compiled and tested on any host:

```sh
make test       # configure, build, run unit tests
make syntax     # -fsyntax-only over every project source file
```

## Packaging

On Haiku:

```sh
scripts/package.sh
# -> dist/YellowDocs-0.1.0-1-x86_64.hpkg
```

The script builds Release, compiles `YellowDocs.rdef` (app signature, file
types, HVIF icon) with `rc`, attaches it with `xres`, and runs
`package create`. The package description is
`packaging/hpkg/YellowDocs.PackageInfo`.

## Regenerating icons

```sh
python3 scripts/generate-icons.py
```

Writes the PNG icons and `YellowDocs.rdef` (including the embedded HVIF app
icon) under `apps/YellowDocs/resources/`.

## License

MIT — see [LICENSE](LICENSE).
