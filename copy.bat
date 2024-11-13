::copy nvidia texture tools to app dir
xcopy /y "%~dp0nvtt" "%~dp0bin\debug\nvtt\" /E /Q
xcopy /y "%~dp0nvtt" "%~dp0bin\release\nvtt\" /E /Q
:: copy icons folder to app dir
xcopy /y "%~dp0icons_fluent" "%~dp0bin\debug\icons_fluent\" /E /Q
xcopy /y "%~dp0icons_fluent" "%~dp0bin\release\icons_fluent\" /E /Q