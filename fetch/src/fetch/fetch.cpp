//
//  fetch.cpp
//  fetch
//
//  Created by Corey Ferguson on 9/2/25.
//

#include "fetch.h"

namespace fetch {
    // Constructors

    error::error(const size_t status, const std::string status_text, const std::string text, fetch::header::map headers) {
        this->_status = status;
        this->_status_text = status_text;
        this->_text = text;
        this->_headers = headers;
    }

    response::response(const size_t status, const std::string status_text, fetch::header::map headers, const std::string text) {
        this->_status = status;
        this->_status_text = status_text;
        this->_headers = headers;
        this->_text = text;
    }

    header::value::value() {
        this->_str = "";
    }

    header::value::value(const int value) {
        this->_int = value;
        this->_str = std::to_string(this->_int);
        this->_parsed = true;
    }

    header::value::value(const std::string value) {
        this->_str = value;
    }

    // Operators

    header::value::operator int() {
        int value;

        this->get(value);

        return value;
    }

    header::value::operator std::string() {
        return this->get();
    }

    int header::value::operator=(const int value) {
        this->_set(value);

        return this->_int;
    }

    std::string header::value::operator=(const std::string value) {
        this->_set(value);

        return this->_str;
    }

    bool header::value::operator==(const int value) {
        int _value;

        this->get(_value);

        return _value == value;
    }

    bool header::value::operator==(const std::string value) {
        return this->get() == value;
    }

    bool header::value::operator==(const struct value value) {
        return this->get() == value.get();
    }

    bool header::value::operator!=(const int value) {
        return !(*this == value);
    }

    bool header::value::operator!=(const std::string value) {
        return !(*this == value);
    }

    bool header::value::operator!=(const struct value value) {
        return !(*this == value);
    }

    // Member Functions

    int header::value::_set(const int value) {
        this->_int = value;
        this->_str = std::to_string(this->_int);

        return this->_int;
    }

    std::string header::value::_set(const std::string value) {
        this->_str = value;

        return this->_str;
    }

    header::value error::get(const std::string key) {
        return this->_headers[key];
    }

    header::value response::get(const std::string key) {
        return this->_headers[key];
    }

    std::string header::value::get() const {
        return this->_str;
    }

    void header::value::get(int& value) {
        if (!this->_parsed) {
            this->_int = parse_int(this->_str);
            this->_parsed = true;
        }

        value = this->_int;
    }

    fetch::header::map error::headers() {
        return this->_headers;
    }

    fetch::header::map response::headers() {
        return this->_headers;
    }

    size_t error::status() const {
        return this->_status;
    }

    size_t response::status() const {
        return this->_status;
    }

    std::string error::status_text() const {
        return this->_status_text;
    }

    std::string response::status_text() const {
        return this->_status_text;
    }

    std::string error::text() const {
        return this->_text;
    }

    std::string response::text() const {
        return this->_text;
    }

    const char* error::what() const throw() {
        return this->_status_text.c_str();
    }

    // Non-Member Fields

    size_t _timeout = 30;

    // Non-Member Functions

    response request(fetch::header::map& headers, const std::string url, const std::string method, const std::string body) {
        // Begin - Map start line
        // Map method
        std::stringstream ss(method + " ");

        // Set cursor to the end
        ss.seekp(0, std::ios::end);

        // Begin - Map target (path)
        // Begin - Parse host
        // Find host
        int start = 0;

        while (start < (int)url.length() - 1 && (url[start] != '/' || url[start + 1] != '/'))
            start++;
        
        start = start == url.length() - 1 ? 0 : start + 2;

        // Find target
        size_t end = start;

        while (end < url.length() && url[end] != '/')
            end++;

        // Parse host
        std::vector<std::string> host;

        split(host, url.substr(start, end - start), ":");

        if (host.size() == 1)
            throw fetch::error(0, "port is required");

        if (host[0] == "localhost")
            host[0] = "127.0.0.1";

        std::string _host = host[0] + ":" + host[1];
        // End - Parse host

        // Map target
        std::string target = url.substr(end);

        ss << target << " ";
        // End - Map target (path)

        // Map protocol
        ss << "HTTP/1.1\r\n";
        // End - Map start line

        // Begin - Map request headers
        // Sanitize headers
        for (const auto& [key, value]: headers) {
            std::string lower_key = tolowers(key);

            if (lower_key == "content-length" || lower_key == "host")
                headers.erase(key);
        }
        
        // Map host
        ss << "host: " << _host << "\r\n";

        // Map request headers
        if (body.length())
            headers["content-length"] = (int)body.length();

        for (const auto& [key, value]: headers)
            ss << key << ": " << value.get() << "\r\n";

        headers["host"] = _host;
        // End - Map request headers

        // Map body
        if (body.length()) {
            ss << "\r\n";
            ss << body << "\r\n";
        }

        ss << "\r\n";

#if LOGGING
       std::cout << ss.str() << std::endl;
#endif
    
        // Begin - Perform fetch
        std::atomic<bool> nores = true;

        try {
            mysocket::tcp_client* client = new mysocket::tcp_client(host[0], parse_int(host[1]));

            client->send(ss.str());

            // Begin - Listen for timeout
            std::thread([&nores, &client]() {
                for (size_t i = 0; i < timeout() && nores.load(); i++)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                if (nores.load()) {
                    nores.store(false);

                    // Sever connection
                    client->close();
                }
            }).detach();
            // End - Listen for timeout

            ss.str(client->recv());

            nores.store(false);
            
#if LOGGING
        std::cout << ss.str() << std::endl;
#endif

            client->close();
        } catch (mysocket::error& e) {
            if (nores.load())
                throw fetch::error(0, "Unknown error", e.what());

            throw fetch::error(0, "Unknown error", "Connection timed out");
        }

        // Server disconnected
        if (ss.str().empty())
            throw fetch::error(0, "Unknown error");
        // End - Perform fetch

        // Begin - Parse response
        std::string str;

        getline(ss, str);

        std::vector<std::string> tokens;

        ::tokens(tokens, str);

        // Begin - Parse status and status text
        size_t      status = stoi(tokens[1]);
        std::string status_text = tokens[2];

        // Merge status_text
        for (size_t i = 3; i < tokens.size(); i++)
            status_text += " " + tokens[i];
        // End - Parse status and status text

        // Parse response headers
        header::map _headers;

        while (getline(ss, str)) {
            std::vector<std::string> header;

            split(header, str, ":");
            
            // Empty line
            if (header.size() == 1)
                break;

            // Case-insensitive
            _headers[tolowers(header[0])] = trim(header[1]);
        }

        // Parse response body
        std::string        text;
        std::ostringstream oss;

        while (getline(ss, text))
            oss << trim_end(text) << "\r\n";

        int content_length;

        _headers["content-length"].get(content_length);

        size_t _content_length = content_length == INT_MIN ?
                _headers["transfer-encoding"] == "chunked" ?
                oss.str().length() :
                0 :
                content_length;

        text = oss.str().substr(0, _content_length);

        // Send response
        if (status < 200 || status >= 400)
            throw fetch::error(status, status_text, text, _headers);

        return fetch::response(status, status_text, _headers, text);
    }

    size_t& timeout() {
        return _timeout;
    }
}
