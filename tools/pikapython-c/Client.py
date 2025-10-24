from langchain_openai import ChatOpenAI
from langchain_mcp_adapters.tools import load_mcp_tools
from langgraph.graph import StateGraph, END, add_messages
from langchain_core.tools import BaseTool
from langchain_core.messages import SystemMessage, BaseMessage, HumanMessage, AIMessage, ToolMessage
from langgraph.types import interrupt, Command
from typing import TypedDict, Annotated, List, Dict, Any, Optional

from mcp import ClientSession, StdioServerParameters    
from mcp.client.stdio import stdio_client

from MCP_config import MODEL_NAME, BASE_URL, API_KEY, MAX_RETRYIES, TIME_OUT, debug_level, TEMPERATURE, MAX_TOKENS, RECURSION_LIMIT, LOG_ENABLED
from prompt.prompt_loader import load_prompt

import pyfiglet

import os
import asyncio
import json
import argparse
from datetime import datetime
from pathlib import Path
import traceback
import math


#定义状态类
class State(TypedDict):
    user_input: str
    user_id: str
    messages: Annotated[List[dict], add_messages]   # 消息信息


# 自定义条件判断函数
def should_continue(state: State) -> str:
    messages = state['messages']
    last_message = messages[-1]
    
    # 如果最后一条消息是AIMessage且有工具调用，则继续调用工具
    if isinstance(last_message, AIMessage) and last_message.tool_calls:
        return "tools"
    # 否则结束
    return "end"


class SingletonMeta(type):
    """单例元类"""
    _instances = {}
    
    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super().__call__(*args, **kwargs)
        return cls._instances[cls]


