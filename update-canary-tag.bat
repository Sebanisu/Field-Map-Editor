@echo off
REM Delete the Canary tag locally (if it exists)
git tag -d Canary

REM Delete the Canary tag on GitHub
git push origin :refs/tags/Canary

REM Create a new Canary tag on the latest commit
git tag Canary

REM Force push the new Canary tag to GitHub
git push origin Canary --force
