import configparser


parser = configparser.ConfigParser()
result = parser.read_string(
    "# service configuration\r\n"
    "[Server]\r\n"
    "Host = example.com\r\n"
    "token = left=right==tail\r\n"
    "; ignored comment\r\n"
    "\r\n"
    "[Client]\r\n"
    "Mode: FAST\r\n"
)
print("read:", result is None)
print("sections:", parser.sections())
print("options:", parser.options("Server"))
print("host:", parser.get("Server", "HOST"))
print("token:", parser.get("Server", "token"))
print("items:", parser.items("Client"))

changed = parser.set("Server", "Host", "internal")
print("set:", changed is None, parser.get("Server", "host"))
server = parser["Server"]
print("subscription-read:", server["token"])
print("subscription-case:", server["HOST"])
print("subscription-get:", server.get("missing"), server.get("missing", "fallback"))
print("subscription-keys:", list(server.keys()))
server["Port"] = "8080"
print("subscription-set:", server["port"])
parser["Extra"] = {"Enabled": "yes"}
extra = parser["Extra"]
print("subscription-write:", extra["enabled"])
print("has-section:", parser.has_section("Server"), parser.has_section("server"))
print("has-option:", parser.has_option("Server", "HOST"), parser.has_option("Missing", "x"))
print("optionxform:", parser.optionxform("MiXeD"))
print("remove-option:", parser.remove_option("Server", "PORT"), parser.remove_option("Server", "PORT"))
print("remove-section:", parser.remove_section("Extra"), parser.remove_section("Extra"))

empty = configparser.ConfigParser()
print("read-none:", empty.read_string(None), empty.sections())

try:
    duplicate = configparser.ConfigParser()
    duplicate.read_string("[dup]\nkey = first\nKEY = second\n")
    print("duplicate: accepted")
except Exception:
    print("duplicate: rejected")

try:
    invalid = configparser.ConfigParser()
    invalid.read_string("key = value\n")
    print("invalid-section: accepted")
except Exception:
    print("invalid-section: rejected")

try:
    parser.get("Missing", "value")
    print("missing-section: accepted")
except Exception:
    print("missing-section: rejected")

try:
    parser.get("Server", "missing")
    print("missing-option: accepted")
except Exception:
    print("missing-option: rejected")

try:
    parser.set("Server", "invalid", 1)
    print("set-type: accepted")
except TypeError:
    print("set-type: rejected")

try:
    invalid_type = configparser.ConfigParser()
    invalid_type.read_string(1)
    print("read-type: accepted")
except TypeError:
    print("read-type: rejected")
