curr_dir='c:/tiny'
scri_file="$curr_dir/script_file.txt"
find 'c:/script' | grep '.sh$' | tee $scri_file
for i in `cat $scri_file`; do
	echo $i "---"
done
