#include "utils/rest_client.h"

RestClient client = RestClient("6991-2601-5c2-201-7a90-1caf-5e81-96ed-61fd.ngrok.io");

void setup() {
    Serial.println("Hello World");
    delay(500);
    Serial.println("Making Request");

    String response;
    int statusCode = client.post("/games/4/move?move=d8-h4", &response);

    Serial.println("Response got");
    Serial.println(statusCode);
    Serial.println(response);
}

void loop() {
    delay(1000);
    Serial.println("Deed is done");
}
