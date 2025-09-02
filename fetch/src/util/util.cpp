//
//  util.cpp
//  fetch
//
//  Created by Corey Ferguson on 9/1/25.
//

#include "util.h"

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

bool is_pow(const size_t b, const size_t n) {
    if (b == 0)
        return false;
    
    if (n == 0)
        return b < 2;
    
    int result = log(b) / log(n);
    
    return (int)result - result == 0;
}


bool is_string_literal(const std::string value) {
    size_t i = 0;

    while (i < value.length() && value[i] != '\"')
        i++;

    return i != value.length();
}

int pow2(const int b) {
    if (b == 0)
        return 1;
    
    return pow(2, ceil(log(b) / log(2)));
}

void tokens(std::vector<std::string>& target, const std::string source) {
    size_t start = 0;
    
    for (size_t end = 0; end < source.length(); end++) {
        while (end < source.length() && isspace(source[end]))
            end++;
        
        start = end;
        
        while (end < source.length() && !isspace(source[end]))
            end++;
        
        if (start != end)
            target.push_back(source.substr(start, end - start));
    }
}

std::string uuid() {
    std::ostringstream              oss;
    std::random_device              rd;
    std::mt19937_64                 gen(rd());
    std::uniform_int_distribution<> uid;
    
    for (size_t i = 0; i < 8; i++)
        oss << std::hex << uid(gen) % 16;
    
    oss << "-";
    
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 4; j++)
            oss << std::hex << uid(gen) % 16;
        
        oss << "-";
    }
    
    for (size_t i = 0; i < 12; i++)
        oss << std::hex << uid(gen) % 16;
    
    return oss.str();
}
