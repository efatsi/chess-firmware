#include <JsonParserGeneratorRK.h>

class Response {
public:
  bool parsable;
  int statusCode;
  String body;
  JsonParser parser;

  Response(int s, String b) {
    statusCode = s;
    body = b;

    parser.clear();
    parser.addString(body);
    parsable = parser.parse();
  }

  bool success() {
    if (!parsable) return false;

    bool s;
    parser.getOuterValueByKey("success", s);

    return s;
  }

  String message() {
    if (!parsable) return "";

    String s;
    parser.getOuterValueByKey("message", s);

    return s;
  }

  String dig(String key) {
    if (!parsable) return "";

    String value;
    parser.getOuterValueByKey(key, value);

    return value;
  }

  String error() {
    if (parsable) {
      return message();
    } else {
      return "Parsing failed";
    }
  }
};
