; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=musikCube
AppVerName=musikCube a2
AppPublisher=musik Development Team
AppPublisherURL=http://www.musikcube.com
AppSupportURL=http://www.musikcube.com
AppUpdatesURL=http://www.musikcube.com
DefaultDirName={pf}\musikCube
DefaultGroupName=musikCube
AllowNoIcons=yes
LicenseFile=license.txt
WizardSmallImageFile=musikCubeSetup.bmp
OutputBaseFilename=musikCubeSetup_a2
ChangesAssociations=yes

[Tasks]
; NOTE: The following entry contains English phrases ("Create a desktop icon" and "Additional icons"). You are free to translate them into another language if required.
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"
; NOTE: The following entry contains English phrases ("Create a Quick Launch icon" and "Additional icons"). You are free to translate them into another language if required.
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "sqlite.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "fmod.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "musik.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "license.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "changelog.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "musik.exe.manifest"; DestDir: "{app}"; Flags: ignoreversion
Source: "musik.ttf"; DestDir: "{fonts}"; FontInstall: "musikCube TrueType Font"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\musikCube"; Filename: "{app}\musik.exe"
; NOTE: The following entry contains an English phrase ("Uninstall"). You are free to translate it into another language if required.
Name: "{group}\Uninstall musikCube"; Filename: "{uninstallexe}"
Name: "{group}\changelog.txt"; Filename: "{app}\changelog.txt"
Name: "{userdesktop}\musikCube"; Filename: "{app}\musik.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\musikCube"; Filename: "{app}\musik.exe"; Tasks: quicklaunchicon

[Run]
; NOTE: The following entry contains an English phrase ("Launch"). You are free to translate it into another language if required.
Filename: "{app}\musik.exe"; Description: "Launch musikCube"; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKCR; SubKey: .ogg; ValueType: string; ValueData: OGG; Flags: uninsdeletekey
Root: HKCR; SubKey: OGG; ValueType: string; ValueData: OGG Vorbis Media File; Flags: uninsdeletekey
Root: HKCR; SubKey: OGG\Shell\Open\Command; ValueType: string; ValueData: """{app}\musik.exe"" ""%1"""; Flags: uninsdeletevalue
Root: HKCR; Subkey: OGG\DefaultIcon; ValueType: string; ValueData: {app}\musik.exe,0; Flags: uninsdeletevalue
Root: HKCR; SubKey: .mp3; ValueType: string; ValueData: MP3; Flags: uninsdeletekey
Root: HKCR; SubKey: MP3; ValueType: string; ValueData: MP3 Media File; Flags: uninsdeletekey
Root: HKCR; SubKey: MP3\Shell\Open\Command; ValueType: string; ValueData: """{app}\musik.exe"" ""%1"""; Flags: uninsdeletevalue
Root: HKCR; Subkey: MP3\DefaultIcon; ValueType: string; ValueData: {app}\musik.exe,0; Flags: uninsdeletevalue
