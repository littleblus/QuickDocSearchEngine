#pragma once
#include <string>

namespace html_util {
	struct doc_info {
		std::string url;
		std::string title;
		std::string content;
	};

	// 提取文件中的url
	std::string extractUrl(const std::string& file_path, const std::string& src_path);

	// 提取string中的title
	std::string extractTitle(const std::string& file);

	// 提取string中的content(去标签)
	std::string extractContent(const std::string& file);
}