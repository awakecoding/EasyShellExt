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

```pwsh
Register-ShellExtension -FilePath "$PWD/EasyShellExt.dll" -CLSID "{B0D35103-86A1-471C-A653-E130E3439A3B}" -ExtensionName "EasyShellExt-awakecoding"
Unregister-ShellExtension -CLSID "{B0D35103-86A1-471C-A653-E130E3439A3B}"
```
