#include <iostream>
#include <vector>
#include <string>
#include <filesystem> // C++17, 需要链接库 -lstdc++fs

const std::string src_path = "/home/lwj/project/search_engine/data/input";
const std::string raw_path = "/home/lwj/project/search_engine/raw_html";

struct doc_info {
	std::string url;
	std::string title;
	std::string content;
};

void setFlieList(const std::string& path, std::vector<std::string>& file_list);
void parserHtml(const std::vector<std::string>& file_list, std::vector<doc_info>& doc_list);
void saveRawHtml(const std::vector<doc_info>& doc_list, const std::string& raw_path);

int main() {
	std::vector<std::string> file_list;
	// 将目录中的每一个文件的路径存入file_list中
	setFlieList(src_path, file_list);
	// 对每一个文件进行解析
	std::vector<doc_info> doc_list;
	parserHtml(file_list, doc_list);
	// 将解析结果存入raw_html中
	saveRawHtml(doc_list, raw_path);

	return 0;
}

void setFlieList(const std::string& path, std::vector<std::string>& file_list) {
	std::filesystem::path root_path(path);
	if (!std::filesystem::exists(root_path)) {
		throw std::runtime_error("path not exist");
	}

	for (const auto& file : std::filesystem::recursive_directory_iterator(root_path)) {
		// 只要常规文件中的.html文件
		if (file.is_regular_file() && file.path().extension() == ".html") {
			file_list.push_back(file.path().string());
		}
	}
}

void parserHtml(const std::vector<std::string>& file_list, std::vector<doc_info>& doc_list) {

}

void saveRawHtml(const std::vector<doc_info>& doc_list, const std::string& raw_path) {

}