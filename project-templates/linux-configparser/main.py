import configparser


first = configparser.ConfigParser()
first.read_string(
    "# service configuration\r\n"
    "[Server]\r\n"
    "Host = example.com\r\n"
    "token = left=right==tail\r\n"
    "; ignored comment\r\n"
    "\r\n"
    "[Client]\r\n"
    "Mode: FAST\r\n"
)
second = configparser.ConfigParser()
second.read_string("[Server]\nHost = second.example\n")
second.set("Server", "Host", "changed.example")
print("isolation:", first.get("Server", "host") == "example.com")
print("sections:", first.sections())
print("options:", first.options("Server"))
print("token:", first.get("Server", "TOKEN"))
print("items:", first.items("Client"))
server = first["Server"]
print("subscription:", server["HOST"], server.get("missing", "fallback"))
print("keys:", list(server.keys()))
server["Port"] = "8080"
print("subscription-set:", server["port"])
first["Extra"] = {"Enabled": "yes"}
print("subscription-write:", first["Extra"]["enabled"])
print("has-section:", first.has_section("Server"), first.has_section("server"))
print("has-option:", first.has_option("Server", "PORT"))
print("remove-option:", first.remove_option("Server", "port"))
print("remove-section:", first.remove_section("Extra"))
try:
    duplicate = configparser.ConfigParser()
    duplicate.read_string("[dup]\nkey = first\nKEY = second\n")
    print("duplicate: accepted")
except Exception:
    print("duplicate: rejected")
try:
    invalid = configparser.ConfigParser()
    invalid.read_string("key = value\n")
    print("missing-section-header: accepted")
except Exception:
    print("missing-section-header: rejected")
try:
    first.get("Missing", "value")
    print("missing-section: accepted")
except Exception:
    print("missing-section: rejected")
try:
    first.get("Server", "missing")
    print("missing-option: accepted")
except Exception:
    print("missing-option: rejected")
try:
    first.set("Server", "invalid", 1)
    print("invalid-value-type: accepted")
except TypeError:
    print("invalid-value-type: rejected")
