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
    string              url = "http://localhost:8080/greeting";
    string              method = "POST";
    string              body = "{ \"firstName\": \"Corey\" }";
    map<string, string> headers = {{ "Content-Type", "application/json" }};

    try {
        fetch::response response = request(url, method, body, headers);

        cout << response.text() << endl;
    } catch (fetch::error& e) {
        if (e.text().length())
            throw runtime_error(e.text());

        throw e;
    }
}
