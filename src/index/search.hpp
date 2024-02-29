#pragma once
#include "index.hpp"
#include <mutex>
#include <algorithm>
#include <chrono>
#include <jsoncpp/json/json.h>
#include <unordered_map>

namespace search {
	class Search {
	private:
		Search() = default;
		Search(const Search&) = delete;
		Search& operator=(const Search&) = delete;
	public:
		static Search* getInstance() noexcept {
			if (instance == nullptr) {
				std::lock_guard<std::mutex> lock(mtx);
				if (instance == nullptr) {
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

		struct inverted_elem_unique {
			uint64_t doc_id;
			std::string longest_word;
			int weight;

			inverted_elem_unique() = default;
		};

		/// @brief 搜索
		/// @param query 
		/// @return json格式的搜索结果，包括搜索结果，搜索时间，搜索结果数量
		std::string search(const std::string& query) {
			auto time_start = std::chrono::high_resolution_clock::now();
			auto words = jieba::cutString(query);
			// 去重
			std::unordered_map<uint64_t, inverted_elem_unique> token_map;
			for (auto& word : words) {
				for (auto& c : word) c = std::tolower(c);
				auto ii = index->get_inverted_index(word);
				if (ii == nullptr) continue;
				// result.insert(result.end(), ii->begin(), ii->end());
				for (const auto& elem : *ii) {
					auto& it = token_map[elem.doc_id];
					it.doc_id = elem.doc_id;
					it.weight += elem.weight;
					if (it.longest_word.size() < word.size()) {
						it.longest_word = word;
					}
				}
			}

			std::vector<inverted_elem_unique> result;
			for (const auto& elem : token_map) {
				result.push_back(std::move(elem.second));
			}
			// 按照权重排序，降序
			std::sort(result.begin(), result.end(), [](const inverted_elem_unique& a, const inverted_elem_unique& b) {
				return a.weight > b.weight;
				});
			// 返回json格式的搜索结果
			Json::Value root, results;
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
				item["abst"] = getAbst(doc_info.content, elem.longest_word);
				results.append(item);
				cnt++;
			}
			auto time_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> time_s = time_end - time_start;
			root["results"] = results;
			root["time"] = time_s.count();
			root["cnt"] = cnt;
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
			bool is_start = pos <= prev_len;
			size_t start = is_start ? 0 : pos - prev_len;

			bool is_end = pos + next_len >= content.size();
			size_t end = is_end ? content.size() : pos + next_len;
			if (start > end) return "null";
			return (is_start ? "" : "...") + content.substr(start, end - start) + (is_end ? "" : "...");
		}
	private:
		static Search* instance;
		static ns_index::Index* index;
		static std::mutex mtx;
	};

	Search* Search::instance = nullptr;
	ns_index::Index* Search::index = nullptr;
	std::mutex Search::mtx;
}