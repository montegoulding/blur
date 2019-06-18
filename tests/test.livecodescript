script "Testblur"

local sRepoRoot

on TestSetup
	local tMode
	if $MODE is empty then
		put "Release" into tMode
	else
		put $MODE into tMode
	end if

	local tExtension, tSuffix, tPath
	switch the platform
	case "linux"
		put "so" into tExtension
		put "build" into tPath
		if the processor is "x86_64" then
			put "-x64" into tSuffix
		else
			put "-x86" into tSuffix
		end if
		break 
	case "macOS"
		put "bundle" into tExtension
		put "build/" & tMode into tPath
		break 
	case "win32"
		put "dll" into tExtension
		if the processor is "x86_64" then
			put "_build/" & tMode & "/x64" into tPath
			put "-x64" into tSuffix
		else
			put "_build/" & tMode & "/Win32" into tPath
			put "-x86" into tSuffix
		end if
		break 
	end switch

	set the itemDelimiter to slash
	put item 1 to -3 of the filename of me into sRepoRoot
	TestDiagnostic sRepoRoot & return & the filename of me

	_TestLoadExternal format("%s/%s/blur%s.%s", \
		sRepoRoot, \
		tPath,\
		tSuffix, \
		tExtension)
end TestSetup

on _TestLoadExternal pExternal
	TestDiagnostic pExternal
	set the externals of the templateStack to pExternal

	create stack pExternal && "External"
	start using it
	if the externalCommands of it is empty and \
			the externalFunctions of it is empty then
		write "Cannot load external" && pExternal & return to stderr
		quit 1
	end if
end _TestLoadExternal

on TestBlur
	TestSkipIfNot "ui"

	import snapshot from rectangle the screenRect

	local theOriginalImageData,theNewImageData
	put the imageData of image 1 into theOriginalImageData
	blur 5, \
		"theOriginalImageData", \
		the width of image 1, \
		the height of image 1, "theNewImageData"
	
	TestAssert "Blur did something", \
		theNewImageData is not empty and \
		theNewImageData is not theOriginalImageData
end TestBlur