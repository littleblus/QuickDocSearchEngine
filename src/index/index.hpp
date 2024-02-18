#pragma once
#include "jieba_util.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

#define SEP '\3'
//#define DEBUG

namespace ns_index {
	struct doc_info {
		std::string url;
		std::string title;
		std::string content;
		uint64_t doc_id;
	};

	struct inverted_elem {
		uint64_t doc_id;
		std::string word;
		int weight;

		inverted_elem(uint64_t doc_id, const std::string& word, int weight)
			: doc_id(doc_id), word(word), weight(weight) {
		}
	};

	struct count {
		int32_t title_cnt = 0;
		int32_t content_cnt = 0;
	};


	// 根据cnt（标题cnt和内容cnt）计算权重
	inline int weight(const count& cnt) {
		return 10 * cnt.title_cnt + 1 * cnt.content_cnt;
	}

	class Index {
	public:
		// 构建索引
		void build(const std::string& raw_path) { // 传入的是文档的处理后内容
			std::fstream file(raw_path, std::ios::in | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("build file open failed");
			}
			std::string line;
			int errcnt = 0;
#ifdef DEBUG
			int cnt = 0;
#endif
			while (std::getline(file, line)) {
#ifdef DEBUG
				if (++cnt % 50 == 0) std::cerr << "构建索引:" << cnt << "行" << '\n';
#endif
				// 建立正排索引
				doc_info doc;
				doc.doc_id = forward_index.size();
				// url\3title\3content
				size_t url_end = line.find(SEP);
				if (url_end == std::string::npos) {
					std::cerr << "build file format error, count:" << errcnt << '\n';
					continue;
				}
				size_t title_end = line.find(SEP, url_end + 1);
				if (title_end == std::string::npos) {
					std::cerr << "build file format error, count:" << errcnt << '\n';
					continue;
				}
				doc.url = line.substr(0, url_end);
				doc.title = line.substr(url_end + 1, title_end - url_end - 1);
				doc.content = line.substr(title_end + 1);

				// 建立倒排索引
				auto wt = jieba::cutString(doc.title);
				auto wc = jieba::cutString(doc.content);
				// 进行词频统计，忽略大小写
				std::unordered_map<std::string, count> wordToCnt;
				for (auto& str : wt) { // 标题的词频统计
					std::string s = str;
					for (auto& ch : s) ch = tolower(ch);
					wordToCnt[s].title_cnt++;
				}
				for (auto& str : wc) { // 内容的词频统计
					std::string s = str;
					for (auto& ch : s) ch = tolower(ch);
					wordToCnt[s].content_cnt++;
				}
				for (auto& [str, cnt] : wordToCnt) {
					// 保证str全部是小写
					inverted_index[str].emplace_back(doc.doc_id, str, weight(cnt));
				}

				forward_index.push_back(std::move(doc));
			}
		}

		// 获取正排索引
		const doc_info& get_doc_info(uint64_t doc_id) const {
			if (doc_id >= forward_index.size() || doc_id < 0) {
				throw std::out_of_range("doc_id out of range");
			}
			return forward_index[doc_id];
		}
		doc_info& get_doc_info(uint64_t doc_id) {
			return const_cast<doc_info&>(static_cast<const Index*>(this)->get_doc_info(doc_id));
		}

		// 获取倒排索引
		const std::vector<inverted_elem>* get_inverted_index(const std::string& word) const noexcept {
			auto it = inverted_index.find(word);
			return it == inverted_index.end() ? nullptr : &it->second;
		}
		std::vector<inverted_elem>* get_inverted_index(const std::string& word) noexcept {
			return const_cast<std::vector<inverted_elem>*>(static_cast<const Index*>(this)->get_inverted_index(word));
		}
	private:
		std::vector<doc_info> forward_index; // 正排索引，文档id对应下标
		std::unordered_map<std::string, std::vector<inverted_elem>> inverted_index; // 倒排索引
	};
}