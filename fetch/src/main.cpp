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
        { "content-type", "application/json" }
    };

    string      url = "http://localhost:8081/greeting";
    // string      url = "http://localhost:8081/no-reply";

    string      method = "POST";
    string      body = "{ \"firstName\": \"Corey\" }";

    try {
        auto response = request(headers, url, method, body);
        // auto response = request(headers, "http://calapi.inadiutorium.cz/api/v0/en/calendars/general-en/today");

        // Not supported
        // auto response = request(headers, "https://vpic.nhtsa.dot.gov/api/vehicles/getallmakes?format=json");

        cout << response.text() << endl;
    } catch (fetch::error& e) {
        if (e.text().length())
            throw runtime_error(e.text());

        throw e;
    }
}
