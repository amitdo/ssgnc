#include <ssgnc.h>

#include <iostream>
#include <string>

namespace {

bool readLine(std::istream *in, std::string *line)
{
	try
	{
		if (!std::getline(*in, *line))
			return false;
		return true;
	}
	catch (...)
	{
		in->setstate(std::ios::badbit);
		return false;
	}
}

bool searchNgrams(std::istream *in, const ssgnc::Database &database,
	ssgnc::Query *query)
{
	std::string line;
	while (readLine(in, &line))
	{
		ssgnc::String query_str(line.c_str(), line.length());
		if (!database.parseQuery(query_str, query))
		{
			SSGNC_ERROR << "ssgnc::Database::parseQuery() failed" << std::endl;
			return false;
		}

		ssgnc::Agent agent;
		if (!database.search(*query, &agent))
		{
			SSGNC_ERROR << "ssgnc::Database::search() failed" << std::endl;
			return false;
		}

		ssgnc::Int16 encoded_freq;
		std::vector<ssgnc::Int32> tokens;
		ssgnc::StringBuilder ngram_str;

		while (agent.read(&encoded_freq, &tokens))
		{
			if (!database.decode(encoded_freq, tokens, &ngram_str))
			{
				SSGNC_ERROR << "ssgnc::Database::decode() failed" << std::endl;
				return false;
			}

			std::cout << ngram_str << '\n';
			if (!std::cout)
			{
				SSGNC_ERROR << "std::ostream::operator<<() failed"
					<< std::endl;
				return false;
			}
		}

		if (agent.bad())
		{
			SSGNC_ERROR << "ssgnc::Agent::read() failed" << std::endl;
			return false;
		}

		std::cout << '\n';
		if (!std::cout)
		{
			SSGNC_ERROR << "std::ostream::operator<<() failed" << std::endl;
			return false;
		}
	}

	if (in->bad())
	{
		SSGNC_ERROR << "::readLine() failed" << std::endl;
		return false;
	}

	if (!std::cout.flush())
	{
		SSGNC_ERROR << "std::ostream::flush() failed" << std::endl;
		return false;
	}
	return true;
}

}  // namespace

int main(int argc, char *argv[])
{
	ssgnc::Query query;
	query.set_max_num_results(10);

	if (!query.parseOptions(&argc, argv))
		return 1;

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0]
			<< " [OPTION]... INDEX_DIR [FILE]...\n\n";
		ssgnc::Query::showOptions(&std::cerr);
		return 2;
	}

	ssgnc::Database database;
	if (!database.open(argv[1]))
		return 3;

	if (argc == 2)
	{
		if (!searchNgrams(&std::cin, database, &query))
			return 4;
	}

	for (int i = 2; i < argc; ++i)
	{
		std::cerr << "> " << argv[i] << std::endl;
		std::ifstream file(argv[i], std::ios::binary);
		if (!file)
		{
			SSGNC_ERROR << "ssgnc::ifstream::open() failed: "
				<< argv[i] << std::endl;
			continue;
		}

		if (!searchNgrams(&file, database, &query))
			return 4;
	}

	return 0;
}