#include "index.h"
#include <iostream>

#define SEP '\3'

namespace index {
	// struct doc_info {
	// 	std::string url;
	// 	std::string title;
	// 	std::string content;
	// 	uint64_t doc_id;
	// };

	// struct inverted_elem {
	// 	std::string word;
	// 	uint64_t doc_id;
	// 	int weight;
	// };

	void Index::build(std::string raw_path) {
		std::fstream file(raw_path, std::ios::in | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("build file open failed");
		}
		std::string line;
		int errcnt = 0;
		while (std::getline(file, line)) {
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

			forward_index.push_back(std::move(doc));
		}
	}

	const index::doc_info& Index::get_doc_info(uint64_t doc_id) const {
		if (doc_id >= forward_index.size() || doc_id < 0) {
			throw std::out_of_range("doc_id out of range");
		}
		return forward_index[doc_id];
	}

	index::doc_info& Index::get_doc_info(uint64_t doc_id) {
		return const_cast<index::doc_info&>(static_cast<const index::Index&>(*this).get_doc_info(doc_id));
	}

	const std::vector<inverted_elem>& Index::get_inverted_index(const std::string& word) const {
		auto it = inverted_index.find(word);
		return it == inverted_index.end() ? std::vector<inverted_elem>() : it->second;
	}

	std::vector<inverted_elem>& Index::get_inverted_index(const std::string& word) {
		return const_cast<std::vector<inverted_elem>&>(static_cast<const index::Index&>(*this).get_inverted_index(word));
	}
}