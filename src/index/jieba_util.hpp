#pragma once
#include "../cppjieba/Jieba.hpp"
#include <vector>
#include <string>

namespace jieba {
	const char* const DICT_PATH = "cppjieba/dict/jieba.dict.utf8";
	const char* const HMM_PATH = "cppjieba/dict/hmm_model.utf8";
	const char* const USER_DICT_PATH = "cppjieba/dict/user.dict.utf8";
	const char* const IDF_PATH = "cppjieba/dict/idf.utf8";
	const char* const STOP_WORD_PATH = "cppjieba/dict/stop_words.utf8";
	static cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);

	// 输入一个字符串，输出切分后的词
	std::vector<std::string> cutString(const std::string& src) noexcept {
		std::vector<std::string> vs;
		jieba.CutForSearch(src, vs);
		return vs;
	}
}