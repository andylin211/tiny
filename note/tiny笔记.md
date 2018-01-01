# 做一件事，做好

## ctlcode.exe

用于将I/O控制码分解为各个分量；
将各个分量组成一个I/O控制码；
提供UI。

> ctlcode.exe -s -c 222000

ctlcode=222000
type=22
access=0
function=800
method=0

> ctlcode.exe -s -t 22 -a 0 -f 800 -m 0

ctlcode=222000
type=22
access=0
function=800
method=0

> ctlcode.exe -c 222000 # 弹窗

ctlcode=222000
type=22
access=0
function=800
method=0

> ctlcode.exe -t 22 -a 0 -f 800 -m 0 # 弹窗

ctlcode=222000
type=22
access=0
function=800
method=0


## ctlfuzz

> ctlfuzz.exe -d "device_name" -c 222000 -i 512 -o 512

## ctlscan 

扫描inlen和oulen

> ctlscan.exe -d "device_name" -c 222000 
> ctlscan.exe -d "device_name" -t 22 -a 3 -f 800 -m 3

ctlcode	 inlen   outlen
222000   256     256

> ctlscan.exe -d "device_name" -r 800-900

ctlcode  inlen   outlen
222000   256     32
222008   4       4
222008   4       4
222180   8       32


## ctlfuzz.exe 

> ctlfuzz.exe -d "device_name" -c 222000 -i 512 -o 4

类似wget一样，是在做事情，所以显示进度条吧？怎么显示呢？













