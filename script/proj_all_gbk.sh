#
dir=`dirname $0`
updir=`dirname $dir`
projdir=$updir/project
PATH="$updir/shell;$updir/tool"
echo $PATH
echo $projdir
for i in `find $projdir -type f -name "*.vcxproj" | grep -v "_old" `; do
	if [ -z "`grep "<PostBuildEvent>" $i`" ]; then
		cat $i | sed 's/a/b/g' 
		echo "==$i=="
	else
		echo "$i has post build, ignore it."
	fi
done