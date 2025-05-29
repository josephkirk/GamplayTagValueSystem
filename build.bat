@echo off
set UAT="C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\RunUAT.bat"
%UAT% BuildPlugin -plugin="%~dp0\GamplayTagValue.uplugin" -package="GamplayTagValue"