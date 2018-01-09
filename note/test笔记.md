## vmware 虚拟机U盘启动

1. 添加硬盘并使用物理磁盘-选U盘（老毛桃启动U盘）
2. 电源进入BIOS，选择启动顺序

## 破坏分区引导记录，老毛桃恢复

1. winhex把分区C:第一扇区引导指令改掉（保留分区信息）
2. 确认无法引导
3. 老毛桃U盘启动
4. 引导恢复，选择对应分区，安装

## win7安装grub引导

grldr.mbr拷贝到c:\,
```
bcdedit /create /d "start grub" /application bootsector bcdedit /set {id} device boot bcdedit /set {id} path \grldr.mbr bcdedit /displayorder {id} /addlast
```

xp下载boot.ini后面加上
`c:\grldr.mbr="start grub"`

https://wiki.archlinux.org/index.php/GRUB_(%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87)

bios->mbr->dpt->pbr->bootmgr->bcd->winload.exe->加载内核->整个window

bcd相当于xp的boot.ini
bcd是全局的

windows都是通过bcd来引导吗？


