#pragma once
#include "../cppjieba/include/cppjieba/Jieba.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

namespace jieba {
	const char* const DICT_PATH = "src/cppjieba/dict/jieba.dict.utf8";
	const char* const HMM_PATH = "src/cppjieba/dict/hmm_model.utf8";
	const char* const USER_DICT_PATH = "src/cppjieba/dict/user.dict.utf8";
	const char* const IDF_PATH = "src/cppjieba/dict/idf.utf8";
	const char* const STOP_WORD_PATH = "src/cppjieba/dict/stop_words.utf8";
	static cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);

	// 输入一个字符串，输出切分后的词
	std::vector<std::string> cutString(const std::string& src) noexcept {
		std::vector<std::string> vs;
		jieba.CutForSearch(src, vs);

		// 去掉暂停词
		std::ifstream ifs(STOP_WORD_PATH);
		if (!ifs.is_open()) {
			throw std::runtime_error("stop word file open failed");
		}
		std::unordered_set<std::string> stop_word;
		std::string word;
		while (std::getline(ifs, word)) {
			stop_word.insert(word);
		}
		ifs.close();

		for (auto it = vs.begin(); it != vs.end(); ) {
			if (stop_word.find(*it) != stop_word.end()) {
				it = vs.erase(it);
			}
			else {
				++it;
			}
		}

		return vs;
	}
}