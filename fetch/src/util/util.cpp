//
//  util.cpp
//  fetch
//
//  Created by Corey Ferguson on 9/1/25.
//

#include "util.h"

int decimal(const std::string hex) {
    int result = 0;

    for (size_t i = 0; i < hex.length(); i++) {
        int start = hex[i] >= '0' && hex[i] <= '9' ? 
                    48 : 
                    hex[i] >= 'A' && hex[i] <= 'Z' ? 
                    55 : hex[i] >= 'a' && hex[i] <= 'z' ? 
                    87 : 
                    INT_MIN;

        if (start == INT_MIN)
            return INT_MIN;

        result += (hex[i] - start) * pow(16, hex.length() - i - 1);
    }

    return result;
}

std::string decode(const std::string string) {
    if (string.empty())
        return string;

    // Find opening double quotations
    size_t l = 0;
    
    while (l < string.length() - 1 && (string[l] == '\\' || string[l] != '\"'))
        l++;
    
    // None found; return string
    if (l == string.length() - 1)
        return string;

    // Copy string
    size_t len = string.length() + 1;
    char*  str = new char[len];
    
    strcpy(str, string.c_str());
    
    // Erase opening double quotations
    for (size_t i = l; i < len - 1; i++)
        std::swap(str[i], str[i + 1]);
    
    len--;
    
    // Find closing double quotations
    size_t r = l;

    while (r < len - 2 && (str[r] == '\\' || str[r + 1] != '\"'))
        r++;

    // None found
    if (r == len - 2) {
// Info
#if LOGGING
        std::cout << "Missing terminating '\"' character: (" + string + ")\n";
#endif

        for (size_t i = l; i < len - 2; i++) {
            if (str[i] == '\\' && str[i + 1] == '\"') {
                for (size_t j = i; j < len - 1; j++)
                    std::swap(str[j], str[j + 1]);
                
                len--;
                i++;
            }
        }
    } else {
        // Erase closing double quotations
        for (size_t i = ++r; i < len - 1; i++)
            std::swap(str[i], str[i + 1]);

        len--;

        for (size_t i = l; i < r - 1; i++) {
            if (str[i] == '\\' && str[i + 1] == '\"') {
                for (size_t j = i; j < len - 1; j++)
                    std::swap(str[j], str[j + 1]);
                    
                len--;
                r--;
                i++;
            }
        }

        while (r < len - 2) {
            if (str[r] == '\\' && str[r + 1] == '\"') {
                for (size_t j = 0; j < 2; j++) {
                    for (size_t k = r; k < len - 1; k++)
                        std::swap(str[k], str[k + 1]);
                    
                    len--;
                }
            } else
                r++;
        }
    }

    std::string result = std::string(str);
    
    delete[] str;
    
    return result;
}

std::string encode(const std::string string) {
    size_t len = string.length() + 1;
    char*  str = new char[pow2((int)len + 2)];
    
    strcpy(str, string.c_str());
    
    // Insert leading double quotations
    str[len] = '\"';
    
    for (size_t i = len; i > 0; i--)
        std::swap(str[i], str[i - 1]);
    
    len++;
    
    // Escape double quotations
    for (size_t i = 1; i < len - 1; i++) {
        if (str[i] == '\"') {
            // Resize, if required
            if (is_pow(len + 2, 2)) {
                char* tmp = new char[pow2((int)(len + 2) * 2)];

                for (size_t j = 0; j < len; j++)
                    tmp[j] = str[j];

                delete[] str;

                str = tmp;
            }

            // Insert escape character
            str[len] = '\\';
            
            for (size_t j = len; j > i; j--)
                std::swap(str[j], str[j - 1]);
            
            len++;
            i++;
        }
    }
    
    // Insert trailing double quotoations
    str[len] = '\"';
    
    std::swap(str[len], str[len - 1]);
    
    len++;

    std::string result = std::string(str);

    delete[] str;
    
    return result;
}

// 1. (\+|-)?
// 2. (\+|-)?[0-9]+
bool is_int(const std::string value) {
    int i = 0;
    
    // leading positive (+) or negative (-) sign
    if (i != value.length() && (value[i] == '+' || value[i] == '-'))
        i++;
    
    if (i == value.length())
        return false;
    
    for (; i < value.length(); i++)
        if (!isdigit(value[i]))
            return false;
    
    return true;
}

