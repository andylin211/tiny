#
dir=`dirname $0`
updir=`dirname $dir`
PATH="$updir/shell;$updir/tool"
echo $PATH
for i in `find $updir -type f -name "*.vcxproj" | grep -v "_old" `; do
	if [ "utf8" = "`encode -f $i`" ]; then
		encode -f $i -c ansi -o $i
		encode -f $i
	fi
	sed 's/encoding="utf-8"/encoding="gb2312"/g' $i > $i.bak
	cat $i.bak > $i
	rm -f $i.bak
done