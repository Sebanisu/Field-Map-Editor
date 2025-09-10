$clang = "C:/Program Files/LLVM/bin/clang-format.exe"  # ensure it's in PATH

# Get all tracked source files (respects .gitignore automatically)
$files = git ls-files *.cpp *.hpp *.h *.c *.cc *.hh

foreach ($f in $files) {
    Write-Output "Formatting $f"
    & $clang -i --style=file $f
}

Write-Output "Done!"
