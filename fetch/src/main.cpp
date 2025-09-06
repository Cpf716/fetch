//
//  main.cpp
//  fetch
//
//  Created by Corey Ferguson on 9/1/25.
//

#include "fetch.h"

using namespace fetch;
using namespace mysocket;
using namespace std;

map<string, string> headers = {{ "content-type", "application/json" }};

int main(int argc, const char * argv[]) {
    // auto server = new tcp_server(8082);

    string url = "http://localhost:8081/greeting";
    // string url = "http://localhost:8082/greeting";

    string method = "POST";
    string body = "{ \"firstName\": \"Corey\" }";

    try {
        auto response = request(headers, url, method, body);

        cout << response.text() << endl;
    } catch (fetch::error& e) {
        // server->close();

        if (e.text().length())
            throw fetch::error(e.status(), e.text(), e.text(), e.headers());

        throw e;
    }
}
