#*****************************************************************************#
#    Copyright (C) 2025 Julian Xhokaxhiu                                      #
#*****************************************************************************#

#*****************************************************************************#
#    This is free and unencumbered software released into the public domain.  #
#                                                                             #
#    Anyone is free to copy, modify, publish, use, compile, sell, or          #
#    distribute this software for any purpose, commercial or non-commercial,  #
#    and by any means.                                                        #
#                                                                             #
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.          #
#    For more information, please refer to <https://unlicense.org>            #
#*****************************************************************************#


Set-StrictMode -Version Latest

if ($env:_BUILD_BRANCH -eq "refs/heads/main" -Or $env:_BUILD_BRANCH -eq "refs/tags/canary")
{
  $env:_IS_BUILD_CANARY = "true"
  $env:_IS_GITHUB_RELEASE = "true"
}
elseif ($env:_BUILD_BRANCH -like "refs/tags/*")
{
  $env:_CHANGELOG_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')).Replace('.','')
  $env:_BUILD_VERSION = $env:_BUILD_VERSION.Substring(0,$env:_BUILD_VERSION.LastIndexOf('.')) + ".0"
  $env:_IS_GITHUB_RELEASE = "true"
}
$env:_RELEASE_VERSION = "${env:_BUILD_VERSION}"

$vcpkgRoot = ".\vcpkg"
$vcpkgBaseline = [string](jq --arg baseline "builtin-baseline" -r '.[$baseline]' vcpkg.json)
$vcpkgOriginUrl = &"git" -C $vcpkgRoot remote get-url origin
$vcpkgTagName = &"git" -C $vcpkgRoot describe --exact-match --tags

$releasePath = [string](jq -r '.configurePresets[0].binaryDir' CMakePresets.json).Replace('${sourceDir}/', '')

Write-Output "--------------------------------------------------"
Write-Output "BUILD CONFIGURATION: $env:_RELEASE_CONFIGURATION"
Write-Output "RELEASE VERSION: $env:_RELEASE_VERSION"
Write-Output "VCPKG ORIGIN: $vcpkgOriginUrl"
Write-Output "VCPKG TAG: $vcpkgTagName"
Write-Output "VCPKG BASELINE: $vcpkgBaseline"
Write-Output "--------------------------------------------------"

Write-Output "_BUILD_VERSION=${env:_BUILD_VERSION}" >> ${env:GITHUB_ENV}
Write-Output "_RELEASE_VERSION=${env:_RELEASE_VERSION}" >> ${env:GITHUB_ENV}
Write-Output "_IS_BUILD_CANARY=${env:_IS_BUILD_CANARY}" >> ${env:GITHUB_ENV}
Write-Output "_IS_GITHUB_RELEASE=${env:_IS_GITHUB_RELEASE}" >> ${env:GITHUB_ENV}
Write-Output "_CHANGELOG_VERSION=${env:_CHANGELOG_VERSION}" >> ${env:GITHUB_ENV}

# --- Detect previous release tag ---
$allTags = git tag
$currentTag = $env:_BUILD_BRANCH -replace '^refs/tags/', ''

# Get only new-style tags
$newTags = $allTags | Where-Object { $_ -match '^\d+\.\d{1,3}\.\d{1,3}\.?\d*$' }

# Pick the newest tag that is NOT the current tag
$prevTag = $newTags | Sort-Object { [version]($_) } -Descending |
           Where-Object { $_ -ne $currentTag } |
           Select-Object -First 1

# Fallback to legacy if no valid new tag found
if (-not $prevTag) {
    Write-Host "No valid new tags found, using legacy tags if available"
    $legacyTags = $allTags | Where-Object { $_ -match '^1\.0\.\d{4}$' }
    $prevTag = $legacyTags | Sort-Object { [version]($_) } -Descending |
               Select-Object -First 1
}

# If still nothing, fallback to 0.0.0.0
if (-not $prevTag) {
    Write-Host "No valid previous tag found, using fallback 0.0.0.0"
    $prevTag = "0.0.0.0"
}

Write-Host "Detected previous tag: $prevTag"
Add-Content -Path $env:GITHUB_OUTPUT -Value "prev_tag=$prevTag"


# --- Generate release notes ---
Write-Host "## What's Changed"
git log "$prevTag..HEAD" --pretty=format:"* %s by %an (%h)%n"

if ($env:_IS_BUILD_CANARY -eq "true") {
    Write-Host "`nFull Changelog: https://github.com/Sebanisu/Field-Map-Editor/compare/$prevTag...canary`n"
} else {
    Write-Host "`nFull Changelog: https://github.com/Sebanisu/Field-Map-Editor/compare/$prevTag...$currentTag`n"
}

# Load vcvarsall environment for x86
$vcvarspath = &"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -prerelease -latest -property InstallationPath
cmd.exe /c "call `"$vcvarspath\VC\Auxiliary\Build\vcvarsall.bat`" x86 && set > %temp%\vcvars.txt"
Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
  if ($_ -match "^(.*?)=(.*)$") {
    Set-Content "env:\$($matches[1])" $matches[2]
  }
}

# Unset VCPKG_ROOT if set
[Environment]::SetEnvironmentVariable('VCPKG_ROOT','')

# Add Github Packages registry
nuget sources add -Name github -Source "https://nuget.pkg.github.com/${env:GITHUB_REPOSITORY_OWNER}/index.json" -Username ${env:GITHUB_REPOSITORY_OWNER} -Password ${env:GITHUB_PACKAGES_PAT} -StorePasswordInClearText
nuget setApiKey ${env:GITHUB_PACKAGES_PAT} -Source "https://nuget.pkg.github.com/${env:GITHUB_REPOSITORY_OWNER}/index.json"
nuget sources list

# Vcpkg setup
cmd.exe /c "call $vcpkgRoot\bootstrap-vcpkg.bat"

vcpkg integrate install

# Start the build
cmake --preset "${env:_RELEASE_CONFIGURATION}"
cmake --build --preset "${env:_RELEASE_CONFIGURATION}"

# Start the packaging
7z a ".\.dist\${env:_RELEASE_NAME}-${env:_RELEASE_VERSION}.zip" ".\$releasePath\bin\${env:_RELEASE_CONFIGURATION}\*"

