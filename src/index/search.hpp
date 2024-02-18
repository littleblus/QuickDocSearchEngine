#pragma once
#include "index.hpp"
#include <mutex>
#include <algorithm>
#include <jsoncpp/json/json.h>

namespace search {
	class Search {
	private:
		Search() = default;
		Search(const Search&) = delete;
		Search& operator=(const Search&) = delete;
	public:
		static Search* getInstance() noexcept {
			if (index == nullptr) {
				std::lock_guard<std::mutex> lock(mtx);
				if (index == nullptr) {
					instance = new Search();
					index = new ns_index::Index();
				}
			}
#ifdef DEBUG
			std::cerr << "获取search单例" << '\n';
#endif
			return instance;
		}

		void init(const std::string& raw_path) {
			std::cerr << "search init start, please wait..." << '\n';
			try {
				index->build(raw_path);
			}
			catch (const std::runtime_error& e) {
				std::cerr << "search init failed: " << e.what() << '\n';
			}
			std::cerr << "search init success" << '\n';
		}

		/// @brief 搜索
		/// @param query 
		/// @return json格式的搜索结果
		std::string search(const std::string& query) {
			auto words = jieba::cutString(query);
			// 按照权重排序，降序
			std::vector<ns_index::inverted_elem> result;
			for (auto& word : words) {
				for (auto& c : word) c = std::tolower(c);
				auto ii = index->get_inverted_index(word);
				if (ii == nullptr) continue;
				result.insert(result.end(), ii->begin(), ii->end());
			}
			std::sort(result.begin(), result.end(), [](const ns_index::inverted_elem& a, const ns_index::inverted_elem& b) {
				return a.weight > b.weight;
				});
			// 返回json格式的搜索结果
			Json::Value root;
			int cnt = 0;
			for (auto& elem : result) {
				ns_index::doc_info doc_info;
				try {
					doc_info = index->get_doc_info(elem.doc_id);
				}
				catch (const std::out_of_range& e) {
					std::cerr << "search out of range, doc_id:" << elem.doc_id << '\n';
					continue;
				}
				Json::Value item;
				item["url"] = doc_info.url;
				item["title"] = doc_info.title;
				item["abst"] = getAbst(doc_info.content, elem.word);
				root.append(item);
			}
#ifdef DEBUG
			Json::StyledWriter writer;
#else
			Json::FastWriter writer;
#endif
			return writer.write(root);
		}

		/// @brief 按照关键字获取一个文档的摘要
		/// @param content 文档的内容
		/// @param word 检索的关键字
		/// @return 摘要，即content的子串
		std::string getAbst(const std::string& content, const std::string& word) noexcept {
			// 找到首次出现的位置，往前50个字符，往后100个字符
			// 忽略大小写，但是返回的摘要保留原大小写
			auto it = std::search(content.begin(), content.end(), word.begin(), word.end(), [](char a, char b) {
				return std::tolower(a) == std::tolower(b);
				});

			if (it == content.end()) return "null";
			size_t pos = std::distance(content.begin(), it);

			const int prev_len = 50, next_len = 100;
			size_t start = pos > prev_len ? pos - prev_len : 0;
			size_t end = pos + next_len < content.size() ? pos + next_len : content.size();
			if (start > end) return "null";
			return content.substr(start, end - start);
		}
	private:
		static Search* instance;
		static ns_index::Index* index;
		static std::mutex mtx;
	};

	ns_index::Index* Search::index = nullptr;
	std::mutex Search::mtx;
}