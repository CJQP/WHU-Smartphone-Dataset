@echo off
dir /b *.txt > list
for /f "tokens=1,2 delims=." %%a in (list) do (
	 ..\..\..\ExTxPos.exe %%a.%%b.txt
	 ..\..\..\gga2pos.exe -in %%a.%%b.pos -out %%a.%%b-1.pos -ts 2020/09/15
	 del %%a.%%b.pos	 
	 ..\..\..\pos2kml.exe %%a.%%b-1.pos
)


