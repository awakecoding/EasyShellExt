# EasyShellExt
A windows shell extension sample.

Update `CustomImpl.h`, `CustomImpl.cpp` files and add your custom configurations and logic. 

Register.reg:

```text
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}]
@="EasyShellExt-awakecoding"

[HKEY_CLASSES_ROOT\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}\InprocServer32]
@="C:\\Windows\\System32\\EasyShellExt.dll"
"ThreadingModel"="Apartment"

[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}]
@="EasyShellExt-awakecoding"

[HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}\InprocServer32]
@="C:\\Windows\\System32\\EasyShellExt.dll"
"ThreadingModel"="Apartment"

[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved]
"{B0D35103-86A1-471C-A653-E130E3439A3B}"="EasyShellExt-awakecoding"
```

Unregister.reg:

```text
Windows Registry Editor Version 5.00

[-HKEY_CLASSES_ROOT\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}]

[-HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{B0D35103-86A1-471C-A653-E130E3439A3B}]

[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved]
"{B0D35103-86A1-471C-A653-E130E3439A3B}"=-
```

```powershell
function Register-ShellExtension {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter(Mandatory = $true)]
        [string]$CLSID,

        [Parameter(Mandatory = $false)]
        [string]$ExtensionName = "CustomShellExtension"
    )

    # Validate that the file exists
    if (!(Test-Path $FilePath)) {
        Write-Error "File path '$FilePath' does not exist."
        return
    }

    # Register in HKEY_CLASSES_ROOT
    $clsidPathHKCR = "Registry::HKEY_CLASSES_ROOT\CLSID\$CLSID"
    New-Item -Path $clsidPathHKCR -Force | Out-Null
    Set-ItemProperty -Path $clsidPathHKCR -Name "(Default)" -Value $ExtensionName

    $inprocPathHKCR = "$clsidPathHKCR\InprocServer32"
    New-Item -Path $inprocPathHKCR -Force | Out-Null
    Set-ItemProperty -Path $inprocPathHKCR -Name "(Default)" -Value $FilePath
    Set-ItemProperty -Path $inprocPathHKCR -Name "ThreadingModel" -Value "Apartment"

    # Register in HKEY_LOCAL_MACHINE\SOFTWARE\Classes
    $clsidPathHKLM = "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\$CLSID"
    New-Item -Path $clsidPathHKLM -Force | Out-Null
    Set-ItemProperty -Path $clsidPathHKLM -Name "(Default)" -Value $ExtensionName

    $inprocPathHKLM = "$clsidPathHKLM\InprocServer32"
    New-Item -Path $inprocPathHKLM -Force | Out-Null
    Set-ItemProperty -Path $inprocPathHKLM -Name "(Default)" -Value $FilePath
    Set-ItemProperty -Path $inprocPathHKLM -Name "ThreadingModel" -Value "Apartment"

    # Approve the shell extension
    $approvedPath = "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved"
    New-Item -Path $approvedPath -Force | Out-Null
    Set-ItemProperty -Path $approvedPath -Name $CLSID -Value $ExtensionName

    Write-Host "Shell extension registered successfully!" -ForegroundColor Green
}

function Unregister-ShellExtension {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory = $true)]
        [string]$CLSID
    )

    Write-Host "Unregistering shell extension with CLSID: $CLSID" -ForegroundColor Yellow

    # Remove registry keys in HKEY_CLASSES_ROOT
    $clsidPathHKCR = "Registry::HKEY_CLASSES_ROOT\CLSID\$CLSID"
    if (Test-Path $clsidPathHKCR) {
        Remove-Item -Path $clsidPathHKCR -Force -Recurse -ErrorAction SilentlyContinue
        Write-Host "Removed from HKCR:\CLSID" -ForegroundColor Green
    }

    # Remove registry keys in HKEY_LOCAL_MACHINE\SOFTWARE\Classes
    $clsidPathHKLM = "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\$CLSID"
    if (Test-Path $clsidPathHKLM) {
        Remove-Item -Path $clsidPathHKLM -Force -Recurse -ErrorAction SilentlyContinue
        Write-Host "Removed from HKLM:\SOFTWARE\Classes\CLSID" -ForegroundColor Green
    }

    # Remove the shell extension approval entry
    $approvedPath = "Registry::HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved"
    if (Get-ItemProperty -Path $approvedPath -Name $CLSID -ErrorAction SilentlyContinue) {
        Remove-ItemProperty -Path $approvedPath -Name $CLSID -ErrorAction SilentlyContinue
        Write-Host "Removed from Shell Extensions\Approved" -ForegroundColor Green
    }

    Write-Host "Shell extension unregistered successfully!" -ForegroundColor Cyan
}
```

```powershell
Register-ShellExtension -FilePath "$PWD/EasyShellExt.dll" -CLSID "{B0D35103-86A1-471C-A653-E130E3439A3B}" -ExtensionName "EasyShellExt-awakecoding"
Unregister-ShellExtension -CLSID "{B0D35103-86A1-471C-A653-E130E3439A3B}"
```

