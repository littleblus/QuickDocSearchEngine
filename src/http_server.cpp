#include "cpp-httplib/httplib.h"
#include "index/search.hpp"

const std::string www_rootpath = "wwwroot";
const std::string raw_path = "raw_html/raw_html.txt";

int main() {
	auto searcher = search::Search::getInstance();
	searcher->init(raw_path);

	httplib::Server svr;
	svr.Get("/", [](const httplib::Request& req, httplib::Response& rsp) {
		rsp.set_content("Hello World!", "text/plain");
		});
	svr.listen("localhost", 1234);

	return 0;
}