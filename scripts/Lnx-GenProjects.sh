dp0=$(dirname $(readlink -f $0))
cd "$dp0/.."
./vendor/premake/bin/premake5-linux gmake2