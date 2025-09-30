"""示例脚本：与 --module 注入配套使用。

严格按 REQUIRE.md：
运行命令：
	python run_pika.py --module test_module_example test_example.py

脚本自身不负责放置模块文件；模块目录需预先存在于根目录：
	./test_module_example/
		test_module_example.pyi
		test_module_example_Test.c

run_pika.py 在本次运行期间会：
1. 临时复制 .pyi 与 C 到 pikapython-linux/pikapython/ 下
2. 强制（若需要）重新 cmake+预编译+编译
3. 运行后清理临时复制的文件与目录
"""
import time
import test_module_example

# Python baseline implementation (for performance comparison)
def py_add(a: int, b: int) -> int:
	return a + b

obj = test_module_example.Test()

# Functional checks
print('[EXAMPLE] add(7, 35)=', obj.add(7, 35))
print('[EXAMPLE] greet:"', obj.greet('demo'), '"', sep='')
assert obj.add(1, 2) == 3
assert obj.greet('x').startswith('Hello,')

# Performance comparison (simplified for Pika runtime): run both versions 10000 times
ITER = 10000
py_total = 0.0
c_total = 0.0
for _ in range(ITER):
	t0 = time.time()
	py_add(123, 456)
	py_total += (time.time() - t0)
	t1 = time.time()
	obj.add(123, 456)
	c_total += (time.time() - t1)

if ITER > 0:
	py_mean = py_total / ITER
	c_mean = c_total / ITER
else:
	py_mean = 0.0
	c_mean = 0.0
if c_mean > 0:
	ratio = py_mean / c_mean
else:
	ratio = 0.0
py_mean_us = py_mean * 1000000.0
c_mean_us = c_mean * 1000000.0
print('[PERF] python_total=' + ('%.6f' % py_total) + 's mean=' + ('%.2f' % py_mean_us) + 'us')
print('[PERF] cmod_total=' + ('%.6f' % c_total) + 's mean=' + ('%.2f' % c_mean_us) + 'us')
print('[PERF] speedup(py_mean/c_mean)= ' + ('%.2f' % ratio) + 'x')

print('[EXAMPLE][SELFTEST] test_module_example OK')
