# 在windows下使用类似linux下的shell

## 部署

首页
http://win-bash.sourceforge.net/

下载
https://sourceforge.net/projects/win-bash/?source=typ_redirect

shell.w32-ix86.zip直接解压即可，放到c盘根目录，设置环境变量PATH

测试
Win+R "cmd" enter调出cmd
bash
bash$ which bash.exe


## 命令行里最帅的部分，IO重定向！

from "the linux command line" ch6 Redirection

cat - concatenate file（合并文件）
sort - sort lines of text
uniq - report or omit repeated lines
grep - print lines matching a pattern
wc - print newline, word and byte counts for each file
head - output the first part of a file
tail - output the last part of a file
tee - read from standard input and write to standard output and file

"everything is a file".

famous: stdout, stdin, stderr

一般来说stdout和stderr是和屏幕绑定的，不是磁盘文件；
一般来说stdin是和键盘绑定的，不是磁盘文件，或者stdout；

IO 重定向用于改变stdout，stderr，stdin的默认绑定；

三部分：

1. 重定向stdout
2. 重定向stdin
3. 重定向stderr

使用知名的'>'符号；

`ls -l /usr/bin` 默认stdout绑定到屏幕，`ls -l /usr/bin > ls-output.txt` 通过'>'将stdout绑定到ls-output.txt磁盘文件

`ls -l /bin/usr > ls-output.txt` 输出错误信息，为什么错误信息在屏幕上，不在磁盘文件里？

因为ls没有把错误信息写到stdout里，而是写在stderr里；
stdout绑定在磁盘文件，stderr绑定仍然绑定在屏幕；

">>"可以追加；

如何重定向stderr："2>"、"2>>"

如何重定向stdout和stderr到同一个文件："&>"、"&>>"

重定向stdin；

cat可以接受磁盘文件作为输入，`cat ls-output.txt`（stdin绑定到磁盘文件）；
cat可以接受多个磁盘文件作为输入，用于合并文件，`cat movie.mpeg.0* > movie.mpeg`

为什么`cat movie.mpeg.0* > movie.mpeg`可以实现预期的功能？

通配符'*'展开，是按照顺序的，看到`cat movie.mpeg.0*`，应该看到的是`cat movie.mpeg.01 movie.mpeg.02 movie.mpeg.03 movie.mpeg.04 ..`（有序的）

`cat`，没有给磁盘文件（stdin不绑定到磁盘文件），stdin还是绑定在键盘（默认），所以在等键盘输入，并以Ctrl+D结束

`cat > cat-output.txt` 重定向stdout到磁盘文件；

`cat < cat-input.txt` 重定向stdin为磁盘文件（而不是键盘）

grep看看：

```
grep "tiny" "grep-input.txt"
grep "tiny" < "grep-input.txt"
grep "tiny"
grep "tiny" > "grep-output.txt"
```

grep也是一样，分别绑定stdin到磁盘文件，绑定stdin到键盘，绑定stdout到磁盘文件；

除了知名的"<", ">", ">>", "2>", "&>",另一个非常有名的符号"|"——Pipeline

`command1 | command2` 即将前一个命令的stdout绑定到后一个命令的stdin，前后相继；

`ls -l /usr/bin | grep "tiny" | cat`

grep之后的结果作为cat的输入，输入和参数是不同的，作为参数必须写在cat后面，这里grep得到的内容即作为输入的内容；

`echo "/usr/bin" | ls`这是错的，ls不接受输入（不管从磁盘文件还是键盘），`ls /usr/bin`这里的/usr/bin作为参数存在，而不是指定磁盘文件作为stdin；

uniq 命令经常和sort命令一起用；

`ls /bin /usr/bin | sort | uniq | wc -l`计算不重复的行数
`ls /bin /usr/bin | sort | uniq -d | wc`计算重复的行数

grep 非常好用！

-i 忽略大小写

-v 打印不匹配的

head 和 tail用于查看大文件的头或者尾

-n 指定行数

`tail -f /var/log/messages` 监控

