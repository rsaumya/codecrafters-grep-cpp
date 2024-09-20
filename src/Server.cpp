#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>


class patternMatch {

    private:
        bool positiveGroup (const std::string& inputLine, const std::string& pattern);
        std::string getGroup(int groupNumber);
        bool storeMatchedPattern(const std::string& inputLine,const std::string& pattern);
        std::pair<std::string,std::string> oneOrMoreHelper(const std::string& inputLine,const std::string& pattern);

        std::string Digit ="0123456789";
        std::string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"+Digit;
        std::vector<std::string> backreferenceGroup;
        size_t matchedStringLength =0;
        std::pair<std::string,std::string> addStorage;
        bool start = false;

    public:

        bool matchPattern (const std::string& inputLine, const std::string& pattern);    
};

bool patternMatch::positiveGroup(const std::string& inputLine, const std::string& pattern){
    size_t n = pattern.find_first_of('-');
    if(n != std::string::npos){
        size_t first = alphaNum.find_first_of(pattern[n-1]);
        size_t last = alphaNum.find_first_of(pattern[n+1]);
        std::string newPattern = alphaNum.substr(first,last-first);
        return newPattern.find_first_of(inputLine[0])!= std::string::npos;
    }
    return pattern.find_first_of(inputLine[0]) != std::string::npos;
}

std::string patternMatch::getGroup(int groupNumber){
    if ( groupNumber <= backreferenceGroup.size())
        return backreferenceGroup[groupNumber-1];
    return "";
}

bool patternMatch::storeMatchedPattern(const std::string& inputLine, const std::string& pattern){
    size_t closeParenPos = pattern.find(')');
    if (closeParenPos == std::string::npos) return false;

    std::string groupPattern = pattern.substr(1, closeParenPos - 1);
    std::string remainingPattern = pattern.substr(closeParenPos + 1);
    backreferenceGroup.push_back('('+groupPattern+')');

    if(pattern[closeParenPos+1] == '+'){
        addStorage = oneOrMoreHelper(inputLine.substr(matchedStringLength),pattern);
        return matchPattern(addStorage.first,addStorage.second);
    }

    // Match alternation (|) within parentheses, e.g. (abc|def)
    size_t alternationPos = groupPattern.find('|');
    if (alternationPos != std::string::npos) {
        std::string leftPattern = groupPattern.substr(0, alternationPos);
        std::string rightPattern = groupPattern.substr(alternationPos + 1);

        // Try matching the left side or the right side
        return matchPattern(inputLine, leftPattern + remainingPattern) || matchPattern(inputLine, rightPattern + remainingPattern);
    }
    // else if (matchPattern(inputLine,groupPattern+remainingPattern))
    return matchPattern(inputLine,groupPattern+remainingPattern);
}

//One or More Helper Function
std::pair<std::string,std::string> patternMatch::oneOrMoreHelper(const std::string& inputLine,const std::string& pattern){
    size_t i =0;
    size_t n = pattern.find_first_of('+');
    
    std::string inputLineModified = inputLine;
    std::string patternModified = pattern.substr(0,n);

    while(matchPattern(inputLineModified,patternModified)){
        inputLineModified = inputLineModified.substr(1);
        i++;
    }
    if( i==0) return std::make_pair(inputLineModified,patternModified);
        return std::make_pair(inputLineModified, pattern.substr(n+1));

}

// Main match pattern function
bool patternMatch::matchPattern(const std::string& inputLine, const std::string& pattern){

    //if pattern found then it is empty otherwise input line will be empty
    if(pattern.empty()) return true;
    if (pattern[0] == '$') {
        return inputLine.empty();
    }
    if(inputLine.empty()) return false;

    //check for digit "\d"
    if(pattern.substr(0,2) == "\\d"){
        start = true;
        if(pattern[2] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }
        size_t n = inputLine.find_first_of(Digit);
        if(n != std::string::npos){
            return matchPattern(inputLine.substr(n+1),pattern.substr(2));
        }
        return false;
    }

    //check for alpha numeric values
    if (pattern.substr(0, 2) == "\\w") {
        start = true;
        if(pattern[2] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }
        size_t n = inputLine.find_first_of(alphaNum);
        if (n != std::string::npos) {          
            return matchPattern(inputLine.substr(n+1), pattern.substr(2));
        }
        return false;
    }


    //check for [ ] and [^ ]
    if (pattern[0] == '[') {
        start = true;
        size_t bracketPosition = pattern.find(']');
        std::string patternString = pattern.substr(1, bracketPosition - 1);
        
        if(pattern[bracketPosition+1] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }

        if (patternString[0] == '^') {
            if (!positiveGroup(inputLine, patternString.substr(1))) {
                return matchPattern(inputLine.substr(1), pattern.substr(bracketPosition + 1));
            }
            return false;
        }
        if (positiveGroup(inputLine, patternString)) {
            return matchPattern(inputLine.substr(1), pattern.substr(bracketPosition + 1));
        }
        return false;
    }
    if (pattern[0] == '^') {
        start = true;
        size_t n = pattern.find_first_of(' ') ;
        if(n == std::string::npos){
            int n = pattern.length()-1;
            return (inputLine.substr(0,n) == pattern.substr(1));
        }
        if (matchPattern(inputLine.substr(0, n-1), pattern.substr(1,n-1)))
             return matchPattern(inputLine.substr(n-1),pattern.substr(n+1));
        return false;
    }

    //Handles '+' for one or more time when pattern is a single character
    if (pattern.length() > 1 && pattern[1] == '+'){
        start = true;
        size_t i = 0;
        while (i < inputLine.length() && (pattern[0] == '.' || pattern[0] == inputLine[i])) {
            i++;
        }
        if(i==0) return false;
        return matchPattern(inputLine.substr(i), pattern.substr(2));
    }


    if (pattern.length() > 1 && pattern[1] == '?') {
        start = true;
        if(pattern[2] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }
        if ( matchPattern(inputLine, pattern.substr(2))) return true;
        if (!inputLine.empty() && (pattern[0] == '.' || pattern[0] == inputLine[0])) {
            return matchPattern(inputLine.substr(1), pattern.substr(2));
        }
    }

    if (pattern[0] == '.') {
        start = true;
        if(pattern[1] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }
        return matchPattern(inputLine.substr(1), pattern.substr(1));
    }

    if (pattern[0] == '(') {
        start = true;
        return storeMatchedPattern(inputLine, pattern);
    }

    if (pattern[0] == '\\' && isdigit(pattern[1])) {
        if(pattern[2] == '+'){
            addStorage = oneOrMoreHelper(inputLine,pattern);
            return matchPattern(addStorage.first,addStorage.second);
        }
        int groupNumber = pattern[1] - '0';
        std::string capturedGroup = getGroup(groupNumber);
        return matchPattern(inputLine,capturedGroup+pattern.substr(2));
    }

    if (pattern[0] == inputLine[0]) {
        start = true;
        return matchPattern(inputLine.substr(1), pattern.substr(1));
    }
    if(!start) return matchPattern(inputLine.substr(1),pattern);
    return false;
    
}




int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    // Uncomment this block to pass the first stage
    //
    std::string inputLine;
    std::getline(std::cin, inputLine);
    patternMatch match;
    try {
        if (match.matchPattern(inputLine, pattern)){
            // std::cout<<"match found"<<std::endl;
            return 0;
        } 
        else{
            // std::cout<<"match not found!"<<std::endl;
            return 1;
        }
            
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
