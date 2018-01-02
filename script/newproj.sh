tiny_dir='c:/tiny'
shell_dir="$tiny_dir/shell"
tool_dir="$tiny_dir/tool"
script_dir="$tiny_dir/script"
project_dir="$tiny_dir/project"
PATH="$shell_dir;$tool_dir"
echo $PATH

name=$1
echo "project name is $1"
if [ -z $name ]; then
    echo 'project name is null'
    exit
fi

echo "project directory is $project_dir"

new_dir="$project_dir/$name"

if [ -d "$new_dir" ]; then
    echo 'project directory has been existed'
fi

if [ ! -d "$project_dir" ]; then
    echo mkdir "$project_dir"
    mkdir "$project_dir"
fi


if [ ! -d "$project_dir" ]; then
    echo mkdir "$new_dir"
    mkdir "$new_dir"
fi

guid=`uuidgen`

output="$new_dir/$name.vcxproj"

if [ -f "$output" ]; then
    echo mv "$output" "$output.bak"
    mv "$output" "$output.bak"
fi

echo ouput to mv "$output"
cat $script_dir/newproj.vcxproj | \
    sed "s/<ProjectGuid>__<\/ProjectGuid>/<ProjectGuid>\{$guid\}<\/ProjectGuid>/g" | \
    sed "s/<RootNamespace>__<\/RootNamespace>/<RootNamespace>$name<\/RootNamespace>/g" \
    > "$output"
	
	
	
	
	
	
	
	
	
	
