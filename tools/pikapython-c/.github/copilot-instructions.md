# Copilot Instructions for pikapython-c

## 项目架构概览
- 本项目为面向 PikaPython 的 AI 驱动 Python-C 编译加速器与自动化验证平台。
- 主要组件：
  - `Client.py`：核心入口，负责与 MCP 服务端交互，管理会话、工具加载、LLM 初始化和代理图构建。
  - `MCP_server.py`：MCP 服务端，基于 FastMCP，注册文件读写等工具，供客户端调用。
  - `MCP_config.py`：配置文件，包含模型、API、超参数等关键配置。
  - `Prompt.py`：存放自动化测试/验证相关的 prompt 模板。

## 关键开发流程
- 启动流程：
  1. `Client.py` 初始化 MCP 客户端（通过 `StdioServerParameters` 启动 `MCP_server.py`）。
  2. 加载工具（如文件读写），绑定到 LLM。
  3. 构建代理图（StateGraph），实现 agent-tool 交互。
- 工具注册与调用：
  - 服务端通过 `@mcp.tool()` 装饰器注册工具（如 `read_file`, `write_file`）。
  - 客户端通过 LLM 工具绑定和 agent graph 调用工具。
- 配置管理：
  - 所有模型、API、超参数均在 `MCP_config.py` 配置，需统一引用。

## 项目约定与模式
- 所有文件路径需使用 Linux 风格（如 `./` 前缀）。
- 工具函数需通过 MCP 工具注册，便于 agent 自动发现和调用。
- 分支命名采用 `Feat_xxx` 约定，贡献流程见 README。
- 测试/验证逻辑建议集中在 `Prompt.py`，便于 prompt 复用。

## 依赖与集成
- 依赖 `langchain_openai`, `langchain_mcp_adapters`, `fastmcp` 等库。
- LLM 相关参数（如模型名、API key）需从 `MCP_config.py` 读取。
- MCP 工具与 LLM 通过 `bind_tools` 集成。

## 示例：注册与调用工具
```python
# 服务端注册工具
@mcp.tool()
def read_file(file_path: str) -> str:
    ...

# 客户端调用工具
self.mcp_tools = await load_mcp_tools(self.session)
self.llm_with_tools = llm.bind_tools(self.mcp_tools)
```

## 重要文件参考
- `Client.py`：agent 入口与主流程
- `MCP_server.py`：工具注册与服务端逻辑
- `MCP_config.py`：统一配置
- `Prompt.py`：自动化验证模板

---
如有不清楚或遗漏的部分，请反馈以便进一步完善说明。