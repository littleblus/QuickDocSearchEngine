#include "html_util.h"
#include <stdexcept>

std::string html_util::extractUrl(const std::string& file_path, const std::string& src_path) {
	// url与文档路径是一一对应的
	// 从文件路径中构建url
	std::string head = "https://www.boost.org/doc/libs/1_84_0/doc/html/";
	auto begin = file_path.find("input");
	if (begin == std::string::npos) {
		throw std::runtime_error("file path format error");
	}
	return head + file_path.substr(src_path.size() + 1); // +1是为了去掉路径中的'/'
}

std::string html_util::extractTitle(const std::string& file) {
	auto begin = file.find("<title>");
	if (begin == std::string::npos) {
		throw std::runtime_error("title not found");
	}
	auto end = file.find("</title>", begin);
	if (end == std::string::npos) {
		throw std::runtime_error("title format error");
	}
	begin += 7; // "<title>"的长度
	if (begin > end) {
		throw std::runtime_error("title format error");
	}
	return file.substr(begin, end - begin);
}

std::string html_util::extractContent(const std::string& file) {
	// 去标签，基于状态机
	std::string content;
	enum {
		LABEL,
		CONTENT
	} state = LABEL;
	for (auto c : file) {
		switch (state) {
		case LABEL:
			if (c == '>') {
				state = CONTENT;
			}
			break;
		case CONTENT:
			if (c == '<') {
				state = LABEL;
			}
			else {
				if (c == '\n') {
					c = ' ';
				}
				content.push_back(c);
			}
			break;
		}
	}
	return content;
}