还有一个不常用但不可少的命令：tee

从stdin读取内容，输出到文件（参数指定），并输出到stdout；

`ls /usr/bin | tee ls.txt | grep zip`

## 以shell的角度看shell

先讲到expansion，比如知名的符号"*"；

处理"*"等通配符的过程称为expansion；

通配就是代指一组对象，expansion就是把这一组对象枚举出来；

```
echo this is a test
echo *
```

上面第一行是echo最普通的用法，即原样输出；
第二行和第一行的区别在于，没有明确指明输入的内容，偷懒了；
expansion就是换这个偷懒的债的；
这里"*"被展开为当前目录下所有文件、目录（字母序）；

"*"表示通配文件名中的任意字符串，注意是文件名；

expansion有很多种；

pathname expansion：

```
echo D*
echo *s
echo [[:upper:]]*
echo /user/*/share
ls -A # almost all 展示隐藏文件
echo .[!.]* #这个差不多意思（除了多个...作为头的文件）
```

~ expansion (tilde expansion):

```
echo ~
echo ~foo
```

arithmetic expansion

```
echo $((2+2))
```

记住，expansion里用到的数学表达式用$((expression))表示；只能整数

支持`+ - * / % **`运算；

可以嵌套`echo $(($((5**2+1))*3))`;

brace expansion

```
echo {A,B,C}            # 枚举
echo {1..5}             # 范围
echo {001..015}         # 0前缀
echo {Z..A}             # 逆序范围
echo {A{1,2}, B{3,4}}b 	# 嵌套
```

parameter expansion

```
echo $USER
printenv | less
```

command substitution

把命令行的输出展开作为参数；

```
echo $(ls)
ls -l $(which cp)
file $(ls -d /usr/bin/* | grep zip)
```

老的语法是``

引号！！！！因为这么多expansion，引号显得格外重要

```
echo this is a            test # expansion后没有长空格
echo the total price is $100.0 # $1被展开为参数（变量？）
```

双引号的作用

所有特殊字符都失效了，不再特殊：$, \, `；
参数展开，算术展开，命令展开仍然有效；

`echo "$USER $((2+2)) $(cal)` 参数、算数、命令

```
echo $(cal)
echo "$(cal)"
```

注意这两个语句的区别！！
展开：cal命令的输出作为参数，类似于

```
echo this is a       test
echo "this is a        test"
```

区别在于，后者保留格式，包括空格，换行符等

单引号的作用---所有的所有都失效了！！！

转义符：

`echo "The balance for user $USER is: \$5.00"`

ctrl+r搜索历史命令

!! 重复上一个命令
!number 重复history里的命令

ping

netstat 

wget

ssh

## 搜索文件 find

```
locate bin/zip
find ~
find ~ -type d | wc -l
find ~ -type f | wc -l
find ~ -type f -name "*.JPG" -size +1M | wc -l
```

这里的*会被展开？不会，因为""全部失效，处理$相关的参数、算数、命令

find的开关实在太多了！！！

除了各种开关，还有逻辑组合！！

```
find ~ \( -type f -not -perm 0600 \) -or \( -type d -not -perm 0700 \)
```
为什么()要转义？？

还有预定义的action！！！

-delete
-ls 
-print 隐含的
-quit

`find ~ -type f -name '*.BAK' -delete`

还有用户定义的action！！！

-exec rm '{}' ';'

'{}'表示当前路径, ';'表示命令结束

`find ~ -type f -name 'foo*' -ok ls -l '{}' ';'`

-ok表示先确认

`find ~ -type f -name 'foo*' -ok ls -l '{}' +`

ls只执行一次，区别于上一条命令

xargs

可以把输出（写到stdout）的内容作为参数

`find ~ -type f -name 'foo*' | xargs ls -l `

完成上面一样的任务，更容易记忆

## 压缩解压缩

tar 

四种模式：压缩，解压，追加，list内容
c 
x
r
t

```
tar cf playground.tar playground
tar t playground.tar
tar tvf playground.tar
mkdir foo
cd foo
tar xvf ../playground.tar
```

