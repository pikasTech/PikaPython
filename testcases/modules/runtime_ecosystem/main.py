import eventloop
import os
import unittest


case = unittest.TestCase()
case.assertEqual(os.path.join("/tmp", "x"), "/tmp/x")
case.assertEqual(os.path.basename("/tmp/x"), "x")
values = []
loop = eventloop.EventLoop()
loop.start_new_task_once(lambda: 7, (), lambda value: values.append(value), "once")
loop.run_once(loop.tasks["once"].next_due)
case.assertEqual(values, [7])
case.assertEqual(loop.run_once(), 0)
print("[RUNTIME_ECOSYSTEM] mqtt=os=eventloop=unittest ok")
