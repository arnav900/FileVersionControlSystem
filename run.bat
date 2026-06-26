@echo off
g++ -std=c++11 main.cpp -o main.exe
if errorlevel 1 (
  echo Compilation failed.
  pause
  exit /b 1
)
echo Compilation successful. Running program...
main.exe
pause