## 正则表达式！！！！

哇哦

grep [option] regex [file ...]

可以接受多个文件
`ls | grep "txt" | xargs grep "hello world" `

```
grep bzip dirlist*.txt    # 多个文件
grep -l bzip dirlist*.txt # 打印文件名，而不是行
grep -L bzip dirlist*.txt # 打印文件名，而不是行（不匹配）
grep -h bzip dirlist*.txt # 多文件时，省略文件名
grep -n bzip dirlist*.txt # 前缀行号
```

元字符和文本

^ $ . [ ] { } - ? * + ( ) | \

匹配开始结束
匹配字符(组)
匹配数量
提取

一般够用就好了

## 文本处理

cat 合并
sort 排序
uniq 去重
cut 
paste
join
comm
diff
patch
tr
sed
aspell

cat还有很多开关用来更好地可视化文本

`cat -A foo.txt`
`cat -ns foo.txt` 显示行号，省略连续空行

`du -s /usr/share/* | sort -nr | head` 算术倒序
`ls -l /usr/bin | sort -nr -k 5 | head` 算数倒序，根据第5个key

`sort --key=1,1 --key=2n input.txt`根据多个key排序,n跟在2后面表示数

`sort -k 3.7nbr -k 3.1nbr -k 3.4nbr input.txt` 
3.7表示第三个key，第7个字符开始，nbr表示算术省略空字符倒序

`sort -t ':' -k 7 /etc/passwd | head`根据":"来分割多个域

`sort foo.txt | uniq -c`前缀一个出现次数
d 显示重复
-f n 跳过n个域
-s n 跳过n个字符
-i 忽略case


`cut -f 3 distros.txt` 剪切第三个域及之后的文本
`cut -c 7-10` 剪切第7到第10个字符
`cut -d ':' -f 1 /etc/passwd | head` -d分隔符

`paste 1.txt 2.txt`

join!!! 数据库？？

文本比较

`comm 1.txt 2.txt`

`diff 1.txt 2.txt`

1d0
< a
4a4
> e

看不懂？
r1ar2 
r1cr2
r1dr2

`diff -c 1.txt 2.txt`就更友好一些了
`diff -u 1.txt 2.txt`变更

```
diff -Naur old.txt new.txt > patch.txt
patch < patch.txt  # 自动找到patch对象！！
```


tr，字符转换！！

```
echo "lowercase" | tr a-z A-Z
echo "lowercase" | tr [:lower:] A-Z
tr -d '\r' <dos_file> unix_file
echo "aaaaabbbbc" | tr -s ab # 输出abc，squeeze
```

sed，stream editor！！

```
echo "front" | sed 's/front/back/'
sed '2,3s/front/back/'  1.txt # 指定行号
sed 's/\([0-9]\{2\}\)\/\([0-9\{2\}\)\/\([0-9]\{4\}\)$/\3-\1-\2/' 1.txt #提取
```

为什么这么多转义？
因为sed只支持基本正则表达式，仅. ^ $ [ ] *
其他的字符都认为时普通，若要表达正则，前缀\转义
如( ) { } + ? |这些

```
echo "aaabbccc" | sed 's/b/B/' #只替换一处匹配
echo "aaabbccc" | sed 's/b/B/g' #替换一行所有匹配
sed -f distros.sed distros.txt #
```

格式化输出

```
nl distros.txt # 行号
echo "the quick brown fox jumped over the lazy dog" | fold -w 12 -s
fmt 
printf
```

```
#!/bin/bash

# Program to output a system information page

TITLE="System Information Report For $HOSTNAME"
CURRENT_TIME=$(date +"%x %r %Z")
TIMESTAMP="Generated $CURRENT_TIME, by $USER"

cat << _EOF_ # cat的一个技巧,_EOF_作为结束符(<<)
<HTML>
	<HEAD>
		<TITLE>$TITLE</TITLE>
	</HEAD>
	<BODY>
		<H1>$TITLE</H1>
		<P>$TIMESTAMP</P>
	</BODY>
</HTML>
_EOF_
```

