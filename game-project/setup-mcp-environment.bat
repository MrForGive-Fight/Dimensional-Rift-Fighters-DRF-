@echo off
:: Windows MCP Environment Setup Launcher
:: This wrapper ensures PowerShell script runs with proper permissions

echo Starting MCP Environment Setup for Windows...
echo.

:: Check if running as Administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo This script needs to run as Administrator for some installations.
    echo Right-click and select "Run as administrator"
    echo.
    pause
)

:: Run PowerShell script with execution policy bypass
powershell -ExecutionPolicy Bypass -File "%~dp0setup-mcp-environment.ps1"

pause