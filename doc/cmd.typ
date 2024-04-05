#set text(font: ("Times New Roman", "NSimSun"))
#show strong: set text(font: ("Times New Roman", "SimHei"))
#show emph: set text(red, style: "italic")
#set text(size: 11pt)

= 一些可能用到的命令

+ 生成新的`CMakelists.txt`文件，并将整个`simulator`文件夹下的文件全都（从`GB2312`或者`UTF-8 with BOM`）转换为`UTF-8`编码。
  ```shell
  build simulator -p simulator
  ```

+ 将`yaml`文件转换为对应的`Config.hpp`。该文件是整个可编程交换机的配置文件，包括了所有的配置信息。`yaml`文件为通用格式，硬件上可以被转换为`scala`文件。_请不要自己修改`Config.hpp`，应当总是使用`yaml`生成。如果没有硬件设计权限，也不要修改`yaml`，应该向硬件设计者提出改进建议。_
  ```shell
  config cfg/reference.yaml -p simulator -m pipeline
  ```