; ArcadeGame Installer
#define MyAppName "Arcade Game by Unjin"
#define MyAppVersion "1.0"
#define MyAppPublisher "Unjin"
#define MyAppExeName "ArcadeGame.exe"

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\ArcadeGame
DefaultGroupName=Arcade Game
OutputDir=installer_output
OutputBaseFilename=ArcadeGame_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=assets\icon.ico  ; если у тебя есть иконка, иначе удали строку

[Files]
; Основной exe и папки
Source: "ArcadeGame.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs
Source: "sounds\*"; DestDir: "{app}\sounds"; Flags: ignoreversion recursesubdirs

; ВСЕ DLL из ucrt64/bin (добавь сюда всё что нужно, я поставил основные)
Source: "C:\msys64\ucrt64\bin\libgtk-4-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libgobject-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libglib-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libgdk_pixbuf-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libgio-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libgdk-4-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libpango-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libcairo-2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\msys64\ucrt64\bin\libgstreamer-1.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
; ... добавь остальные по списку выше или через Dependencies.exe

; Плагины GStreamer (обязательно, иначе звук не будет работать)
Source: "C:\msys64\ucrt64\lib\gstreamer-1.0\*"; DestDir: "{app}\lib\gstreamer-1.0"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; Flags: nowait postinstall skipifsilent