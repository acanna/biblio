#include "../src/DBLPManager.h"
#include "../src/ArticleInfo.h"
# include "gtest/gtest.h"

using namespace std;

DBLPManager dblp;

TEST (PaperPresent, Positive) {
	const string query = "Land.Cover.Classification.and.Forest.Change.Analysis";
	const string title = "Land Cover Classification and Forest Change Analysis, Using Satellite Imagery-A Case Study in Dehdez Area of Zagros Mountain in Iran.";
	const string venue = "J. Geographic Information System";
	const string year = "2011";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result[0].get_year(), year);
	EXPECT_EQ(result[0].get_venue(), venue);
	EXPECT_EQ(result[0].get_title(), title); 
}

TEST (PaperAbsent1, Negative) {
	const string query = "QQQ";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result.size(), 0);
}

TEST (PaperAbsent2, Negative) {
	const string query = "http://dblp.org/search/pbl/api?q";
	vector <ArticleInfo> result = dblp.publicationRequest(query);
	EXPECT_EQ(result.size(), 0);
}

/*
int main (int argc , char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
*/