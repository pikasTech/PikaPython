import configparser


first = configparser.ConfigParser()
first.read_string("[stable]\nalpha = 1\nbeta = x=y\n")
second = configparser.ConfigParser()
second.read_string("[stable]\nalpha = 2\n")
second.set("stable", "alpha", "3")
print("isolation:", first.get("stable", "alpha") == "1")

before = str(first)
try:
    first.read_string("[broken]\nvalue = one\nVALUE = two\n")
    print("failure: accepted")
except Exception:
    print("failure: rejected")
print("recovery:", str(first) == before)

expected = "[stable]\nalpha = 1\nbeta = x=y\n\n"
print("serialization:", str(first) == expected)
first["Extra"] = {"Enabled": "yes"}
extra = first["Extra"]
print("subscription:", extra["enabled"] == "yes")
extra["Mode"] = "FAST"
print("subscription-case:", extra["MODE"] == "FAST")
print("subscription-fallback:", extra.get("missing", "fallback") == "fallback")
print("has-option:", first.has_option("Extra", "mode"))
print("remove-option:", first.remove_option("Extra", "MODE"))
print("remove-section:", first.remove_section("Extra"))

try:
    first.read("missing.ini")
    print("read-provider: accepted")
except RuntimeError:
    print("read-provider: rejected")

try:
    first.write(None)
    print("write-provider: accepted")
except RuntimeError:
    print("write-provider: rejected")
