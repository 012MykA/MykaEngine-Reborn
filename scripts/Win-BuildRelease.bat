cd ..\build

msbuild MykaSolution.slnx /t:Build /p:Configuration=Release /p:Platform=x64

pause