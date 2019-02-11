@echo off
cd /d %~dp0

for /f "tokens=*" %%i in ('dir /ad /b build-*') do (
	CALL:X_DEL_DIR "%%i"
)

CALL:X_VS_PROJ_CLEARN_FUNC .
for /f "tokens=*" %%i in ('dir /ad /b *') do (
	CALL:X_VS_PROJ_CLEARN_FUNC %%i
)

del bin\*.ilk
del bin\*.exp
CALL:X_DEL_DIR bin\log


goto:eof

:X_VS_PROJ_CLEARN_FUNC
rd %~1\Debug /s /q
rd %~1\Release /s /q
rd %~1\GeneratedFiles /s /q
rd %~1\Win32 /s /q
rd %~1\x64 /s /q
rd %~1\.vs /s /q
del %~1\*.sdf
del %~1\*.VC.db
GOTO:EOF

:X_DEL_DIR
rd /s /q "%~1" 
GOTO:EOF
