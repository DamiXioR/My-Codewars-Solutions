#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <string>

// Parse Seconds To Time Units
struct Parser {
    int requestedTimeUnit{0};
    int restTimeInSeconds{0};
};

struct ParsedTime {
    int years{0};
    int days{0};
    int hours{0};
    int minutes{0};
    int seconds{0};
};

struct TimeConstants {
    const static int yearInSeconds {31'536'000};
    const static int dayInSeconds {86'400};
    const static int hourInSeconds {3'600};
    const static int minuteInSeconds {60};
};

auto timeUnitParser = [](const int requestedTimeUnitInSeconds, int seconds){
    int numOfTimeUnits {0};
    if(seconds >= requestedTimeUnitInSeconds) {
        numOfTimeUnits = seconds / requestedTimeUnitInSeconds;
        seconds = seconds % requestedTimeUnitInSeconds;
    }
    return Parser{.requestedTimeUnit = numOfTimeUnits, .restTimeInSeconds = seconds};
};

// Convert Time Units To Words
void repleaceDelimitersWithSeparators(std::string& allUnitOfTimeInWords, int timeUnitsGtThanZero){
    std::string delimiter{"$"};
    while (timeUnitsGtThanZero > 2){
        int founded = allUnitOfTimeInWords.find(delimiter);
        if(founded != std::string::npos){
            allUnitOfTimeInWords.replace(founded, 1, ", ");
           --timeUnitsGtThanZero;
        }
    }

    if(timeUnitsGtThanZero == 2){
            int founded = allUnitOfTimeInWords.find(delimiter);
            if(founded != std::string::npos){
                allUnitOfTimeInWords.replace(founded, 1, " and ");
                --timeUnitsGtThanZero;
            }
    }

    if(timeUnitsGtThanZero == 1){
        allUnitOfTimeInWords.pop_back();
    }
}

void convertTimeUnitsToWords(std::string& allUnitOfTimeInWords, std::string unitOfTimeInWord, int timeUnit){
    std::string delimiter{"$"};
    if(timeUnit > 1){
        allUnitOfTimeInWords += std::to_string(timeUnit) + " " + unitOfTimeInWord + "s" + delimiter;
    } else {
        allUnitOfTimeInWords += std::to_string(timeUnit) + " " + unitOfTimeInWord + delimiter;
    }
}

std::string changeUnitsToHumanReadableFormat(ParsedTime& time){
    std::string result{""};
    int timeUnitsGtThanZero{0};
    if(time.years > 0){
        convertTimeUnitsToWords(result, std::string{"year"}, time.years);
        ++timeUnitsGtThanZero;
    }
    if(time.days > 0){
        convertTimeUnitsToWords(result, std::string{"day"}, time.days);
        ++timeUnitsGtThanZero;
    }
    if(time.hours > 0){
        convertTimeUnitsToWords(result, std::string{"hour"}, time.hours);
        ++timeUnitsGtThanZero;
    }
    if(time.minutes > 0){
        convertTimeUnitsToWords(result, std::string{"minute"}, time.minutes);
        ++timeUnitsGtThanZero;
    }
    if(time.seconds > 0){
        convertTimeUnitsToWords(result, std::string{"second"}, time.seconds);
        ++timeUnitsGtThanZero;
    }
    
    repleaceDelimitersWithSeparators(result, timeUnitsGtThanZero);

    return result;
}

// General Functions
std::string parseTime(int seconds){
    Parser years = timeUnitParser(TimeConstants::yearInSeconds, seconds);
    Parser days = timeUnitParser(TimeConstants::dayInSeconds, years.restTimeInSeconds);
    Parser hours = timeUnitParser(TimeConstants::hourInSeconds, days.restTimeInSeconds);
    Parser minutes = timeUnitParser(TimeConstants::minuteInSeconds, hours.restTimeInSeconds);

    ParsedTime parsedTime{
        .years = years.requestedTimeUnit, 
        .days = days.requestedTimeUnit, 
        .hours = hours.requestedTimeUnit, 
        .minutes = minutes.requestedTimeUnit, 
        .seconds = minutes.restTimeInSeconds};

    std::string result = changeUnitsToHumanReadableFormat(parsedTime);
    return result;
}

std::string format_duration(int seconds) {
    if(seconds == 0){
        return "now";
    }
    return parseTime(seconds);
}

// Test Cases
TEST_CASE("Format Duration - Main Function") {
    CHECK_EQ(format_duration(0), "now");
    CHECK_EQ(format_duration(1), "1 second");       
    CHECK_EQ(format_duration(62), "1 minute and 2 seconds");
    CHECK_EQ(format_duration(120), "2 minutes");
    CHECK_EQ(format_duration(3662), "1 hour, 1 minute and 2 seconds");
}

TEST_CASE("Repleace Delimiters With Separators") {
    std::string time1 = "10 years$2 days$55 hours$1 minute$33 seconds$";
    repleaceDelimitersWithSeparators(time1, 5);
    CHECK_EQ(time1, "10 years, 2 days, 55 hours, 1 minute and 33 seconds");

    std::string time2 = "1 year$2 days$3 minutes$4 seconds$";
    repleaceDelimitersWithSeparators(time2, 4);
    CHECK_EQ(time2, "1 year, 2 days, 3 minutes and 4 seconds");

    std::string time3 = "10 years$5 days$";
    repleaceDelimitersWithSeparators(time3, 2);
    CHECK_EQ(time3, "10 years and 5 days");

    std::string time4 = "5 days$";
    repleaceDelimitersWithSeparators(time4, 1);
    CHECK_EQ(time4, "5 days");

    std::string time5 = "";
    repleaceDelimitersWithSeparators(time5, 0);
    CHECK_EQ(time5, "");

}

TEST_CASE("Convert Time Units To Words") {
    std::string allTimeInText{""};
    std::string timeUnitInText{"year"};
    int numOfUnitsPlural{4};
    int numOfUnitsSingular{1};
    convertTimeUnitsToWords(allTimeInText, timeUnitInText, numOfUnitsPlural);
    CHECK_EQ(allTimeInText, "4 years$");
    allTimeInText.clear();
    convertTimeUnitsToWords(allTimeInText, timeUnitInText, numOfUnitsSingular);
    CHECK_EQ(allTimeInText, "1 year$");
}

// For Parse Seconds To Time Units
bool operator==(const Parser& first, const Parser& second){
    return first.requestedTimeUnit == second.requestedTimeUnit 
        && first.restTimeInSeconds == second.restTimeInSeconds;
}

TEST_CASE("Parse Seconds To Time Units") {
    //years
    CHECK_EQ(timeUnitParser(31'536'000, 31'535'999), Parser{0, 31'535'999});
    CHECK_EQ(timeUnitParser(31'536'000, 31'536'000), Parser{1, 0});
    CHECK_EQ(timeUnitParser(31'536'000, 31'536'001), Parser{1, 1});
    CHECK_EQ(timeUnitParser(31'536'000, 31'536'100), Parser{1, 100});

    //days
    CHECK_EQ(timeUnitParser(86'400, 86'399), Parser{0, 86'399});
    CHECK_EQ(timeUnitParser(86'400, 86'400), Parser{1, 0});
    CHECK_EQ(timeUnitParser(86'400, 86'401), Parser{1, 1});
    CHECK_EQ(timeUnitParser(86'400, 86'500), Parser{1, 100});

    //hours
    CHECK_EQ(timeUnitParser(3'600, 3'599), Parser{0, 3'599});
    CHECK_EQ(timeUnitParser(3'600, 3'600), Parser{1, 0});
    CHECK_EQ(timeUnitParser(3'600, 3'601), Parser{1, 1});
    CHECK_EQ(timeUnitParser(3'600, 3'700), Parser{1, 100});

    //minutes
    CHECK_EQ(timeUnitParser(60, 59), Parser{0, 59});
    CHECK_EQ(timeUnitParser(60, 60), Parser{1, 0});
    CHECK_EQ(timeUnitParser(60, 61), Parser{1, 1});
    CHECK_EQ(timeUnitParser(60, 70), Parser{1, 10});

    //seconds
    CHECK_EQ(timeUnitParser(60, 44), Parser{0, 44});
    CHECK_EQ(timeUnitParser(60, 17), Parser{0, 17});
    CHECK_EQ(timeUnitParser(60, 1), Parser{0, 1});
    CHECK_EQ(timeUnitParser(60, 0), Parser{0, 0});
}
