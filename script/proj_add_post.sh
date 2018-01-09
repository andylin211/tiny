#
dir=`dirname $0`
updir=`dirname $dir`
PATH="$updir/shell;$updir/tool"
echo $PATH
for i in `find $updir -type f -name "*.vcxproj" | grep -v "_old" `; do
	if [ -z "`grep "<PostBuildEvent>" $i`" ]; then
		cat $i | sed 's/<\/ItemDefinitionGroup>/<PostBuildEvent><Command>\$(SolutionDir)shell\/bash\.exe \$(SolutionDir)script\/post\.sh \$(OutDir)\$(TargetName)\$(TargetExt)<\/Command><\/PostBuildEvent><\/ItemDefinitionGroup>/g' > $i.bak
		cat $i.bak > $i
	else
		echo "$i has post build, ignore it."
	fi
done