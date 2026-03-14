; MachOExplorer Windows installer (Inno Setup)
; Build installer:
;   iscc packaging/windows/MachOExplorer.iss

#ifndef MyAppName
  #define MyAppName "MachOExplorer"
#endif
#ifndef MyAppVersion
  #define MyAppVersion "2.0.0"
#endif
#define MyAppPublisher "everettjf"
#define MyAppURL "https://github.com/everettjf/MachOExplorer"
#define MyAppExeName "MachOExplorer.exe"
#ifndef MyAppSourceRoot
  #define MyAppSourceRoot "..\\..\\dist\\windows\\MachOExplorer"
#endif

[Setup]
AppId={{D9D5FF2C-7BEA-4FC0-A00A-2A80B849FA63}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/releases
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
OutputDir=..\..\dist\windows\installer
OutputBaseFilename=MachOExplorer-Setup-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#MyAppSourceRoot}\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
