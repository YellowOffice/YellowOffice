#include <YellowCommon/Test.h>
#include <YellowCommon/Json.h>

using YellowCommon::JsonValue;

TEST(JsonParseObject)
{
    JsonValue value;
    BString error;
    CHECK(JsonValue::Parse("{\"a\": 1, \"b\": \"text\", \"c\": true}", &value, &error));
    CHECK_EQUAL(1.0, value.Get("a").AsNumber());
    CHECK_STREQ("text", value.Get("b").AsString().String());
    CHECK(value.Get("c").AsBool());
}

TEST(JsonParseNested)
{
    JsonValue value;
    CHECK(JsonValue::Parse("{\"outer\": {\"inner\": [1, 2, 3]}}", &value));
    const JsonValue& inner = value.Get("outer").Get("inner");
    CHECK_EQUAL(3, inner.CountItems());
    CHECK_EQUAL(2.0, inner.At(1).AsNumber());
}

TEST(JsonRoundTrip)
{
    JsonValue original;
    original.Set("name", JsonValue("YellowDocs"));
    original.Set("count", JsonValue((int32)42));
    original.Set("ratio", JsonValue(0.5));
    original.Set("enabled", JsonValue(true));

    BString text = original.Serialize();
    JsonValue parsed;
    CHECK(JsonValue::Parse(text, &parsed));
    CHECK_STREQ("YellowDocs", parsed.Get("name").AsString().String());
    CHECK_EQUAL(42, parsed.Get("count").AsInt());
    CHECK_EQUAL(0.5, parsed.Get("ratio").AsNumber());
    CHECK(parsed.Get("enabled").AsBool());
}

TEST(JsonParseError)
{
    JsonValue value;
    BString error;
    CHECK(!JsonValue::Parse("{invalid}", &value, &error));
    CHECK(!error.IsEmpty());
}

TEST(JsonStringEscaping)
{
    JsonValue value;
    value.Set("s", JsonValue("line1\nline2\t\"quoted\""));
    BString text = value.Serialize();
    JsonValue parsed;
    CHECK(JsonValue::Parse(text, &parsed));
    CHECK_STREQ("line1\nline2\t\"quoted\"", parsed.Get("s").AsString().String());
}
