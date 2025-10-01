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

# Use ASCII Unit Separator as delimiter
$delimiter = "`u{001F}"

# Get logs with the delimiter
$logEntries = git log "$prevTag..HEAD" --pretty=format:"%h$delimiter%s$delimiter%an$delimiter%d"

$formattedNotes = "## What's Changed`n"

foreach ($entry in $logEntries) {
    $parts = $entry -split $delimiter
    $sha = $parts[0]
    $message = $parts[1]
    $author = $parts[2]
    $refs = $parts[3]

    if ($refs -match "pull/(\d+)") {
        $prNumber = $matches[1]
        $formattedNotes += "* $message by @$author in https://github.com/Sebanisu/Field-Map-Editor/pull/$prNumber`n"
    } else {
        $formattedNotes += "* $message by @$author`n"
    }
}

# Append full changelog link
$formattedNotes += "`nFull Changelog: https://github.com/Sebanisu/Field-Map-Editor/compare/$prevTag...canary`n"

# Set output for GitHub Actions
Add-Content -Path $env:GITHUB_OUTPUT -Value "prev_tag=$prevTag"
Add-Content -Path $env:GITHUB_OUTPUT -Value "release_notes=$formattedNotes"

Write-Host "Generated release notes:"
Write-Host $formattedNotes


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

