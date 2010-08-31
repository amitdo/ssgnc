#include <ssgnc.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#ifndef SSGNC_CGI_INDEX_DIR
#define SSGNC_CGI_INDEX_DIR "/usr/share/ssgnc"
#endif  // SSGNC_CGI_INDEX_DIR

namespace {

using namespace ssgnc;

typedef std::pair<String, String> KeyValuePair;
typedef std::map<String, String> KeyValueMap;

MemPool mem_pool;
KeyValueMap key_value_map;

Query query;
String query_str;

Database database;
Agent agent;

Int16 encoded_freq;
std::vector<Int32> tokens;

Int64 freq;
std::vector<String> token_strs;
StringBuilder ngram_buf;
StringBuilder token_buf;

void setupQuery()
{
	static const Int64 DEFAULT_MIN_FREQ = 10LL;
	static const Int64 DEFAULT_MAX_NUM_RESULTS = 100LL;
	static const Int64 DEFAULT_IO_LIMIT = 256LL << 10;

	query.set_min_freq(DEFAULT_MIN_FREQ);
	query.set_max_num_results(DEFAULT_MAX_NUM_RESULTS);
	query.set_io_limit(DEFAULT_IO_LIMIT);

	static const Int64 MAX_IO_LIMIT = 1LL << 20;

	std::vector<KeyValuePair> key_value_pairs;
	query.parseQueryString(std::getenv("QUERY_STRING"),
		&mem_pool, &key_value_pairs);
	key_value_map.insert(key_value_pairs.begin(), key_value_pairs.end());

	if (query.io_limit() == 0 ||
		query.io_limit() > static_cast<UInt64>(MAX_IO_LIMIT))
		query.set_io_limit(MAX_IO_LIMIT);
}

void printToken(const String &token)
{
	token_buf.clear();
	for (std::size_t i = 0; i < token.length(); ++i)
	{
		switch (token[i])
		{
		case '<':
			if (!token_buf.append("&lt;"))
				return;
			break;
		case '>':
			if (!token_buf.append("&gt;"))
				return;
			break;
		case '&':
			if (!token_buf.append("&amp;"))
				return;
			break;
		default:
			if (!token_buf.append(token[i]))
				return;
			break;
		}
	}
	std::cout << token_buf;
}

void printHtmlHeader()
{
	std::cout << "<head>\n"
		"<meta http-equiv=\"Content-Type\""
		" content=\"text/html; charset=utf-8\">\n"
		"<title>SSGNC</title>\n"
		"<style type=\"text/css\">\n"
		"<!--\n"
		"table { border: 2px solid; margin: 10px; padding: 5px; }\n"
		"table caption { caption-side: top; font-size: 125%;\n"
		" padding: 0px 0px 10px 0px; }\n"
		"table tr td { padding: 5px 10px; }\n"
		"table tr td.last { padding: 5px 10px 10px 10px; }\n"
		"table tr td.link { border-top: 1px solid; text-align: right; }\n"
		"a { color: inherit; text-decoration: none; }\n"
		"-->\n"
		"</style>\n"
		"</head>\n";
}

void printHtmlForm()
{
	std::cout << "<form method=\"get\" action=\"\">\n"
		"<table>\n"
		"<caption><a href=\"?\">SSGNC Request Form</a></caption>\n";

	std::cout << "<tr><td>Query</td>\n"
		"<td><input type=\"text\" name=\"q\" /></td>\n"
		"<td><input type=\"submit\" value=\"Submit\" /></td></tr>\n";

	std::cout << "<tr><td>Token Order</td><td colspan=\"2\">\n"
		"<input type=\"radio\" name=\"o\" value=\"unordered\" "
		<< ((query.order() == Query::UNORDERED) ? " checked=\"checked\"" : "")
		<< " /> Unordered\n"
		"<input type=\"radio\" name=\"o\" value=\"ordered\""
		<< ((query.order() == Query::ORDERED) ? " checked=\"checked\"" : "")
		<< " /> Ordered\n"
		"<input type=\"radio\" name=\"o\" value=\"phrase\""
		<< ((query.order() == Query::PHRASE) ? " checked=\"checked\"" : "")
		<< " /> Phrase\n"
		"<input type=\"radio\" name=\"o\" value=\"fixed\""
		<< ((query.order() == Query::FIXED) ? " checked=\"checked\"" : "")
		<< " /> Fixed</td></tr>\n";

	std::cout << "<tr><td>Min. Frequency</td><td>\n"
		"<input type=\"text\" name=\"f\" value=\""
		<< query.min_freq() << "\" /></td></tr>\n";

	std::cout << "<tr><td>No. Tokens</td>\n"
		"<td><input type=\"text\" name=\"t\" value=\""
		<< query.min_num_tokens() << '-'
		<< query.max_num_tokens() << "\" /></td></tr>\n";

	std::cout << "<tr><td>Max. Results</td>\n"
		"<td><input type=\"text\" name=\"r\" value=\""
		<< query.max_num_results() << "\" /></td></tr>\n";

	std::cout << "<tr><td>IO Limit</td>\n"
		"<td><input type=\"text\" name=\"i\" value=\""
		<< query.io_limit() << "\" /></td></tr>\n";

	std::cout << "<tr><td class=\"last\">Format</td>\n"
		"<td colspan=\"2\" class=\"last\">\n"
		"<input type=\"radio\" name=\"c\" value=\"html\""
		" checked=\"checked\" /> Html\n"
		"<input type=\"radio\" name=\"c\" value=\"text\" /> Text\n"
		"<input type=\"radio\" name=\"c\" value=\"xml\" /> Xml</td></tr>\n";

	std::cout << "<tr><td colspan=\"3\" class=\"link\">\n"
		"<a href=\"http://code.google.com/p/ssgnc/\">SSGNC Project Site\n"
		" - http://code.google.com/p/ssgnc/</a>\n"
		"</td></tr>\n";

	std::cout << "</table>\n"
		"</form>\n";
}

void printXmlQuery()
{
	std::cout << "<query>\n"
		"<str>" << query_str << "</str>\n"
		"<min_freq>" << query.min_freq() << "</min_freq>\n"
		"<min_num_tokens>" << query.min_num_tokens() << "</min_num_tokens>\n"
		"<max_num_tokens>" << query.max_num_tokens() << "</max_num_tokens>\n"
		"<max_num_results>" << query.max_num_results()
		<< "</max_num_results>\n"
		"<io_limit>" << query.io_limit() << "</io_limit>\n";

	std::cout << "<order>";
	switch (query.order())
	{
	case Query::UNORDERED:
		std::cout << "unordered";
		break;
	case Query::ORDERED:
		std::cout << "ordered";
		break;
	case Query::PHRASE:
		std::cout << "phrase";
		break;
	case Query::FIXED:
		std::cout << "fixed";
		break;
	}
	std::cout << "</order>\n"
		"</query>\n";
}

void handleHtmlRequest()
{
	std::cout << "Content-Type: text/html; charset=utf-8\n\n";

	std::cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"
		" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
		"<html>\n";
	printHtmlHeader();
	std::cout << "<body>\n";

	if (query_str.empty())
		printHtmlForm();

	while (agent.read(&encoded_freq, &tokens))
	{
		if (!database.decode(encoded_freq, tokens, &freq, &token_strs))
			continue;

		std::cout << "<div class=\"result\">";
		for (std::size_t i = 0; i < token_strs.size(); ++i)
		{
			std::cout << "<span class=\"token\">";
			printToken(token_strs[i]);
			std::cout << "</span> ";
		}
		std::cout << "<span class=\"freq\">" << freq << "</span>";
		std::cout << "</div>\n";
	}

	std::cout << "</body>\n"
		"</html>\n";
}

void handleTextRequest()
{
	std::cout << "Content-Type: text/plain; charset=utf-8\n\n";

	while (agent.read(&encoded_freq, &tokens))
	{
		if (database.decode(encoded_freq, tokens, &ngram_buf))
			std::cout << ngram_buf << '\n';
	}
}

void handleXmlRequest()
{
	std::cout << "Content-Type: application/xml\n\n";

	std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\""
		" standalone=\"yes\"?>\n"
		"<search>\n";
	printXmlQuery();

	std::cout << "<results>\n";
	while (agent.read(&encoded_freq, &tokens))
	{
		if (!database.decode(encoded_freq, tokens, &freq, &token_strs))
			continue;

		std::cout << "<result>";
		for (std::size_t i = 0; i < token_strs.size(); ++i)
		{
			std::cout << "<token>";
			printToken(token_strs[i]);
			std::cout << "</token>";
		}
		std::cout << "<freq>" << freq << "</freq>";
		std::cout << "</result>\n";
	}
	std::cout << "</results>\n"
		"</search>\n";
}

}  // namespace

int main()
{
	setupQuery();

	query_str = key_value_map["q"];
	if (!query_str.empty())
	{
		if (database.open(SSGNC_CGI_INDEX_DIR))
		{
			if (database.parseQuery(query_str, &query))
				database.search(query, &agent);
		}
	}

	String content_type = key_value_map["c"];
	if (!content_type.empty())
	{
		if (String("html").startsWith(content_type))
			handleHtmlRequest();
		else if (String("text").startsWith(content_type))
			handleTextRequest();
		else if (String("xml").startsWith(content_type))
			handleXmlRequest();
	}
	else
		handleHtmlRequest();

	return 0;
}