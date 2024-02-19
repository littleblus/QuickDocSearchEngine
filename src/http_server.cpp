#include "cpp-httplib/httplib.h"
#include "index/search.hpp"

const std::string www_rootpath = "wwwroot";
const std::string raw_path = "raw_html/raw_html.txt";

int main() {
	auto searcher = search::Search::getInstance();
	searcher->init(raw_path);

	httplib::Server svr;
	svr.set_base_dir(www_rootpath.c_str());
	svr.Get("/s", [](const httplib::Request& req, httplib::Response& rsp) {
		if (!req.has_param("word")) {
			rsp.set_content("word is required", "text/plain; charset=utf-8");
			return;
		}
		std::string word = req.get_param_value("word");
#ifdef DEBUG
		std::cout << "search word: " << word << std::endl;
#endif
		auto json_result = search::Search::getInstance()->search(word);
		rsp.set_content(json_result, "application/json; charset=utf-8");
		});
	svr.listen("localhost", 1234);

	return 0;
}