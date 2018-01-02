tiny_dir='c:/tiny'
shell_dir="$tiny_dir/shell"
script_dir="$tiny_dir/script"
PATH="$shell_dir"
echo "root: "$tiny_dir
echo "shell: "$shell_dir
echo "script: "$script_dir
echo "PATH: "$PATH

tmp="$script_dir/tmp"
tmp_sh="$tmp.sh"
echo $tmp
echo $tmp_sh


find . -type d -name "include"

find $tiny_dir -type d -name "Debug" -or \
	-type d -name "Release" -or \
	-type d -name "x64" -or \
    -type f -name "*.bak" \
    > $tmp
cat $tmp
cat $tmp | sed 's/\(.*\)/rm -f -r \1/g' > $tmp_sh
bash $tmp_sh
rm -f $tmp
rm -f $tmp_sh
