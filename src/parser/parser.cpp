// #define DEBUG
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem> // C++17, 需要链接库 -lstdc++fs
#include "html_util.h"

#define SEP '\3'

using html_util::doc_info;

const std::string src_path = "../../data/input";
const std::string raw_path = "../../raw_html/raw_html.txt";

void setFlieList(const std::string& path, std::vector<std::string>& file_list);
void parserHtml(const std::vector<std::string>& file_list, std::vector<doc_info>& doc_list);
void saveRawHtml(const std::vector<doc_info>& doc_list, const std::string& raw_path);

int main() {
	std::vector<std::string> file_list;
	// 将目录中的每一个文件的路径存入file_list中
	try {
		setFlieList(src_path, file_list);
	}
	catch (const std::exception& e) {
		std::cerr << "src_path error:" << src_path << e.what() << '\n';
		return 1;
	}
	// 对每一个文件进行解析
	std::vector<doc_info> doc_list;
	parserHtml(file_list, doc_list);
	// 将解析结果存入raw_html中
	try {
		saveRawHtml(doc_list, raw_path);
	}
	catch (const std::exception& e) {
		std::cerr << "raw_path error:" << raw_path << e.what() << '\n';
		return 1;
	}

	return 0;
}

void setFlieList(const std::string& path, std::vector<std::string>& file_list) {
	std::filesystem::path root_path(path);
	// 判断路径是否存在
	if (!std::filesystem::exists(root_path)) {
		throw std::runtime_error("root path not exist");
	}

	// 递归遍历目录（递归迭代器）
	for (const auto& file : std::filesystem::recursive_directory_iterator(root_path)) {
		// 只要常规文件中的.html文件
		if (file.is_regular_file() && file.path().extension() == ".html") {
#ifdef DEBUG
			std::cout << file.path() << '\n';
#endif
			file_list.push_back(file.path().string());
		}
	}
}

void parserHtml(const std::vector<std::string>& file_list, std::vector<doc_info>& doc_list) {
	for (const auto& file : file_list) {
		doc_info doc;
		// 解析文件
		std::fstream fs(file);
		if (!fs.is_open()) {
			std::cerr << "file in" << file << " open failed" << '\n';
			continue;
		}
		// 将文件内容读入到string中
		// std::istreambuf_iterator<char>(fs)是一个迭代器，指向文件流的起始位置
		// std::istreambuf_iterator<char>()是一个迭代器，指向文件流的结束位置
		// 多加一对括号是因为编译器可能会将括号内的内容解释为函数声明
		std::string raw((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
		try {
			doc.url = html_util::extractUrl(file, src_path);
			doc.title = html_util::extractTitle(raw);
			doc.content = html_util::extractContent(raw);
		}
		catch (const std::exception& e) {
			std::cerr << "file in" << file << " error:" << e.what() << '\n';
			continue;
		}
#ifdef DEBUG
		std::cout << "url: " << doc.url << '\n';
		std::cout << "title: " << doc.title << '\n';
		std::cout << "content: " << doc.content << '\n';
#endif

		// 将解析结果存入doc_list中，因为这个doc不在需要了，所以使用std::move
		doc_list.push_back(std::move(doc));
		fs.close();
	}
}

void saveRawHtml(const std::vector<doc_info>& doc_list, const std::string& raw_path) {
	// 每个文档之间使用'\n'分隔
	// 文档每个属性之间用'\3'分割
	// url\3title\3content\n
	std::fstream out(raw_path, std::ios::out | std::ios::binary | std::ios::trunc); // 二进制写入并清空文件
	if (!out.is_open()) {
		throw std::runtime_error("output file open failed");
	}
	for (auto& doc : doc_list) {
		out.write(doc.url.c_str(), doc.url.size());
		out.put(SEP);
		out.write(doc.title.c_str(), doc.title.size());
		out.put(SEP);
		out.write(doc.content.c_str(), doc.content.size());
		out.put('\n');
	}
	out.close();
}
