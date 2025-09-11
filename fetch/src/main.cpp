//
//  main.cpp
//  fetch
//
//  Created by Corey Ferguson on 9/1/25.
//

#include "fetch.h"

using namespace fetch;
using namespace std;

int main(int argc, const char * argv[]) {
    header::map headers = {
        { "accept", (vector<string>){ "application/json", "text/plain" }},
        { "content-type", "application/json" }
    };

    string      url = "http://localhost:8081/greeting";
    // string      url = "http://localhost:8081/no-reply";

    string      method = "POST";
    string      body = "{ \"firstName\": \"Corey\" }";

    try {
        auto response = request(headers, url, method, body);

        cout << response.text() << endl;
    } catch (fetch::error& e) {
        if (e.text().length())
            throw fetch::error(e.status(), e.text(), e.text(), e.headers());

        throw e;
    }
}