```powershell
function Register-ExplorerCommand {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory = $true)]
        [string]$FilePath,

        [Parameter(Mandatory = $true)]
        [string]$CLSID,

        [Parameter(Mandatory = $true)]
        [string]$Verb,

        [Parameter(Mandatory = $false)]
        [string]$MenuText = "Run Elevated",

        [Parameter(Mandatory = $false)]
        [string[]]$Extensions = @(".exe", ".msi", ".lnk", ".ps1", ".bat")  # Restrict to these
    )

    # Validate the DLL Path
    if (!(Test-Path $FilePath)) {
        Write-Error "ERROR: DLL path '$FilePath' does not exist. Exiting."
        return
    }

    Write-Host "✅ DLL Path verified: $FilePath" -ForegroundColor Green

    # Register CLSID in HKEY_CLASSES_ROOT\CLSID
    $clsidPathHKCR = "Registry::HKEY_CLASSES_ROOT\CLSID\$CLSID"
    if (Test-Path $clsidPathHKCR) {
        Write-Host "⚠️ CLSID already exists in registry: $CLSID" -ForegroundColor Yellow
    } else {
        Write-Host "🆕 Registering CLSID: $CLSID" -ForegroundColor Cyan
        New-Item -Path $clsidPathHKCR -Force | Out-Null
        Set-ItemProperty -Path $clsidPathHKCR -Name "(Default)" -Value "PedmShellExt"
        Write-Host "✅ CLSID registered in HKCR" -ForegroundColor Green
    }

    # Register InprocServer32
    $inprocPathHKCR = "$clsidPathHKCR\InprocServer32"
    if (!(Test-Path $inprocPathHKCR)) {
        Write-Host "🆕 Registering InprocServer32..." -ForegroundColor Cyan
        New-Item -Path $inprocPathHKCR -Force | Out-Null
        Set-ItemProperty -Path $inprocPathHKCR -Name "(Default)" -Value $FilePath
        Set-ItemProperty -Path $inprocPathHKCR -Name "ThreadingModel" -Value "Apartment"
        Write-Host "✅ InprocServer32 registered" -ForegroundColor Green
    }

    # Register Explorer Command for Specific File Extensions
    foreach ($ext in $Extensions) {
        $extKeyPath = "Registry::HKEY_CLASSES_ROOT\$ext"
        $commandPath = "Registry::HKEY_CLASSES_ROOT\$ext\shell\$Verb"

        # Ensure the extension exists before modifying it
        if (!(Test-Path $extKeyPath)) {
            Write-Host "❌ Skipping $ext`: No registry key found for this extension." -ForegroundColor Red
            continue
        }

        # Ensure the shell key exists
        if (!(Test-Path "$extKeyPath\shell")) {
            Write-Host "🆕 Creating missing 'shell' key for $ext..." -ForegroundColor Yellow
            New-Item -Path "$extKeyPath\shell" -Force | Out-Null
        }

        Write-Host "🆕 Registering ExplorerCommand for: $ext at $commandPath" -ForegroundColor Cyan

        # Remove existing entry if necessary
        Remove-Item -Path $commandPath -Force -Recurse -ErrorAction SilentlyContinue
        Start-Sleep -Milliseconds 500  # Allow Windows to release any locks

        # Add new command entry
        New-Item -Path $commandPath -Force | Out-Null
        Set-ItemProperty -Path $commandPath -Name "(Default)" -Value $MenuText
        Set-ItemProperty -Path $commandPath -Name "ExplorerCommandHandler" -Value $CLSID
    }

    Write-Host "✅ ExplorerCommand registered successfully for selected file types!" -ForegroundColor Green
}

function Unregister-ExplorerCommand {
    [CmdletBinding()]
    param (
        [Parameter(Mandatory = $true)]
        [string]$CLSID,

        [Parameter(Mandatory = $true)]
        [string]$Verb,

        [Parameter(Mandatory = $false)]
        [string[]]$Extensions = @(".exe", ".msi", ".lnk", ".ps1", ".bat")  # Restrict to these
    )

    Write-Host "Unregistering Classic ExplorerCommand with CLSID: $CLSID" -ForegroundColor Yellow

    # Remove CLSID registration
    $clsidPathHKCR = "Registry::HKEY_CLASSES_ROOT\CLSID\$CLSID"
    if (Test-Path $clsidPathHKCR) {
        Remove-Item -Path $clsidPathHKCR -Force -Recurse -ErrorAction SilentlyContinue
        Write-Host "✅ Removed CLSID from HKCR" -ForegroundColor Green
    } else {
        Write-Host "⚠️ CLSID not found in HKCR, skipping." -ForegroundColor Yellow
    }

    # Remove ExplorerCommand registry entry for specific file types
    foreach ($ext in $Extensions) {
        $commandPath = "HKEY_CLASSES_ROOT\$ext\shell\$Verb"
        Write-Host "🗑 Removing ExplorerCommand for: $ext at $commandPath" -ForegroundColor Cyan

        cmd.exe /c "reg delete `"$commandPath`" /f" | Out-Null
        Start-Sleep -Milliseconds 500  # Ensure registry updates are processed
    }

    Write-Host "✅ Classic ExplorerCommand unregistered successfully!" -ForegroundColor Cyan
}
```

```powershell
Register-ExplorerCommand -FilePath "$PWD/devolutions_pedm_shell_ext.dll" -CLSID "{0ba604fd-4a5a-4abb-92b1-09ac5c3bf356}" -Verb "RunElevated" -MenuText "Run Elevated"

Unregister-ExplorerCommand -CLSID "{0ba604fd-4a5a-4abb-92b1-09ac5c3bf356}" -Verb "RunElevated"
```
