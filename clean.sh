tbr="to_be_remove"
tbrsh=$tbr".sh"
find . -type d -name "*Debug*" > $tbr
find . -type d -name "*Release*"  >> $tbr
cat $tbr
read -p "remvoe all?(y/n)" reply
case $reply in
	y) 	cat $tbr | sed 's/\(.*\)/rm -f -r \1/g' > $tbrsh
		./$tbrsh
		rm -f $tbr
		rm -f $tbrsh
		;;
	*);;
esac

