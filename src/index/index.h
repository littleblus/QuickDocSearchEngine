#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

namespace index {
	struct doc_info {
		std::string url;
		std::string title;
		std::string content;
		uint64_t doc_id;
	};

	struct inverted_elem {
		uint64_t doc_id;
		int weight;
	};

	class Index {
	public:
		// 构建索引
		void build(std::string raw); // 传入的是文档的处理后内容

		// 获取正排索引
		const doc_info& get_doc_info(uint64_t doc_id) const;
		doc_info& get_doc_info(uint64_t doc_id);

		// 获取倒排索引
		const std::vector<inverted_elem>& get_inverted_index(const std::string& word) const;
		std::vector<inverted_elem>& get_inverted_index(const std::string& word);
	private:
		std::vector<doc_info> forward_index; // 正排索引，文档id对应下标
		std::unordered_map<std::string, std::vector<inverted_elem>> inverted_index; // 倒排索引
	};
}