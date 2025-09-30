@echo off
REM Delete the Canary tag locally (if it exists)
git tag -d canary

REM Delete the Canary tag on GitHub
git push origin :refs/tags/canary

REM Create a new Canary tag on the latest commit
REM git tag Canary

REM Force push the new Canary tag to GitHub
REM git push origin Canary --force
