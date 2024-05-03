@echo off

dir /b *spare*.txt > list

rem Extract Year, Month and Day
for /f "tokens=1-3 delims=_" %%a in (list) do (
	set YEAR=%%a
    set MONTH=%%b
    set DAY=%%c
)


for /f "tokens=1,2 delims=." %%a in (list) do (
	rem Extract NMEA[$GPGGA] results(Chipset solutions) from *_spare_log.txt
	ExTxPos.exe %%a.%%b.txt
	rem Rename file to "nmea.pos"
	ren "%%a.%%b.pos" "%%a.%%b_nmea.txt"
	
	rem Convert gga file (from .txt to .pos)
	gga2pos.exe -in *nmea.txt -out %%a.%%b_nmea.pos -ts %YEAR%/%MONTH%/%DAY%
	
	rem  Convert .pos file to .kml file	 
	pos2kml.exe %%a.%%b_nmea.pos
)

