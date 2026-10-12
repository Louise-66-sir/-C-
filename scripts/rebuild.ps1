# Run in PowerShell after installing Node.js. C sources and notes are maintained separately.
$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent $PSScriptRoot
& node (Join-Path $PSScriptRoot 'build.mjs')
if ($LASTEXITCODE -ne 0) { throw 'HTML and Notebook generation failed.' }
$sourceFiles = @(Get-ChildItem -LiteralPath (Join-Path $repositoryRoot 'c') -File -Force | Where-Object { $_.Extension -in '.c', '.h', '.md' -or $_.Name -eq '.clang-format' } | Select-Object -ExpandProperty FullName)
Compress-Archive -LiteralPath $sourceFiles -DestinationPath (Join-Path $repositoryRoot 'ds408-c-sources.zip') -Force
Write-Output 'Updated index.html, ds408-notes.ipynb, and ds408-c-sources.zip.'
