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

