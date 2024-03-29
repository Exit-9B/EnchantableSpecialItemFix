@echo off

cmake --preset vs2019-windows --check-stamp-file "build/CMakeFiles/generate.stamp" || goto :error
cmake --build build --config Release || goto :error
cmake --install build --component "Fomod" --prefix "package" || goto :error
cmake --install build --component "Data" --prefix "package/Data" || goto :error
cmake --install build --component "Patches" --prefix "package/Patches" || goto :error
cmake --install build --component "SKSEPlugin" --prefix "package/SkyrimSE" || goto :error

cmake --preset vs2019-windows-vr --check-stamp-file "buildVR/CMakeFiles/generate.stamp" || goto :error
cmake --build buildVR --target "EnchantableSpecialItemFix" --config Release || goto :error
cmake --install buildVR --component "SKSEPlugin" --prefix "package/SkyrimVR" || goto :error

pushd package
7z a -r -t7Z "..\EnchantableSpecialItemFix.7z" *
popd
goto :EOF

:error
exit /b %errorlevel%
