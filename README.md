# YellowOffice

A native office productivity suite for Haiku OS.

## Overview

YellowOffice is a comprehensive office suite designed specifically for Haiku OS, utilizing native Haiku APIs and conventions. The suite aims to provide professional-grade productivity tools while maintaining the lightweight, efficient, and user-friendly nature of Haiku.

## Suite Applications

- **YellowDocs**: Full-featured word processor and document editor (primary focus)
- Future applications: Spreadsheets, Presentations, Drawing tools, Mail merge utilities

## Architecture

The project uses a monorepo structure with shared libraries to maximize code reuse across applications:

```
YellowOffice/
├── apps/
│   └── YellowDocs/          # Word processor application
│       ├── src/             # Application source code
│       ├── resources/       # Icons, layouts, etc.
│       ├── tests/           # Application tests
│       └── docs/            # Application-specific docs
├── libraries/
│   ├── YellowDocumentCore/  # Core document model
│   ├── YellowDocumentLayout/# Document layout engine
│   ├── YellowImportExport/  # Import/export adapters
│   ├── YellowUI/            # Shared UI components
│   └── YellowCommon/        # Common utilities
├── packaging/
│   └── hpkg/                # Haiku package files
├── build/                   # Build system files
├── docs/                    # Suite documentation
└── scripts/                 # Build and packaging scripts
```

## Building

### Prerequisites

- Haiku OS (latest nightly recommended)
- Haiku Development Tools
- Git

### Build Instructions

1. **Using Haiku's native build system (Jam):**
   ```bash
   cd /path/to/YellowOffice
   jam -q
   ```

2. **Using CMake (alternative):**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

### Build Options

- `ENABLE_TESTS`: Build unit tests (default: ON)
- `ENABLE_DOCS`: Build documentation (default: OFF)
- `INSTALL_PREFIX`: Installation directory (default: /boot/common)

## Packaging

YellowOffice uses Haiku's native HPKG package format for distribution.

To create packages:
```bash
cd /path/to/YellowOffice
./scripts/package.sh
```

## Installation

1. Build the project
2. Run the package creation script
3. Install using Haiku's PackageManager or `pkgman`:
   ```bash
   pkgman install YellowOffice-*.hpkg
   ```

## Documentation

- [Architecture Overview](docs/ARCHITECTURE.md)
- [API Documentation](docs/API.md)
- [File Formats](docs/FORMATS.md)
- [Development Guide](docs/DEVELOPMENT.md)

## Contributing

Contributions are welcome! Please follow the [Contribution Guidelines](docs/CONTRIBUTING.md).

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.

## Roadmap

See [ROADMAP.md](docs/ROADMAP.md) for development milestones and planned features.

---

**Note**: This is a work in progress. Current focus is on YellowDocs as the foundation for the suite.
