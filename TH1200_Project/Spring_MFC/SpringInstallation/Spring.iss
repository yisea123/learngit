; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{9E40ED1B-01D7-4453-987B-AA3C40FDC711}
AppName=总线控制系统
AppVersion=1.6
AppPublisher=ADTECH
AppPublisherURL=http://www.adtechcn.com/
AppSupportURL=http://www.adtechcn.com/
AppUpdatesURL=http://www.adtechcn.com/
DefaultDirName={pf}\ADTECH\总线控制系统
DefaultGroupName=ADTECH\总线控制系统
DisableProgramGroupPage=yes
InfoBeforeFile=ReleaseNote.txt
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "..\Release\Spring.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "help.mht"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\总线控制系统"; Filename: "{app}\Spring.exe";
Name: "{group}\{cm:UninstallProgram,总线控制系统}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\总线控制系统"; Filename: "{app}\Spring.exe"; Tasks: desktopicon;
Name: "{commondesktop}\总线控制系统手册"; Filename: "{app}\help.mht"; Tasks: desktopicon;
Name: "{commonstartup}\总线控制系统"; Filename: "{app}\Spring.exe"

[Run]
Filename: "{app}\Spring.exe"; Description: "{cm:LaunchProgram,总线控制系统}"; Flags: nowait postinstall skipifsilent

