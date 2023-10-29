# 欢迎使用你的 VS Code 扩展

## 文件夹内容

* 此文件夹包含了扩展所需的所有文件。
* `package.json` - 这是你声明扩展和命令的清单文件。
  * 示例插件注册了一个命令并定义了其标题和命令名称。有了这些信息，VS Code 可以在命令面板中显示该命令。此时它还不需要加载插件。
* `src/extension.ts` - 这是你将为命令提供实现的主文件。
  * 文件导出了一个函数，`activate`，当你的扩展首次激活时（在此案例中是通过执行命令）会调用该函数。在`activate`函数内部，我们调用`registerCommand`。
  * 我们将包含命令实现的函数作为`registerCommand`的第二个参数传递。

## 立即启动和运行

* 按 `F5` 打开一个新窗口，加载你的扩展。
* 通过按下 (`Ctrl+Shift+P` 或在 Mac 上的 `Cmd+Shift+P`) 并输入 `Hello World` 从命令面板运行你的命令。
* 在 `src/extension.ts` 中的代码内设置断点，以调试你的扩展。
* 在调试控制台中查找你的扩展的输出。

## 进行修改

* 在 `src/extension.ts` 中更改代码后，你可以从调试工具栏重新启动扩展。
* 你还可以重新加载包含你的扩展的 VS Code 窗口 (`Ctrl+R` 或在 Mac 上的 `Cmd+R`) 以加载你的更改。

## 探索 API

* 当你打开文件 `node_modules/@types/vscode/index.d.ts` 时，你可以查看我们API的完整集合。

## 运行测试

* 打开调试视图 (`Ctrl+Shift+D` 或在 Mac 上的 `Cmd+Shift+D`) 并从启动配置下拉列表中选择 `Extension Tests`。
* 按 `F5` 在一个新窗口中运行测试，并加载你的扩展。
* 在调试控制台中查看测试结果的输出。
* 对 `src/test/suite/extension.test.ts` 进行更改或在 `test/suite` 文件夹内创建新的测试文件。
  * 所提供的测试运行器只会考虑名称匹配 `**.test.ts` 的文件。
  * 你可以在 `test` 文件夹内创建文件夹，根据你的需求结构化你的测试。

## 更进一步

* [遵循 UX 指导方针](https://code.visualstudio.com/api/ux-guidelines/overview) 创建与 VS Code 的原生界面和模式无缝集成的扩展。
* 通过 [为你的扩展进行打包](https://code.visualstudio.com/api/working-with-extensions/bundling-extension) 减少扩展大小并提高启动时间。
* 在 VS Code 扩展市场上 [发布你的扩展](https://code.visualstudio.com/api/working-with-extensions/publishing-extension)。
* 通过设置 [持续集成](https://code.visualstudio.com/api/working-with-extensions/continuous-integration) 自动化构建。
