//
//  util.h
//  fetch
//
//  Created by Corey Ferguson on 9/1/25.
//

#ifndef util_h
#define util_h

#include <iostream>
#include <random>
#include <sstream>

// Non-Member Functions

/**
 * Decode double quotation-escaped string
 */
std::string decode(const std::string string);

/**
 * Return string escaped by double quotations
 */
std::string encode(const std::string string);

bool        is_pow(const size_t b, const size_t n);

/**
 * Return true if value includes double quotations, otherwise return false
 */
bool        is_string_literal(const std::string value);

/**
 * Return the next power of two
 */
int         pow2(const int b);

void        tokens(std::vector<std::string>& target, const std::string source);
    
std::string uuid();

#endif /* util_h */