除了变量-还有函数

```
function func {
	local foo #局部变量，默认全局变量
	echo "step 2"
	return
}
```

流控制if

```
x=5
if [ $x -eq 5 ]; then 
	echo "x equals 5."
else
	echo "x does not equal 5."
fi
```

if commands; then 
	commands;
[elif commands; then
	commands...]
[else
	commands]
fi

通过commands的返回值来判断

```
ls -d /usr/bin
echo $? # 上一条命令的返回值
echo true # 0
echo false # 1
```

最常用的command是test，或者[ expression ]，注意前后空格！

文件表达式

```
if [ -e "$FILE" ]; then # 存在判定
	if [ -f "$FILE" ]; then # 类型判定
		echo ""
	fi
	if [ -d "$FILE" ]; then echo "" fi
	if [ -r "$FILE" ]; then # 属性判断 -w -x
		echo ""
	fi 
else
	exit 1 # 返回值
fi

exit #默认0
```

字符串表达式

```
if [ $str ]; then echo "not null"; fi
if [ -n $str ]; then echo "none zero length"; fi
if [ -z $str ]; then echo "zero length"; fi
if [ "yes" == "$str" ]; then echo ""; fi #要与“”连用
```

算术表达式

```
[ $INT -eq 0 ]
[ $INT -ne 0 ]
[ $INT -lt 0 ]
[ $INT -ge 0 ]
[ $((INT % 2)) -eq 0 ] # test表达式里的算术表达式$(())
```

更新的版本！！！把前面的忘记把 [[ expression ]]

增加一个很重要的字符串表达式：

string1 =~ *regex*

匹配正则，太帅了！！！！

```
if [[ "$INT" =~ ^-?[0-9]+$ ]]; then # 匹配十进制数字符串
	echo ...
else
	exit 1
fi
```

另一个特性！！！[[ $FILE == foo.* ]]
支持expansion！！

除了[[ ]]及其两个新特性，又给了(())，为整数设计的

即将(())内整数作为test的返回值

```
if ((1)); then echo ""; fi
if ((0)); then echo ""; fi
if ((INT == 0)); then # 对比 [ $INT -eq 0 ]，不需要$
if (( ((INT %2)) == 0 )) #对比 [ $((INT % 2)) -eq 0 ]
```

逻辑组合

在[[]]，和(())形式的test里，&& || !表示与或非

流控制while util

```
while [[ $count -le 5 ]]; do
	echo $count
	count=$((count + 1))
done
```

支持continue、break

流控制 case

case word in 
	[pattern..]) commands;; #注意)和;;符号
esac

```
case $REPLY in
	0)	echo ""
		exit
		;;
	1)	echo""
		;;
	*) echo ""
		;;
esac

pattern支持
a)                	字符"a"
[[:alpha:]])		一组字符
???)				指定数目字符
*.txt)				以.txt结尾
*)					全部

```
read -p "enter word > "

case $REPLY in
	[[:alpha:]]) echo "is a single alphabetic character." ;;
	[ABC][0-9]) echo "is A, B, or C followed by a digit." ;;
	???) echo "is three characters long." ;;
	*.txt) echo "is a word ending in '.txt'" ;;
	*) echo "is something else." ;;
esac
```

;;&支持不break


脚本的参数！！！！

```
cat << _EOF_
\$0 = $0
\$1 = $1
\$2 = $2
\$3 = $3
_EOF_
```
./test.sh a bc 
$* a bc
$@ "a" "bc"
就是有没有分别双引号区别

流控制 for

for v [in words]; do
	commands
done

```
for i in A B C D; do echo $i; done
for i in {A..D}; do echo $i; done
for i in foo*.txt; do echo $i; done
for i in $(strings foo.txt); do ..; done
```

C语言形式

for (( exp1; exp2; exp3 )); do 
	commands
done


```
for (( i=0; i<5; i=i+1 )); do
	echo $i
done
```


