# Release Packaging Notes

## macOS Homebrew (Cask)

## macOS Signed + Notarized Release

`deploy.sh` now assumes a real macOS distribution flow:

1. build `MachOExplorer.app`
2. run `macdeployqt`
3. sign the `.app` with `Developer ID Application`
4. create the `.dmg`
5. submit the `.dmg` to Apple notarization
6. staple the notarization ticket
7. verify with `codesign`, `spctl`, and `stapler`

Required environment variables:

```bash
export CODE_SIGN_IDENTITY="Developer ID Application: Feng Zhu (YPV49M8592)"
export NOTARYTOOL_PROFILE="<keychain-profile>"
```

Or use direct Apple credentials instead of `NOTARYTOOL_PROFILE`:

```bash
export APPLE_ID="<apple-id>"
export APPLE_TEAM_ID="<team-id>"
export APPLE_APP_SPECIFIC_PASSWORD="<app-specific-password>"
```

Recommended notarization credential setup:

```bash
xcrun notarytool store-credentials "<profile-name>" \
  --apple-id "<apple-id>" \
  --team-id "<team-id>" \
  --password "<app-specific-password>"
```

Then run:

```bash
NOTARYTOOL_PROFILE="<profile-name>" ./deploy.sh
```

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