class PythonCTranspiler(metaclass=SingletonMeta):
    """Python到C转换器的代理类"""
    
    def __init__(self):
        # 配置MCP服务器参数
        self.server_params = StdioServerParameters(
            command="python",
            args=["MCP_server.py"]
        )
        self.llm_with_tools = None
        self.mcp_tools = None
        self.graph = None
        self.session = None
        self.initialized = False  # 明确设置初始化状态
        # 日志相关
        self.session_log_dir: Path | None = None
        self.llm_call_index: int = 0
        self.tool_call_index: int = 0
        self.root_log_dir: Path = Path("./logs")
        self._logging_enabled: bool = LOG_ENABLED
        # 动态工作目录 (用于隔离单次任务生成文件，避免冲突)
        ts = datetime.now().strftime('%Y%m%d_%H%M%S')
        self.session_work_dir: Path = Path("./file_create") / ts
        try:
            self.session_work_dir.mkdir(parents=True, exist_ok=True)
        except Exception:
            pass
        # 传递给 MCP 服务端用于写入限制
        os.environ['SESSION_WORK_DIR'] = str(self.session_work_dir.resolve())
        # 原始系统 prompt 文件名（在 main 中设置）
        self._base_prompt_filename: str | None = None
        # Token 用量累计
        self._usage_totals = {
            'completion_tokens': 0,
            'prompt_tokens': 0,
            'prompt_cache_hit_tokens': 0,
            'prompt_cache_miss_tokens': 0,
            'total_tokens': 0,
            'completion_tokens_details': {
                'reasoning_tokens': 0
            },
            'cost_yuan': 0.0
        }

    # ================= 日志辅助函数 =================
    def _init_session_logging(self):
        """初始化本次用户请求的日志目录。
        只在新的用户顶层输入时调用一次。"""
        if not self._logging_enabled:
            self.session_log_dir = None
            return
        now = datetime.now()
        session_dir_name = f"session_{now.strftime('%Y%m%d_%H%M%S')}"
        self.session_log_dir = self.root_log_dir / session_dir_name
        try:
            self.session_log_dir.mkdir(parents=True, exist_ok=True)
        except Exception:
            # 目录创建失败则置空，后续写日志自动跳过
            self.session_log_dir = None
        print(f"日志目录:{self.session_log_dir}")
        self.llm_call_index = 0
        self.tool_call_index = 0

    def _write_log(self, filename: str, data: dict | str):
        """写入单个日志文件。失败时静默。"""
        if not self._logging_enabled or self.session_log_dir is None:
            return
        try:
            file_path = self.session_log_dir / filename
            if isinstance(data, dict):
                content = json.dumps(data, ensure_ascii=False, indent=2)
            else:
                content = str(data)
            file_path.write_text(content, encoding='utf-8')
        except Exception:
            pass

    def _write_log_end(self, filename: str, data: dict | str):
        """写入结束日志文件。"""
        if not self._logging_enabled or self.session_log_dir is None:
            return
        try:
            file_path = self.session_log_dir / filename
            content = str(data)
            file_path.write_text(content, encoding='utf-8')
        except Exception:
            pass

    def set_logging(self, enabled: bool):
        """运行时切换日志开关。若关闭则之后不再写入新的日志。"""
        self._logging_enabled = bool(enabled)
        
    async def initialize(self):
        """初始化MCP会话和工具"""
        # 创建stdio客户端和会话
        # 手动创建stdio客户端
        if self.initialized:
            return self  # 如果已经初始化，直接返回
        self.stdio_client = stdio_client(self.server_params)
        self.read_stream, self.write_stream = await self.stdio_client.__aenter__()
        print("mcp客户端创建成功\n")

        # 创建会话
        self.session = ClientSession(self.read_stream, self.write_stream)
        await self.session.__aenter__()
        print("mcp会话创建成功\n")
            
         # 初始化连接
        await self.session.initialize()
        print("连接初始化成功\n")
            
        # 从MCP中获取工具
        self.mcp_tools = await load_mcp_tools(self.session)
            
        # 初始化LLM
        # 这里把 langchain 内置重试关闭 (max_retries=0), 由我们自定义的指数/线性混合回退控制
        # 避免重复重试导致延迟不可控
        llm = ChatOpenAI(
            model=MODEL_NAME,
            base_url=BASE_URL,
            api_key=API_KEY,
            temperature=TEMPERATURE,
            max_tokens=MAX_TOKENS,
            max_retries=0,
            timeout=TIME_OUT
        )
            
        # 将工具绑定到LLM
        self.llm_with_tools = llm.bind_tools(self.mcp_tools)
            
        # 创建代理图
        self._create_agent_graph()
        print("图创建成功")

        # 新的用户顶层输入：初始化 session 日志目录
        if not self.root_log_dir.exists():
                try:
                    self.root_log_dir.mkdir(parents=True, exist_ok=True)
                except Exception:
                    pass
        self._init_session_logging()
            
        self.initialized = True  # 初始化完成后设置标志

        return self
    
    async def close(self):
        """关闭会话和连接"""
        # 按正确顺序关闭：先 session.__aexit__ 再 stdio_client.__aexit__
        try:
            if self.session:
                try:
                    await self.session.__aexit__(None, None, None)
                except Exception as e:
                    print(f"[WARN] 关闭 session 异常: {e}")
        finally:
            if hasattr(self, 'stdio_client') and self.stdio_client:
                try:
                    await self.stdio_client.__aexit__(None, None, None)
                except Exception as e:
                    print(f"[WARN] 关闭 stdio_client 异常: {e}")
    
    async def __aenter__(self):
        """异步上下文管理器入口"""
        return await self.initialize()
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """异步上下文管理器退出"""
        await self.close()
    
    def _create_agent_graph(self):
        """创建代理图"""
        graph_builder = StateGraph(State)
        
        # 添加节点
        graph_builder.add_node("agent", self._call_model)
        graph_builder.add_node("tools", self._call_tool)
        
        # 设置入口点
        graph_builder.set_entry_point("agent")

        # 添加条件边
        graph_builder.add_conditional_edges(
            "agent",
            should_continue,
            {
                "tools": "tools",  # 需要工具调用
                "end": END         # 不需要工具调用，结束
            }
        )
        
        # 从工具节点返回代理节点
        graph_builder.add_edge("tools", "agent")
        
        # 编译图
        self.graph = graph_builder.compile()
    
    async def _call_model(self, state: State):
        """调用模型处理状态，增加自定义重试机制。

        重试策略:
        1. 默认延迟序列: 1,2,5,10,30,60,120 (秒)。
        2. 可通过环境变量 LLM_RETRY_DELAYS 覆盖，格式: "1,2,5" (秒，正整数/浮点)。解析失败则回退默认。
        3. 仅针对网络/额度/限流/临时性错误进行重试 (OpenAI 402余额不足仍按策略尝试直到序列结束, 便于在外部补款后继续)。
        4. 每次失败记录日志: retry_index, delay, error_type, error_message。
        5. 最终失败写入 final_error.log 并抛出异常 (让上游 graph 终止)。
        """
        messages = state['messages']

        # 解析自定义延迟序列
        default_delays = [1, 2, 5, 10, 30, 60, 120]
        env_delays_raw = os.getenv('LLM_RETRY_DELAYS')
        if env_delays_raw:
            try:
                parsed = []
                for part in env_delays_raw.split(','):
                    p = part.strip()
                    if not p:
                        continue
                    val = float(p)
                    if val <= 0:
                        continue
                    parsed.append(val)
                if parsed:
                    default_delays = parsed
            except Exception:
                # 解析失败静默回退
                pass

        attempts = len(default_delays) + 1  # 初始立即调用 + 延迟列表
        last_err: Exception | None = None
        for attempt in range(1, attempts + 1):
            try:
                response = await self.llm_with_tools.ainvoke(messages)
                break  # 成功
            except Exception as e:
                last_err = e
                # 记录失败日志
                log_payload = {
                    "phase": "LLM_CALL_RETRY_ERROR",
                    "attempt": attempt,
                    "max_attempts": attempts,
                    "error_type": e.__class__.__name__,
                    "error_message": str(e),
                }
                self._write_log('llm_retry.log', log_payload)
                # 终端提示: 当前失败 + 下次等待 (如果还会重试)
                if attempt < attempts:
                    delay = default_delays[attempt - 1] if attempt - 1 < len(default_delays) else default_delays[-1]
                    print(f"[LLM][Retry {attempt}/{attempts}] {e.__class__.__name__}: {str(e)[:140]} -- next wait {delay}s", flush=True)
                else:
                    print(f"[LLM][Retry {attempt}/{attempts}] {e.__class__.__name__}: {str(e)[:140]} -- no more retries", flush=True)
                if attempt == attempts:
                    # 达到最大次数，写终止日志
                    final_payload = {
                        "phase": "LLM_CALL_FINAL_FAILURE",
                        "attempts": attempts,
                        "error_type": e.__class__.__name__,
                        "error_message": str(e),
                    }
                    self._write_log('llm_final_error.log', final_payload)
                    print("[LLM][Abort] 已耗尽所有重试，任务终止。", flush=True)
                    raise  # 抛出最后的异常
                # 计算下一次的 delay
                delay = default_delays[attempt - 1] if attempt - 1 < len(default_delays) else default_delays[-1]
                try:
                    await asyncio.sleep(delay)
                except asyncio.CancelledError:
                    raise
                continue

        # 若循环未 break (理论不会), 直接抛出
        if last_err and 'response' not in locals():
            raise last_err

    # ========== 提取 usage 信息 (兼容不同字段结构) ==========
        usage_raw: dict | None = None
        try:
            # LangChain 常见: response.response_metadata.token_usage
            resp_meta = getattr(response, 'response_metadata', None)
            if isinstance(resp_meta, dict):
                if 'token_usage' in resp_meta and isinstance(resp_meta['token_usage'], dict):
                    usage_raw = resp_meta['token_usage']
                elif 'usage' in resp_meta and isinstance(resp_meta['usage'], dict):
                    usage_raw = resp_meta['usage']
            if usage_raw is None:
                add_kwargs = getattr(response, 'additional_kwargs', None)
                if isinstance(add_kwargs, dict):
                    # OpenAI 兼容格式: usage:{prompt_tokens,...}
                    if 'usage' in add_kwargs and isinstance(add_kwargs['usage'], dict):
                        usage_raw = add_kwargs['usage']
        except Exception:
            usage_raw = None

        usage_record = {}
        if usage_raw:
            # 标准化键名并提取
            ct = usage_raw.get('completion_tokens', 0)
            pt = usage_raw.get('prompt_tokens', 0)
            pch = usage_raw.get('prompt_cache_hit_tokens', 0)
            pcm = usage_raw.get('prompt_cache_miss_tokens', pt - pch) # 如果未提供，则计算
            tt = usage_raw.get('total_tokens', pt + ct)
            
            reasoning_tokens = 0
            if 'completion_tokens_details' in usage_raw and isinstance(usage_raw['completion_tokens_details'], dict):
                reasoning_tokens = usage_raw['completion_tokens_details'].get('reasoning_tokens', 0)

            # 计费规则 (元/百万 tokens)
            PRICE_CACHE_HIT = 0.5
            PRICE_CACHE_MISS = 4.0
            PRICE_OUTPUT = 12.0

            # 计算本次请求的费用
            cost = (
                (pch * PRICE_CACHE_HIT) +
                (pcm * PRICE_CACHE_MISS) +
                (ct * PRICE_OUTPUT)
            ) / 1_000_000

            # 累计
            self._usage_totals['completion_tokens'] += ct
            self._usage_totals['prompt_tokens'] += pt
            self._usage_totals['prompt_cache_hit_tokens'] += pch
            self._usage_totals['prompt_cache_miss_tokens'] += pcm
            self._usage_totals['total_tokens'] += tt
            self._usage_totals['completion_tokens_details']['reasoning_tokens'] += reasoning_tokens
            self._usage_totals['cost_yuan'] += cost

            usage_record = {
                'completion_tokens': ct,
                'prompt_tokens': pt,
                'prompt_cache_hit_tokens': pch,
                'prompt_cache_miss_tokens': pcm,
                'total_tokens': tt,
                'completion_tokens_details': {
                    'reasoning_tokens': reasoning_tokens
                },
                'cost_yuan': cost
            }
            # 每次更新后，写入 summary_stats.log
            self._write_log('summary_stats.log', self._usage_totals)

        # 日志记录
        self.llm_call_index += 1
        now = datetime.now().strftime('%H%M%S')
        log_filename = f"{now}_LLM{self.llm_call_index}.log"
        # 整理 messages 为可序列化
        def serialize_msg(m: BaseMessage):
            base = {
                "type": m.__class__.__name__,
                "content": getattr(m, 'content', None)
            }
            if isinstance(m, AIMessage):
                base["tool_calls"] = getattr(m, 'tool_calls', None)
            if isinstance(m, ToolMessage):
                base["name"] = getattr(m, 'name', None)
            return base

        serialized_messages = [serialize_msg(m) for m in messages]
        # 最新输入消息（倒数第二个可能是 Human / Tool 等，最后一个是 response 前的 AI 触发？此处取倒数第二个作为当前触发上下文，若存在）
        current_trigger_message = serialize_msg(messages[-1]) if messages else None
        log_payload = {
            "phase": "LLM_CALL",
            "model": MODEL_NAME,
            "parameters": {
                "base_url": BASE_URL,
                "temperature": TEMPERATURE,
                "max_tokens": MAX_TOKENS,
                "timeout": TIME_OUT,
            },
            "current_message": current_trigger_message,
            "response": serialize_msg(response),
            "history_messages": serialized_messages,
            "usage": usage_record,
            "usage_totals_accumulated": self._usage_totals
        }
        self._write_log(log_filename, log_payload)

        # 返回更新后的消息列表
        return {"messages": [response]}
    
    async def _call_tool(self, state: State):
        """调用工具处理状态"""
        messages = state['messages']
        last_message = messages[-1]
        
        # 确保最后一条消息是AIMessage且有工具调用
        if not isinstance(last_message, AIMessage) or not last_message.tool_calls:
            return {"messages": []}
        
        tool_calls = last_message.tool_calls
        results = []
        
        for tool_call in tool_calls:
            tool_name = tool_call['name']
            tool_args = tool_call['args']
            
            # 查找对应的工具实例
            tool_map = {t.name: t for t in self.mcp_tools}
            if tool_name not in tool_map:
                result = f"Error: 工具 {tool_name} 未找到"
            else:
                # 执行工具
                print(f"使用工具:{tool_name}")
                tool = tool_map[tool_name]
                try:
                    # 直接传递原始参数，不做兼容/修改
                    result = await tool.ainvoke(tool_args)
                    print(f"    调用工具成功: {tool_name} 输出: {result}")
                except Exception as e:
                    # 捕获所有异常，记录完整 traceback 到日志文件，并返回日志路径供上层 LLM 重试
                    tb = traceback.format_exc()
                    self.tool_call_index += 1
                    now = datetime.now().strftime('%H%M%S')
                    tool_log_filename = f"{now}_TOOL{self.tool_call_index}.log"
                    log_payload = {
                        "phase": "TOOL_CALL_EXCEPTION",
                        "tool_name": tool_name,
                        "args": tool_args,
                        "error_type": e.__class__.__name__,
                        "error_message": str(e),
                        "traceback": tb
                    }
                    self._write_log(tool_log_filename, log_payload)
                    # 将日志文件路径作为结果返回（相对路径）
                    result = f"ERROR_LOG_PATH: {(self.session_log_dir / tool_log_filename).as_posix()}"

            # 记录工具日志
            self.tool_call_index += 1
            now = datetime.now().strftime('%H%M%S')
            tool_log_filename = f"{now}_TOOL{self.tool_call_index}.log"
            tool_log_payload = {
                "phase": "TOOL_CALL",
                "tool_name": tool_name,
                "args": tool_args,
                "result": str(result)
            }
            self._write_log(tool_log_filename, tool_log_payload)
            
            # 为每个工具调用生成一个ToolMessage
            results.append(
                ToolMessage(
                    content=str(result),
                    name=tool_name,
                    tool_call_id=tool_call['id']
                )
            )
        
        # 返回更新后的消息列表 (不再包含自动成功总结逻辑)
        return {"messages": results}
    
    async def process_input(self, user_input: str, state: Optional[State] = None, system_prompt: str | None = None):
        """处理用户输入并返回更新后的状态"""
        if state is None:
            # 如果未显式传入，则动态加载并注入工作目录
            base_name = self._base_prompt_filename or 'core_task.md'
            sys_prompt_text = system_prompt if system_prompt is not None else self.prepare_system_prompt(base_name)
            state = State(messages=[SystemMessage(content=sys_prompt_text)])

        else:
            # 如果调用方已经提前构建了 state，但还未初始化日志，则此处补做
            if self.session_log_dir is None:
                if not self.root_log_dir.exists():
                    try:
                        self.root_log_dir.mkdir(parents=True, exist_ok=True)
                    except Exception:
                        pass
                self._init_session_logging()
        
        # 添加用户消息到状态
        state["messages"].append(HumanMessage(content=user_input))
        
        # 执行图
        invoke_config = {}
        if RECURSION_LIMIT is not None:
            invoke_config["recursion_limit"] = RECURSION_LIMIT
        result = await self.graph.ainvoke(state, config=invoke_config)


        # Client运行完后创建结束日志complate_message.log
        output_dict = {
            "user_input": result.get("user_input", ""),
            "user_id": result.get("user_id", ""),
            "messages": result.get("messages", "")
        }
        self._write_log_end("complate_message.log", output_dict)
        
        return result

    # ================= Prompt 动态注入 =================
    def prepare_system_prompt(self, prompt_file: str) -> str:
        """加载系统 prompt 并动态注入本次会话的工作目录说明。

        规则:
        1. 将所有出现的 './file_create/' 替换为 当前工作目录 (末尾带 '/')。
        2. 支持占位符 '{{WORK_DIR}}' 被替换为当前工作目录。
        3. 追加一段约束说明，强制 LLM 仅在该目录下写文件：<work_dir><module_name>/...
        """
        raw = load_prompt(prompt_file)
        work_dir_str = self.session_work_dir.as_posix() + "/"
        replaced = raw.replace('./file_create/', work_dir_str)
        replaced = replaced.replace('{{WORK_DIR}}', work_dir_str)
        # 规范 run_pika 命令里 --module-dir 参数为动态目录
        # 常见原始示例中使用: --module-dir ./file_create 或省略时间戳，因此替换这类片段
        replaced = replaced.replace('--module-dir ./file_create', f'--module-dir {work_dir_str.rstrip("/")}')
        # 如果 prompt 指出命令示例中直接写 test_example.py 路径，也替换为动态的
        replaced = replaced.replace(' ./file_create/test_example.py', f' {work_dir_str}test_example.py')
        appendix = (
            f"\n\n### 动态工作目录 (自动插入)\n"
            f"本次任务独立工作根目录: {work_dir_str}\n"
            "所有生成/修改文件必须位于该目录 (及其子目录)。禁止写入根仓库其它路径; 若需要读取日志/源码可只读不写。\n"
            "写入非该目录会被工具层直接拒绝。\n"
            "模块目录结构示例: <WORK_DIR><module_name>/<module_name>.pyi 与 C 实现文件。\n"
            f"运行构建示例命令: python run_pika.py --module <module_name> --module-dir {work_dir_str.rstrip('/')} {work_dir_str}test_example.py\n"\
            "\n### 环境限制\n"
            "1. 禁止使用 f-string 语法 (形如 f\"...{x}\").\n"
            "2. 禁止使用 round() 函数。\n"
            "违反上述任一会导致额外修补循环，必须一次性规避。\n"
            "\n### 成功判定与终止策略\n"
            "当首次在运行/自测输出中同时出现 'SELFTEST' 与 'OK' (或生成 [MODULE] 模块汇总块) 视为整体成功。随后立即: \n"
            "1. 输出 [SUMMARY] 段落(列出模块名/文件列表/步骤统计)。\n"
            "2. 不再提出新的工具调用或修改请求，直接结束。\n"
            "禁止在成功后继续追加改进操作; 改进建议只在 [SUMMARY] 里简述一行。\n"
        )
        return replaced + appendix



