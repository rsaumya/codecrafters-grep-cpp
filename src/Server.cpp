#include<algorithm>
#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>

class patternMatch {
private:
	std::string Digit = "0123456789";
	std::string Alphanumeric = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
	std::vector<std::string> capturedGroups;
	bool start = false;
	/*size_t inputStart = 0;
	size_t patternStart = 0;*/


	bool digit(const std::string& inputLine,size_t& inputIndex);
	bool alphaNum(const std::string& inputLine, size_t& inputIndex);
	bool character(const std::string& inputLine, const std::string& pattern, size_t& inputIndex, size_t& patternIndex);
	bool positiveCharacterGroup(const char& inputLine, const std::string& pattern);
	bool match(const std::string& inputLine, const std::string& pattern, size_t& inputIndex, size_t patternIndex);
	bool containPipe(const std::string& pattern);
	bool oneOrMore(const std::string& inputLine, const std::string& pattern, size_t& start);
	bool alteration(const std::string& inputLine, const std::string& pattern, size_t& inputIndex);
	bool startOfLine(const std::string& inputLine, const std::string& pattern, size_t& inputIndex, size_t& patternIndex);
	bool matchBackrefernce(const std::string& inputLine, size_t groupNum, size_t& inputIndex);

public: 
	bool matchPattern(const std::string& inputLine, const std::string& pattern);
};

//check for the digit in the input line
bool patternMatch::digit(const std::string& inputLine, size_t& inputIndex) {
	if (start) {
		return Digit.find(inputLine[inputIndex])!=std::string::npos;
	}
	start = true;
	inputIndex = inputLine.find_first_of(Digit);
	return inputIndex != std::string::npos;
}

//check for the alpha numeric values in input line
bool patternMatch::alphaNum(const std::string& inputLine, size_t& inputIndex ) {
	if (start) {
		return Alphanumeric.find(inputLine[inputIndex])!= std::string::npos;
	}
	start = true;
	inputIndex = inputLine.find_first_of(Alphanumeric);
	return inputIndex != std::string::npos;
}

//check if a given pattern character exists anywhere in the input line
bool patternMatch::character(const std::string& inputLine, const std::string& pattern,size_t& inputIndex, size_t& patternIndex) {
	if (start) {
		return inputLine[inputIndex] == pattern[patternIndex];
	}
	start = true;
	inputIndex = inputLine.find(pattern[patternIndex]);
	return inputIndex != std::string::npos;
}

//check for positive character groups (e.g. [abc]) in the input line
bool patternMatch::positiveCharacterGroup(const char& inputLine, const std::string& pattern) {
	size_t n = pattern.find('-');
	if (n != std::string::npos) {
		char a = pattern[n - 1];
		char b = pattern[n + 1];
		//extract the characters between the range
		size_t start = Alphanumeric.find_first_of(a);
		size_t end = Alphanumeric.find_first_of(b);
		std::string patternRange = Alphanumeric.substr(start, end - start);

		return patternRange.find(inputLine) != std::string::npos;
	}
	return pattern.find(inputLine) != std::string::npos;
}

//bool patternMatch::containSquareBracket(const std::string& pattern) {
//	return pattern.find('[') != std::string::npos;
//}

//one or more function
bool patternMatch::oneOrMore(const std::string& inputLine, const std::string& pattern, size_t& start) {
	size_t count = 0;
	size_t temp = start;
	while (temp != inputLine.length() && match(inputLine, pattern,temp,0)) {
			count++;
		}
	if (count > 0) {
		start += count;
		return true;
	}
	else return false;
}

//check for '|'
bool patternMatch::containPipe(const std::string& pattern) {
	return pattern.find_first_of('|') != std::string::npos;
}

//Alteration
bool patternMatch::alteration(const std::string& inputLine, const std::string& pattern, size_t& inputIndex) {
	std::string word="";
	std::vector<std::string> alterWords;
	for (char c : pattern) {
		if (c == '|') {
			alterWords.push_back(word);
			word = "";
		}
		else word += c;
	}
    alterWords.push_back(word);
	for (std::string s : alterWords) {
		if (match(inputLine, s, inputIndex, 0)) {
			return true;
		}
	}
	return false;
}

//backreference
bool patternMatch::matchBackrefernce(const std::string& inputLine, size_t groupNum, size_t& inputIndex) {
	if (groupNum > capturedGroups.size()) return false;
	
	const std::string& captured = capturedGroups[groupNum-1];
	
	if (inputLine.substr(inputIndex, captured.length()) == captured) {
		inputIndex += captured.length();
		return true;
	}
	return false;
}


