# 第一次汇编作业

- 在数据段中定义以下2个数组

  ```assembly
  s db 100 dup(0)
  t db 100 dup(0)
  ```

- 在代码段中实现以下功能

  - 输入一行字符(敲回车结束输入)并保存到数组`s`中
  - 其中回车转化成ASCII码`00h`保存到数组中
  - 再把`s`中的每个元素逐个取出判断
    - 若元素为小写字母则把它转化成大写并保存到数组`t`中
    - 若元素为空格则丢弃该元素,不要保存到`t`中
    - 若元素为其它字符则原样保存到`t`中
  - 最后输出`t`中的全部元素再输出回车换行

- 提示

  - 可以循环调用`int 21h`的`01h`功能实现一行字符的输入
  - 可以循环调用`int 21h`的`02h`功能实现一行字符的输出
  - 数组`s`和`t`的下标可以分别用寄存器`SI`、`DI`表示