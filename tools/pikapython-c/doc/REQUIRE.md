1. 不要包含任何模板生成，不要包含任何自动回退，不要包含任何 fallback 机制，不要包含任何 default 机制！必须，简单！一件事情只用一种方法做！不要多路径，不要多模式，不要多分支！就是用 test_module_example 作为一个普通的模块，作为参考示例，不要有任何自动生成！！！！！

2. --module 的用法是注入已有的模块到 pikapython-linux 中！必须保留！例如 --module test_module_example 会注入 test_module_example.pyi 到 pikapython-linux/pikapython/，并注入 C 实现到 pikapython-linux/pikapython/pikascript-lib/test_module_example/ 目录下！然后预编译器会扫描到这个模块并生成绑定代码！运行完毕后会自动清理掉注入的 .pyi 和 C 目录！

3. 应该有一个 test_exmaple.py，放到 GUIDE.md，确保 run_pika.py --module test_module_example test_example.py 能跑通！并且 test_example.py 里要 import test_module_example 并调用里面的方法，这是一个示例，是给用户参考然后让用户开发真实业务功能的。

4. 应该指引 agent 创建测试的时候，也创建 py 版本的函数（直接写到测试代码里面），功能测试完成后要进行性能测试，例如让py和c模块的版本各自运行100次，计算时间（用time模块实现计时）test_module_example 和 test_example.py 也要对应修改，符合要求

5. 禁止，严禁使用任何 mock 机制，禁止任何 MockLLM 之类的东西，必须，必须，必须使用真实的 LLM 进行交互，如果遇到错误比如网络问题应该报错退出而不是任何 mock，任何 fallback，任何虚伪数据！！！！！！

6. 删去所有 _success_detect 的代码，必须**由agent自己做决定，是否完成了**，禁止任何的自动检测机制。

7. 解决 compile.log 的返回的路径错误的问题，必须要返回真实的路径，并不存在 `logs/run/latest/` 目录，必须返回 `logs/run/<timestamp>/run.log` 这样的路径，给出真实的，实际对应的路径。
