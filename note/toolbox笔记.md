# 个人工具箱

1. 一个类似管家工具箱的界面，toolbox.exe
2. win-bash工具套（见shell笔记），win-bash/
3. 每个工具都是一个shell脚本，script/
4. 每个工具可以有一个icon，icon/
5. 常用、频率等用户数据，config/

toolbox的逻辑：

1. 遍历script/目录下全部.sh文件，每个.sh文件认为是一个工具
2. .sh文件有一定的标识，用注释写在前几行 --特征注释
3. 特征注释里有name、icon、type等信息 --工具信息
4. 根据工具信息布置工具在工具箱中的位置
5. 点击工具即执行.sh脚本，执行过程除了输出全部日志，不会有任何交互
6. 可能有的工具直接拉起另外一个工具（如拉起guidgen.exe）

双击toolbox.exe
展示工具箱界面
点击工具图标
拉起工具执行
执行同时展示输出信息

方案一

+--------+------------------------------------+
| 最  近 | [  ] [  ] [  ] [  ] [  ] [  ] [  ] | 最近使用
| 常  用 | [  ] [  ] [  ] [  ] [  ] [  ] [  ] | 最常使用
| 测  试 | [  ] [  ] [  ] [  ] [  ] [  ] [  ] | 各分类
|        | [  ] [  ] [  ] [  ]                |
| 系  统 | [  ] [  ] [  ] [  ] [  ] [  ] [  ] |
|        | [  ] [  ] [  ] [  ] [  ] [  ] [  ] |
| 调  试 | [  ] [  ] [  ] [  ] [  ]           |
|                                             |
+---------------------------------------------+
|                                             |
|     日志窗口                                |
|                                             |
+---------------------------------------------+

方案二

device tree一样

方案三

everything
可以叫Everytool


 _________________________________________
|_______________________________________x_|
+-----------------------------------------+
|[_______________________________________]| 支持搜索（全字匹配）
+-----------------------------------------+
| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] | 字母拼音排序（不整理不分类）
| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] |
| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] |
| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] |
| [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] |
| [ ] [ ]                                 |
|_________________________________________|


文件名即工具名--
全部$tool：`ls $root/script/ | sed "s/.sh//g"`

搜索$input：`gfind $root/script/ -type f -name "*$input*"`

不需要图标，最简单：图标=首字+背景色

不需要输入参数：比如关闭自保护，打开蓝光测试日志，
如果需要输入参数的，都以小工具形式出现：比如


## version

读文件版本信息
写文件版本信息（考虑可能之前没有版本信息）

https://msdn.microsoft.com/en-us/library/windows/desktop/ms646981(v=vs.85).aspx

You can add version information to any files that can have Windows resources, such as DLLs, executable files, or .fon font files. To add the information, create a VERSIONINFO Resource and use the resource compiler to compile the resource.

GetFileVersionInfoSize
GetFileVersionInfo
VerQueryValue

语言无关信息（一个双字说明语言，一个fixedfileinfo）
语言相关信息（通过\stringfileinfo\%04x%04x\stringname来获取）

"\\VarFileInfo\\Translation" --> DWORD 低字lang，高字code page
"\\" --> VS_FIXEDFILEINFO

\StringFileInfo\lang-codepage\string-name

"\\StringFileInfo\\%04x%04x\\FileVersion",lang,codepage --> char*
"\\StringFileInfo\\%04x%04x\\ProductVersion",lang,codepage --> char*

仅包含文件版本信息的资源定义文件：
```
VS_VERSION_INFO VERSIONINFO
FILEVERSION 2018.1.7.10
BEGIN
  BLOCK "VarFileInfo"
  BEGIN
    VALUE "Translation", 0x409, 0x4E4
  END
  BLOCK "StringFileInfo"
  BEGIN
    BLOCK "040904E4"
    BEGIN
    VALUE "FileVersion", "2018.1.7.10"
    END
  END
END
```
编译命令：
rc.exe /l 0x0409 test.rc

```
typedef struct tagVS_FIXEDFILEINFO {
  DWORD dwSignature;
  DWORD dwStrucVersion;
  DWORD dwFileVersionMS;
  DWORD dwFileVersionLS;
  DWORD dwProductVersionMS;
  DWORD dwProductVersionLS;
  DWORD dwFileFlagsMask;
  DWORD dwFileFlags;
  DWORD dwFileOS;
  DWORD dwFileType;
  DWORD dwFileSubtype;
  DWORD dwFileDateMS;
  DWORD dwFileDateLS;
} VS_FIXEDFILEINFO;
```

singature 总是0xFEEF04BD，找到这个签名，读取资源块，update到模块里



## 长路径

CreateDirectory function
https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa363855(v=vs.85).aspx

BOOL WINAPI CreateDirectory(
  _In_     LPCTSTR               lpPathName,
  _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
);

lpPathName [in]
The path of the directory to be created.
For the ANSI version of this function, there is a default string size limit for paths of 248 characters (MAX_PATH - enough room for a 8.3 filename). To extend this limit to 32,767 wide characters, call the Unicode version of the function and prepend "\\?\" to the path. For more information, see Naming a File.

https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa365247(v=vs.85).aspx

很复杂！！= = 

只实现部分功能：

创建一个非常长的路径 -- 通过指定\\?\前缀 createdirectory
访问一个非常长的路径-文件操作，删除，运行等 -- 通过指定目录链接 mklink /j
删除一个非常长的目录 -- 通过目录链接删除子目录


## version 

EndUpdateResource last error 110