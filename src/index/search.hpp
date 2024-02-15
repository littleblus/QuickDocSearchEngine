#pragma once
#include "index.hpp"
#include <mutex>
#include <algorithm>
#include <jsoncpp/json/json.h>

namespace search {
	class Search {
	public:
		Search() = default;
		Search(const Search&) = delete;
		Search& operator=(const Search&) = delete;

		static ns_index::Index* getInstance() {
			if (index == nullptr) {
				std::lock_guard<std::mutex> lock(mtx);
				if (index == nullptr) {
					index = new ns_index::Index();
				}
			}
			return index;
		}

		void init(const std::string& raw_path) {
			index = getInstance();
			index->build(raw_path);
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
				item["desc"] = doc_info.title; // TODO
				item["content"] = doc_info.content;
				root.append(item);
			}
			Json::FastWriter writer;
			return writer.write(root);
		}
	private:
		static ns_index::Index* index;
		static std::mutex mtx;
	};

	ns_index::Index* Search::index = nullptr;
	std::mutex Search::mtx;
}