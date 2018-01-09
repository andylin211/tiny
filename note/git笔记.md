## 使用ssh登陆

参考：https://segmentfault.com/a/1190000002645623

保存了私钥公钥在微云上（cloud目录下）。

```
    $ git config --global user.name "andylin211"
    $ git config --global user.email "2273941148@qq.com"
    $ ssh-keygen -t rsa -C "2273941148@qq.com" //生成密钥对id_rsa和id_rsa.pub
    $ ssh-agent bash && ssh-add ~/.ssh/id_rsa //添加密钥到ssh-agent
    $ 将公钥添加到github
    $ ssh -T git@github.com //测试一下
    $ 修改下.git/config 文件中的url -> git@github.com:andylin211/tiny.git
```
    
## 办公机要使用代理才能访问github

安装proxifier来解决
https://www.proxifier.com/download.htm

## 常用命令

    $ git clone git@github:andylin211/tiny // 或者https://github.com/andylin211/tiny
    $ git add ... // -u 
    
## .gitignore文件

    # This file is used to ignore files which are generated
    # ----------------------------------------------------------------------------

    *~
    *.autosave
    *.a
    *.core
    *.moc
    *.o
    *.obj
    *.orig
    *.rej
    *.so
    *.so.*
    *_pch.h.cpp
    *_resource.rc
    *.qm
    .#*
    *.*#
    core
    !core/
    tags
    .DS_Store
    .directory
    *.debug
    Makefile*
    *.prl
    *.app
    moc_*.cpp
    ui_*.h
    qrc_*.cpp
    Thumbs.db
    *.res
    *.rc
    /.qmake.cache
    /.qmake.stash

    # qtcreator generated files
    *.pro.user*

    # xemacs temporary files
    *.flc

    # Vim temporary files
    .*.swp

    # Visual Studio generated files
    *.ib_pdb_index
    *.idb
    *.ilk
    *.pdb
    # *.sln
    *.suo
    # *.vcproj
    *vcproj.*.*.user
    *.ncb
    *.sdf
    *.opensdf
    #*.vcxproj
    *vcxproj.*

    # MinGW generated files
    *.Debug
    *.Release

    # Python byte code
    *.pyc

    # Binaries
    # --------
    *.dll
    *.exe

## U盘搭建仓库

1. gitforwindows.org 下载git
2. 
