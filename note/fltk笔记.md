## 下载最新版本fltk并编译lib

http://www.fltk.org/index.php

## editor着色学习

Fl_Text_Buffer *stylebuf = 0;
Fl_Text_Buffer *textbuf = 0;
Fl_Text_Display::Style_Table_Entry styletable[] = {	// Style table
    { FL_BLACK,      FL_COURIER,           TS }, // A - Plain
    { FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // B - Line comments
    { FL_DARK_GREEN, FL_HELVETICA_ITALIC,  TS }, // C - Block comments
    { FL_BLUE,       FL_COURIER,           TS }, // D - Strings
    { FL_DARK_RED,   FL_COURIER,           TS }, // E - Directives
    { FL_DARK_RED,   FL_COURIER_BOLD,      TS }, // F - Types
    { FL_BLUE,       FL_COURIER_BOLD,      TS }, // G - Keywords
};
void style_unfinished_cb(int, void*) {}
编辑器是Fl_Text_Editor类型，
editor = new Fl_Text_Editor(0, 30, 660, 370);
设置默认字体，字号
特别的：buffer设置为
editor->buffer(textbuf);
editor->highlight_data(stylebuf, styletable, sizeof(styletable) / sizeof(styletable[0]),
                          'A', style_unfinished_cb, 0);
                          
目前为止需要有：
1. 两个Fl_Text_Buffer，一个用于文本，一个用于着色
2. 一个Fl_Text_Display::Style_Table_Entry，用于着色支持
3. 一个空的style_unfinished_cb

关于textbuf的一些用法：

```
new Fl_Text_Buffer;
length获取长度（字节数）
text即文本
text_range(start,end) 获取一个字符串拷贝，需要free
char_at(pos)获取字符
byte_at(pos)获取raw byte
address(pos)就是获取文本字符串中某位置的内存地址
insert(pos, text)插入
append(t)追加
remove(start,end)删除
replace(start,end,text)替换
copy(fromBuf, fromStart, fromEnd, toPos)复制粘贴
undo(cp) 
canUndo()
insertfile(file,pos,buflen)插入文件
loadfile(file,buflen)加载文件
outputfile(file,start,end,buflen)输出到文件
tab_distance()获取/设置tab的宽度
select(start,end)选中
selected()是否有选中
unselect()取消选中
selection_position(start,end)获取选中区间
selection_text()获取选中的文本，是拷贝，需要free()
remove_selection()删除选中文本
replace_selection()替换文本
secondary 的select，干嘛用的！！
highlight(start,end)高亮这个区间的文本
highlight()返回高亮的文本，是拷贝，需要free()
unhignlight()
highlight_position(start,end)获取高亮的区间
highlight_text()获取高亮的文本，是拷贝，需要free()
add_modify_callback(bufModifiedCB, cbArg)
remove_modify_callback(bufModifiedCB,cbArg)
call_modify_callback()
add_predelete_callback()删除前调用
remove_xx
call_xx
line_text(pos)获取整行
line_start(pos)获取行开始pos
line_end(pos)获取行结束pos
word_start(pos)
word_end(pos)
count_displayed_characters()显示字符数目?
skip_
count_lines(start,end)计算行数
skip_lines(start,nline)向下
rewind_lines(start,nline)向上

```

几个逻辑：

初始化时——根据文本生成样式文本，并设置stylebuf

```
void style_init()
{
    /* 文本 */
    char* text = textbuf->text(); 
    
    /* 对应的样式文本 */
    char* style = new char[textbuf->length() + 1];
    memset(style, 'A', textbuf->length());
    style[textbuf->length()] = 0;
    if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());
    style_parse(text, style, textbuf->length());
    
    /* stylebuf设置样式文本 */
    stylebuf->text(style);
    
    /* free */
    delete[] style;
    free(text);
}
```

解析文本style_parse：

```
void
style_parse(const char *text, char *style, int length) 
{
  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Block comments
  // D - Strings
  // E - Directives
  // F - Types
  // G - Keywords
    最后结果是style字符串变成这些字符的组合
}
```


加载文件，后者插入文件的操作
1. loadfile()/insertfile()
2. call_modify_callback()





