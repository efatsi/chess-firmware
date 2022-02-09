#include <JsonParserGeneratorRK.h>

class Response {
public:
  bool success;
  String message;
  JsonParser parser;

  Response(bool s, String m, JsonParser p) {
    success = s;
    message = m;
    parser = p;
  }

  String dig(String key) {
    String value;
    parser.getOuterValueByKey(key, value);

    return value;
  }
};
