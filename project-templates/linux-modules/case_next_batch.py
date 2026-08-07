import fsm
import flashdb
import i18n
import pika_posixfs


states = []


def first():
    states.append("first")
    return "second"


def second():
    states.append("second")
    return None


machine = fsm.StateMachine()
machine.addState(fsm.State("first", first))
machine.addState(fsm.State("second", second))
assert machine.start("first") == 0
assert machine.run_once() == "second"
assert machine.run_once() is None
assert states == ["first", "second"]

i18n.install({"hello": "你好"}, "zh-CN")
assert i18n._("hello") == "你好"
assert i18n._("missing") == "missing"
assert i18n.ngettext("item", "items", 2) == "items"

db = flashdb.KVDB("case", "/tmp/case", {}, None)
assert db.set_blob("k", bytes([1, 2, 3])) == 0
assert db.get_blob("k", 2) == [1, 2]

tsdb = flashdb.TSDB("case", "/tmp/case", 2, None)
assert tsdb.tsl_append(bytes([7])) == 0
assert tsdb.tsl_append(bytes([8])) == 0
seen = []


def collect(record, user):
    seen.append(record.to_blob())
    return 0


assert tsdb.tsl_iter(collect, None) == 0
assert len(seen) == 2

print("[NEXT_BATCH] fsm=i18n=pika_posixfs=flashdb ok")
