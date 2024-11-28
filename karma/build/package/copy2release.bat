
echo AWFUL CHECKOUT-2-RELEASE MUNGER


echo Removing old include copies

del /S /Q %KARMA_DIR%\include
rmdir %KARMA_DIR%\include

echo Copying files to 'release' locations

cd %KARMA_DIR%
mkdir include
copy %KARMA_DIR%\Mcd\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\Mst\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\Mdt\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MdtKea\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MdtBcl\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeGlobals\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeAssetDB\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeAssetDBXMLIO\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeAssetFactory\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeXML\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeViewer2\include\* %KARMA_DIR%\include
copy %KARMA_DIR%\MeApp\include\* %KARMA_DIR%\include


