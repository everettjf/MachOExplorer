# Release Packaging Notes

## macOS Homebrew (Cask)

Use:

```bash
scripts/release_homebrew.sh --version 2.0.0 --dmg /path/to/MachOExplorer.dmg
```

Test without pushing:

```bash
scripts/release_homebrew.sh --version 2.0.0 --dry-run
```

Default target tap:

- `everettjf/homebrew-tap`
- cask path: `Casks/machoexplorer.rb`

## Windows Installer (2026 recommendation)

For a classic "Next/Next/Finish" installer in 2026:

- Primary recommendation: **Inno Setup** (stable, lightweight, scriptable, easy CI automation).
- Enterprise/Store channel option: **MSIX + winget**.

This repo includes:

- Inno Setup script: `packaging/windows/MachOExplorer.iss`
- Build helper: `scripts/build_windows_installer.ps1`

Windows packaging flow:

1. Build `MachOExplorer.exe` with CMake.
2. Run `windeployqt` to stage Qt runtime files.
3. Run `iscc` to generate installer executable.