# 主函数
async def main():
    parser = argparse.ArgumentParser(description="Python->PikaPython 模块转换 Agent")
    parser.add_argument('--code', help='直接传入一段待转换的 Python 代码 (非交互模式)')
    parser.add_argument('--code-file', help='从文件读取待转换 Python 代码 (与 --code 互斥)')
    parser.add_argument('--prompt-file', default='core_task.md', help='指定使用的系统 prompt 文件名 (位于 prompt/ 下)')
    args = parser.parse_args()

    pyfig = pyfiglet.figlet_format("Python->C")
    print(pyfig)
    print("="*30)
    print("\033[1;33;40m llm驱动的python-C跨语言编译系统 (Pika集成路径)\033[0m")
    print("="*30)
    print("\n")

    # 装载系统 prompt
    try:
        system_prompt_text = load_prompt(args.prompt_file)
    except FileNotFoundError as e:
        print(f"[FATAL] Prompt 文件不存在: {e}")
        return

    transpiler = PythonCTranspiler()
    transpiler._base_prompt_filename = args.prompt_file
    await transpiler.initialize()

    # 初始 state
    # 使用动态注入后的 prompt
    dynamic_prompt = transpiler.prepare_system_prompt(args.prompt_file)
    state = State(messages=[SystemMessage(content=dynamic_prompt)])

    # 非交互一次性模式
    if args.code or args.code_file:
        start_time = datetime.now() # 记录开始时间
        if args.code and args.code_file:
            print('[ERROR] --code 与 --code-file 不能同时使用')
            return
        # 如果指定了代码或代码文件，则自动推断模块名并注入
        if args.code_file:
            code_path = Path(args.code_file)
            try:
                code_text = code_path.read_text(encoding='utf-8')
            except Exception as e:
                print(f'[ERROR] 读取代码文件失败: {e}')
                return
            
            inferred_module = code_path.stem.replace('-', '_').replace(' ', '_')
            hint = f"# MODULE_NAME_HINT: {inferred_module}"
            # 避免重复重复注入
            if not code_text.lstrip().startswith('# MODULE_NAME_HINT:'):
                code_text = hint + code_text
        else:
            code_text = args.code

        result_state = await transpiler.process_input(code_text, state, system_prompt=system_prompt_text)
        if result_state["messages"] and isinstance(result_state["messages"][-1], AIMessage):
            print(f"AI: {result_state['messages'][-1].content}")
        
        # 打印详细的 Token 使用量和费用
        end_time = datetime.now()
        duration = end_time - start_time
        total_seconds = int(duration.total_seconds())
        hours, remainder = divmod(total_seconds, 3600)
        minutes, seconds = divmod(remainder, 60)

        cache_hit_tokens = transpiler._usage_totals.get('prompt_cache_hit_tokens', 0)
        cache_miss_tokens = transpiler._usage_totals.get('prompt_cache_miss_tokens', 0)
        completion_tokens = transpiler._usage_totals.get('completion_tokens', 0)
        total_cost = transpiler._usage_totals.get('cost_yuan', 0.0)
        total_tool_calls = transpiler.tool_call_index

        # 计费规则 (元/百万 tokens)
        PRICE_CACHE_HIT = 0.5
        PRICE_CACHE_MISS = 4.0
        PRICE_OUTPUT = 12.0

        cost_cache_hit = (cache_hit_tokens * PRICE_CACHE_HIT) / 1_000_000
        cost_cache_miss = (cache_miss_tokens * PRICE_CACHE_MISS) / 1_000_000
        cost_completion = (completion_tokens * PRICE_OUTPUT) / 1_000_000

        # 费用占比（防止除零）
        denom = total_cost if total_cost > 0 else (cost_cache_hit + cost_cache_miss + cost_completion)
        if denom == 0:
            pct_cache_hit = pct_cache_miss = pct_completion = 0.0
        else:
            pct_cache_hit = cost_cache_hit / denom * 100
            pct_cache_miss = cost_cache_miss / denom * 100
            pct_completion = cost_completion / denom * 100

        print(f"\n{'='*30}\nUsage & Stats Summary:\n")
        print(f"  - Cache Input:   {cache_hit_tokens / 1000:.1f}k tokens ({cost_cache_hit:.3f} 元, {pct_cache_hit:.2f}%)")
        print(f"  - Fresh Input:   {cache_miss_tokens / 1000:.1f}k tokens ({cost_cache_miss:.3f} 元, {pct_cache_miss:.2f}%)")
        print(f"  - Output:        {completion_tokens / 1000:.1f}k tokens ({cost_completion:.3f} 元, {pct_completion:.2f}%)")
        print(f"  - Tool Calls:    {total_tool_calls}")
        print(f"  - Total Time:    {hours}h {minutes}m {seconds}s")
        print(f"\n{'='*30}")
        print(f"Total Cost: {total_cost:.3f} 元\n{'='*30}")

        # 退出前关闭资源
        await transpiler.close()
        return
        # 若不 exit, 继续进入交互
        state = result_state
        
        # 打印详细的 Token 使用量和费用
        end_time = datetime.now()
        duration = end_time - start_time
        total_seconds = int(duration.total_seconds())
        hours, remainder = divmod(total_seconds, 3600)
        minutes, seconds = divmod(remainder, 60)

        cache_hit_tokens = transpiler._usage_totals.get('prompt_cache_hit_tokens', 0)
        cache_miss_tokens = transpiler._usage_totals.get('prompt_cache_miss_tokens', 0)
        completion_tokens = transpiler._usage_totals.get('completion_tokens', 0)
        total_cost = transpiler._usage_totals.get('cost_yuan', 0.0)
        total_tool_calls = transpiler.tool_call_index

        # 计费规则 (元/百万 tokens)
        PRICE_CACHE_HIT = 0.5
        PRICE_CACHE_MISS = 4.0
        PRICE_OUTPUT = 12.0

        cost_cache_hit = (cache_hit_tokens * PRICE_CACHE_HIT) / 1_000_000
        cost_cache_miss = (cache_miss_tokens * PRICE_CACHE_MISS) / 1_000_000
        cost_completion = (completion_tokens * PRICE_OUTPUT) / 1_000_000

        print(f"\n{'='*30}\nUsage & Stats Summary:\n")
        print(f"  - Cache Input:   {cache_hit_tokens / 1000:.2f}k tokens ({cost_cache_hit:.6f} 元)")
        print(f"  - Fresh Input:   {cache_miss_tokens / 1000:.2f}k tokens ({cost_cache_miss:.6f} 元)")
        print(f"  - Output:        {completion_tokens / 1000:.2f}k tokens ({cost_completion:.6f} 元)")
        print(f"  - Tool Calls:    {total_tool_calls}")
        print(f"  - Total Time:    {hours}h {minutes}m {seconds}s")
        print(f"\n{'='*30}")
        print(f"Total Cost: {total_cost:.6f} 元\n{'='*30}")

        # 退出前关闭资源
        await transpiler.close()
        return
        # 若不 exit, 继续进入交互
        state = result_state

    # 交互循环
    while True:
        try:
            user_input = input("你: ").strip()
        except EOFError:
            break
        if user_input.lower() in ["退出", "exit", "quit"]:
            print("再见!")
            break
        if not user_input:
            continue
        state = await transpiler.process_input(user_input, state, system_prompt=system_prompt_text)
        if state["messages"] and isinstance(state["messages"][-1], AIMessage):
            print(f"AI: {state['messages'][-1].content}")

if __name__ == "__main__":
    asyncio.run(main())



# 简化接口函数
def initialize_agent():
    """初始化agent（单例模式会自动处理）"""
    return ChatAgent()

def get_agent_response(agent_instance, user_input):
    return agent_instance.process_message(user_input)

def get_agent_status(agent_instance):
    return {
        'initialized_time': agent_instance.initialized_time,
        'total_conversations': len(agent_instance.conversation_history) // 2,
        'model_loaded': agent_instance.model_loaded
    }