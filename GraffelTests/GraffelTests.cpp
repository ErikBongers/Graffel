#include "pch.h"
#include "CppUnitTest.h"
#include "parser/Parser.h"
#include "parser/Generator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GraffelTests
{
	TEST_CLASS(GraffelTests)
	{
	public:
		
		TEST_METHOD(TestTimelines)
		{
			Parser parser("@12 #markerOne, @34 #markerTwo, 10 #markerThree, @50, 20 #lastOne");
			auto timeline = parser.parseTimeline();
			Generator g;
			auto str = g.generateTimeline(timeline);
		}
	};
}
