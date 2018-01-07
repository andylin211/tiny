#
dir=`dirname $0`
updir=`dirname $dir`
PATH="$updir/shell;$updir/tool"
ver=`date +%Y.%m.%d.%H`

if [ -e "$1" ]; then
	echo "update file version:"
	version -f $1 -v $ver
	version -f $1
else
	echo "usage: version.sh <file>"
fi
