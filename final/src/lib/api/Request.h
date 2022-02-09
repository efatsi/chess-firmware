#include "utils/rest_client.h"
#include "lib/api/Response.h"

RestClient client = RestClient("chessss.herokuapp.com");

class Request {
public:
  Response post(String url) {
    Serial.println("- POST: " + url);
    String body;
    int statusCode = client.post(url, &body);

    Serial.println();
    Serial.println("Response:");
    Serial.println(statusCode);
    Serial.println(body);

    return Response(statusCode, body);
  }
};