//match function
bool patternMatch::match(const std::string& inputLine, const std::string& pattern, size_t& inputIndex, size_t patternIndex) {
	while (inputIndex < inputLine.length() && patternIndex < pattern.length()) {
	
		if (pattern[patternIndex + 1] == '+') {
			start = true;
			if (oneOrMore(inputLine, pattern.substr(patternIndex, 1), inputIndex))
				patternIndex += 2;
			else return false;
		}
		else if (pattern[patternIndex + 2] == '+' && pattern[patternIndex] == '\\') {
			start = true;
			if (oneOrMore(inputLine, pattern.substr(patternIndex, 2), inputIndex))
				patternIndex += 3;
			else return false;
		}
		else if (pattern[patternIndex] == '\\' && isdigit(pattern[patternIndex + 1])) {
			int groupNum = pattern[patternIndex + 1] - '0';
			
			if (!matchBackrefernce(inputLine, groupNum, inputIndex)) return false;
			patternIndex += 2;
		}
		else if (pattern[patternIndex] == '\\') {
				if (pattern[patternIndex + 1] == 'd') {
					
					if (digit(inputLine,inputIndex)) {
						++inputIndex;
						patternIndex += 2;
					}
					else return false;
				}
				else if (pattern[patternIndex + 1] == 'w') {
						if (alphaNum(inputLine,inputIndex)) {
							++inputIndex;
							patternIndex += 2;
						
						}
						else return false;
				}
                else return false;
		}
		else if (pattern[patternIndex] == '^') {
			start = true;
			if (inputIndex != 0) return false; // input must start at the beginning
			patternIndex++;
		}
		else if (pattern[patternIndex] == '$') {
			if (inputIndex != inputLine.length()) return false;
			patternIndex++;
		}
		else if (pattern[patternIndex] == '.') {
			start = true;
			inputIndex++;
			patternIndex++;
		}

		else if (pattern[patternIndex + 1] == '?') {
			size_t temp = 0; //to store the index of pattern in case remaining pattern not match
			//check for the remaining pattern, if it matches then '?' matches the empty charater
			size_t n = inputLine.find(' ', inputIndex);
			std::string str = inputLine.substr(inputIndex, n - inputIndex);
			
			if (match(str, pattern, temp, patternIndex+2) || str =="") {
				patternIndex += 2;
			}	
			else if (inputLine[inputIndex] == pattern[patternIndex]) {
				patternIndex+=2;
				inputIndex++;
			}
		}
		else if (pattern[patternIndex] == '[') {
			start = true;
			size_t closeBracket = pattern.find(']', patternIndex);
			if (closeBracket == std::string::npos)
				throw std::runtime_error("Unhandled pattern " + pattern);

			std::string bracket = pattern.substr(patternIndex+1, closeBracket - patternIndex - 1);
			if (pattern[closeBracket + 1] == '+') {
				if (oneOrMore(inputLine, pattern.substr(patternIndex, closeBracket - patternIndex + 1), inputIndex))
					patternIndex = closeBracket + 2;
				else return false;
			}
			else if (bracket[0] == '^') {
					if (!positiveCharacterGroup(inputLine[inputIndex], bracket.substr(1))) {
						inputIndex++;
						patternIndex = closeBracket + 1;
					}
					else return false;
			}
			else if (positiveCharacterGroup(inputLine[inputIndex], bracket)) {
				inputIndex++;
				patternIndex = closeBracket + 1;
			}
			else return false;
		}
		else if (pattern[patternIndex] == '(') {
			start = true;
			size_t closeBracket = pattern.find(')', patternIndex);
			if (closeBracket == std::string::npos)
				throw std::runtime_error("Unhandled pattern " + pattern);

			std::string group = pattern.substr(patternIndex + 1, closeBracket - patternIndex - 1);
			
			size_t start = inputIndex; //to store starting index of the matched string of input in captured groups
			if (pattern[closeBracket + 1] == '+') {
				if (oneOrMore(inputLine, group, inputIndex)) {
					capturedGroups.push_back(inputLine.substr(start, inputIndex - start + 1));
					patternIndex = closeBracket + 2;
				}
				else return false;
				
			}
			else if (containPipe(group)) {
				if (alteration(inputLine, group, inputIndex)) {
                    capturedGroups.push_back(inputLine.substr(start, inputIndex - start));
					patternIndex = closeBracket + 1;
				}
				else return false;

			}
			else if (match(inputLine, group, inputIndex, 0)) {
				capturedGroups.push_back(inputLine.substr(start, inputIndex - start));
				patternIndex = closeBracket + 1;
			}
			else {
				
				return false;
			}
		}
		else {
            if (start) {
                if (character(inputLine, pattern, inputIndex, patternIndex)) {
                    ++inputIndex;
                    ++patternIndex;
                }
                else return false;
            }
			else {
				std::string subPattern = "";
				size_t i = patternIndex;
				//implement find substring
				while (Alphanumeric.find(pattern[i])!= std::string::npos && pattern[i] != ' ' 
					&& pattern[i+1]!='+' && pattern[i+1] != '?') {
					i++;
				}
				
				subPattern = pattern.substr(patternIndex, i - patternIndex);
				if (character(inputLine, subPattern, inputIndex, patternIndex)) {
					inputIndex += i;
					patternIndex += i;
				}
				else return false;
			}
			
			
		}
	}
	if (patternIndex >= pattern.length() || pattern[patternIndex] == '$') return true;
    else if(inputIndex > inputLine.length()) return false;
    return false;
}


bool patternMatch::matchPattern(const std::string& inputLine, const std::string& pattern) {
	size_t inputIndex = 0, patternIndex = 0;
	return match(inputLine,pattern,inputIndex,patternIndex);
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
		if (match.matchPattern(inputLine, pattern)) {
			return 0;
		}
		else {
			return 1;
		}

	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