// 1. (\+|-)?
// 2. (\+|-)?[0-9]+(\.[0-9]+)?
// 3. (\+|-)?([0-9]+(\.[0-9]+)?|[0-9]*\.[0-9]+)((E|e)(\+|-)?[0-9]+)?
bool is_number(const std::string value) {
    if (value.empty())
        return false;
    
    int i = 0;
    
    // leading positive (+) or negative (-) sign
    if (value[i] == '+' || value[i] == '-')
        i++;
    
    // find decimal point
    int j = i;
    
    while (j < value.length() && value[j] != '.')
        j++;
    
    // if no decimal point is found, start at the beginning (after the sign, if applicable)
    // find exponent
    int k = j == value.length() ? i : j;
    
    while (k < value.length() && !(value[k] == 'E' || value[k] == 'e'))
        k++;
    
    // stop at the decimal point, if applicable; otherwise stop at the exponent, if applicable
    int l = j < k ? j : k,
        m = i;
    
    for (; m < l; m++)
        if (!isdigit(value[m]))
            return false;
    
    // count the number of digits between the beginning (after sign, if applicable) and the decimal point (if applicable)
    // and the decimal point (if applicable) and the exponent (if applicable)
    size_t n = l - i;
    
    //  after decimal (if applicable) and before exponent (if applicable)
    if (j != value.length()) {
        for (m = j + 1; m < k; m++)
            if (!isdigit(value[m]))
                return false;
        
        n += k - j - 1;
    }
    
    // there are no digits between sign (if applicable) and decimal point (if applicable)
    // and/or decimal point (if applicable) and exponent (if applicable)
    if (n == 0)
        return false;
    
    // after exponent (if applicable)
    if (k != value.length()) {
        size_t l = k + 1;
        
        if (l == value.length())
            return false;
        
        // leading positive (+) or negative (-) sign
        if (value[l] == '+' || value[l] == '-')
            l++;
        
        if (l == value.length())
            return false;
        
        for (; l < value.length(); l++)
            if (!isdigit(value[l]))
                return false;
        // single digit
    }
    
    return true;
}

bool is_pow(const size_t b, const size_t n) {
    if (b == 0)
        return false;
    
    if (n == 0)
        return b < 2;
    
    int result = log(b) / log(n);
    
    return (int)result - result == 0;
}

std::string join(std::vector<std::string> values, std::string delimeter) {
    std::ostringstream oss;

    if (values.size()) {
        for (size_t i = 0; i < values.size() - 1; i++)
            oss << values[i] << delimeter;

        oss << values[values.size() - 1];
    }

    return oss.str();
}

int parse_int(const std::string value) {
    return is_int(value) ? stoi(value) : INT_MIN;
}

double parse_number(const std::string value) {
    return is_number(value) ? stod(value) : NAN;
}

int pow2(const int b) {
    if (b == 0)
        return 1;
    
    return pow(2, ceil(log(b) / log(2)));
}

std::vector<std::string> split(const std::string string, const std::string delimeter) {
    std::vector<std::string> result;
    size_t                   start = 0;

    for (int end = 0; end <= (int)string.length() - (int)delimeter.length(); end++) {
        size_t index = 0;

        while (index < delimeter.length() && string[end + index] == delimeter[index])
            index++;

        if (index == delimeter.length()) {
            result.push_back(string.substr(start, end - start));

            start = end + index;
        }
    }

    result.push_back(string.substr(start));

    return result;
}

void split(std::vector<std::string>& target, const std::string source, const std::string delimeter) {
    size_t start = 0;

    for (int end = 0; end <= (int)source.length() - (int)delimeter.length(); end++) {
        size_t index = 0;

        while (index < delimeter.length() && source[end + index] == delimeter[index])
            index++;

        if (index == delimeter.length()) {
            target.push_back(source.substr(start, end - start));

            start = end + index;
        }
    }

    target.push_back(source.substr(start));
}

std::vector<std::string> tokens(const std::string string) {
    std::vector<std::string> result;

    for (size_t start = 0, end = 0; end < string.length(); end++) {
        while (end < string.length() && isspace(string[end]))
            end++;

        start = end;

        while (end < string.length() && !isspace(string[end]))
            end++;

        if (start != end)
            result.push_back(string.substr(start, end - start));
    }

    return result;
}

void tokens(std::vector<std::string>& target, const std::string source) {
    for (size_t start = 0, end = 0; end < source.length(); end++) {
        while (end < source.length() && isspace(source[end]))
            end++;
        
        start = end;
        
        while (end < source.length() && !isspace(source[end]))
            end++;
        
        if (start != end)
            target.push_back(source.substr(start, end - start));
    }
}

std::string tolowerstr(const std::string string) {
    std::string str = string;

    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

std::string trim(const std::string string) {
    // Find leading whitespace
    size_t start = 0;
    
    while (start < string.length() && isspace(string[start]))
        start++;
    
    // Find trailing whitespace
    size_t end = string.length();
    
    while (end > start && isspace(string[end - 1]))
        end--;
        
    return string.substr(start, end - start);
}


std::string trim_end(const std::string string) {
    size_t end = string.length();

    while (end > 0 && isspace(string[end - 1]))
        end--;

    return string.substr(0, end);
}
