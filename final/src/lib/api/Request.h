#include "utils/rest_client.h"
#include "lib/api/Response.h"
#include <JsonParserGeneratorRK.h>

RestClient client = RestClient("chessss.herokuapp.com");

class Request {
public:
  Response post(String url) {
    JsonParser parser;

    Serial.println("- POST: " + url);
    String body;
    int statusCode = client.get(url, &body);

    Serial.println("Response:");
    Serial.println(statusCode);
    Serial.println(body);
    Serial.println();

    parser.addString(body);
    if (parser.parse()) {
      bool success;
      String message;

      parser.getOuterValueByKey("success", success);
      parser.getOuterValueByKey("message", message);

      return Response(success, message, parser);
    } else {
      Serial.println("!! JSON PARSE FAILED !!");
      return Response(false, "JSON Parse Failed", parser);
    }
  }
};
