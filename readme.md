# 快速站内文档搜索引擎
快速站内文档搜索引擎是一个高效的站内文档检索系统，使用先进的C++技术和多种开源库构建，旨在快速定位和检索HTML文档内的信息。通过文档的自动化解析、分词处理以及索引构建，结合一个轻量级服务器，本项目能够支持高并发的文档检索请求，实现信息的快速检索。

## 主要特点
- **文档解析模块**：解析HTML文档，提取文档标题、URI、摘要，并去除HTML标签。
- **分词及索引模块**：使用cppjieba库进行高效分词，移除停用词，通过哈希表构建正排和倒排索引，并对索引进行权重排序。
- **服务器监听模块**：利用cpp-httplib库监听指定端口，处理来自客户端的搜索请求。
- **结果呈现模块**：通过json格式返回搜索结果，然后使用JavaScript动态构建用户界面，以友好的方式展现结果。

## 环境需求
- Linux操作系统
- CMake
- C++编译环境（支持C++17及以上版本）

## 依赖安装
本项目依赖以下库：
- cppjieba
- jsoncpp
- cpp-httplib

安装指南（以Ubuntu为例）：

```bash
sudo apt-get update
sudo apt-get install -y libcppjieba-dev jsoncpp-dev
# 对于cpp-httplib，通常是作为项目的一部分直接包含，无需预先安装。
```
## 构建项目
以下步骤将指导您如何构建项目：
```bash
git clone ...
mkdir build
cd build
cmake ..
make
这将编译出可执行文件，位于build目录下。
```
## 运行项目
启动服务器：
```bash
./parser
./http_server
```
然后就可以通过对应的端口访问主机进行站内文档搜索的测试了
## 贡献
欢迎任何形式的贡献。请Fork本仓库并提交Pull Request，或者直接提交Issue反馈问题及建议。

## 授权协议
该项目采用Apache-2.0授权协议。详情请参阅LICENSE。