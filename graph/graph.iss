; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define GTKPATH "C:\soft\msys64\mingw64\"
#define SITE "http://slovesnov.users.sf.net/"
#define APPNAME "graph"
#define VERSION "1.25"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A3A78A11-A8D9-4DC0-A8D3-19FEB424D97E}
;version number appears three times AppName shows application name in installer
AppName={#APPNAME}
AppVersion={#VERSION}
AppPublisher=Alexey Slovesnov
AppPublisherURL={#SITE}?{#APPNAME}
AppSupportURL={#SITE}?{#APPNAME}
AppUpdatesURL={#SITE}?{#APPNAME}
DefaultDirName={commonpf}\{#APPNAME}
DefaultGroupName={#APPNAME}
OutputBaseFilename={#APPNAME}{#VERSION}Setup
SetupIconFile={#APPNAME}\icon\app.ico
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Messages]
english.BeveledLabel=English
russian.BeveledLabel=Russian

[Files]
Source: "{#GTKPATH}share\icons\Adwaita\16x16\ui\pan-up-symbolic.symbolic.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: onlyifdoesntexist
Source: "{#GTKPATH}share\icons\Adwaita\16x16\ui\pan-down-symbolic.symbolic.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: onlyifdoesntexist
Source: "{#GTKPATH}share\icons\Adwaita\16x16\ui\pan-end-symbolic.symbolic.png"; DestDir: "{app}\share\icons\Adwaita\16x16\actions"; Flags: onlyifdoesntexist
;new gtk version so ignoreversion for dlls
Source: "{#GTKPATH}bin\*.dll"; DestDir: "{app}\bin"; Flags: ignoreversion
;gspawn-win64-helper.exe needs for gtk_show_uri_on_window() function
Source: "{#GTKPATH}bin\gspawn-win64-helper.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#GTKPATH}lib\gdk-pixbuf-2.0\2.10.0\loaders.cache"; DestDir: "{app}\lib\gdk-pixbuf-2.0\2.10.0"; Flags: ignoreversion
Source: "{#GTKPATH}lib\gdk-pixbuf-2.0\2.10.0\loaders\*.dll"; DestDir: "{app}\lib\gdk-pixbuf-2.0\2.10.0\loaders"; Flags: ignoreversion

Source: "Release\{#APPNAME}.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "*.css"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "{#APPNAME}\images\*"; Excludes: "*.txt"; DestDir: "{app}\bin\{#APPNAME}\images"; Flags: onlyifdoesntexist

[Icons]
Name: "{group}\{#APPNAME}"; Filename: "{app}\bin\{#APPNAME}.exe"
Name: "{commondesktop}\{#APPNAME}"; Filename: "{app}\bin\{#APPNAME}.exe"
Name: "{group}\{cm:UninstallProgram,{#APPNAME}}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\bin\{#APPNAME}.exe"; Description: "{cm:LaunchProgram,{#APPNAME}}"; Flags: nowait postinstall skipifsilent
