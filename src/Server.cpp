#include <iostream>
#include <string>
#include <algorithm>

bool positive_group(const std::string& input_line, const std::string& pattern){
    return input_line.find_first_of(pattern) != std::string::npos;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {

    if ( pattern.empty()) return true;
    if (input_line.empty()) return false;

    if (pattern.substr(0,2) == "\\d"){

        if(isdigit(input_line[0]))
            return match_pattern(input_line.substr(1),pattern.substr(2));
        return match_pattern(input_line.substr(1),pattern);
    }else if (pattern.substr(0,2) == "\\w"){

        if(isalnum(input_line[0]))
            return match_pattern(input_line.substr(1),pattern.substr(2));
        return match_pattern(input_line.substr(1),pattern);;

    }else if ( pattern[0] == '[') {

        auto bracket_pos = pattern.find(']');
        std::string pattern_string = pattern.substr(1,bracket_pos-1);

        if ( pattern_string[0] == '^'){
            if(!positive_group(input_line, pattern_string.substr(1)))
                return match_pattern(input_line.substr(1), pattern.substr(bracket_pos+1));
            return 0;
        }
        if (positive_group(input_line, pattern_string))
            return match_pattern(input_line.substr(1), pattern.substr(bracket_pos+1));
        else 
            return 0;
    
    } else if (pattern[0] == '^'){

        int n = pattern.length()-1;
        return (input_line.substr(0,n) == pattern.substr(1));
    
    }else if (pattern.back() == '$'){

        int m = input_line.length();
        int n = pattern.length()-1;
        return(input_line.substr(m-n) == pattern.substr(0,n));
    
    }else if (pattern.length() >1 && pattern[1] == '+'){

        if(!input_line.empty() && (pattern[0] == '.' || pattern[0] == input_line[0]))
            return match_pattern(input_line.substr(1),pattern.substr(2));
    }

    
    if (pattern[0] == input_line[0]){
        return match_pattern(input_line.substr(1),pattern.substr(1));
    }
     return match_pattern(input_line.substr(1),pattern);;
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
    std::string input_line;
    std::getline(std::cin, input_line);
    
    try {
        if (match_pattern(input_line, pattern)) 
                return 0;
        else
                 return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
