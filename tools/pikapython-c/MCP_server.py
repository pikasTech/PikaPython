# MCP_Server.py
from fastmcp import FastMCP
from ctypes import *

import os
import sys
import subprocess
import time
import json
from pathlib import Path

# 记录已 read 过的文件，用于写入前校验
_READ_CACHE = set()
# 记录本会话由 agent 创建或成功写入过的文件，允许后续直接覆盖
_WRITE_TRACK = set()

mcp = FastMCP("服务器")

@mcp.tool()
def read_file(file_path: str) -> str:
    r"""
    读取文件内容工具。

    args:
        file_path: 被读取的文件的地址 (使用linux系统,因此必须包含"./")

    returns:
        成功: 返回"读取成功!"和文件内容
        失败: 返回"ERROR:"和错误信息
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        _READ_CACHE.add(os.path.abspath(file_path))
        return f"读取成功!文件内容:{content}"
    except Exception as e:
        return f"ERROR: {str(e)}"



@mcp.tool()
def write_file(file_path: str, content: str, mode: str = 'w') -> str:
    r"""
    编辑文件内容工具。

    args:
        file_path: 被编辑的文件的地址 (使用linux系统,因此必须包含"./")
        content: 编辑的内容内容
        mode: 模式('默认覆盖模式'),'w'覆盖写入;'a'追加写入

    returns:
        成功: 返回"编辑成功!"和文件路径
        失败: 返回"ERROR:"和错误信息
    """
    try:
        abs_path = os.path.abspath(file_path)
        parent = Path(abs_path).parent
        # 动态工作目录限制
        session_root = os.environ.get('SESSION_WORK_DIR')
        if session_root:
            session_root_abs = os.path.abspath(session_root)
            # 允许写入: session_root 下的任意子路径
            if not abs_path.startswith(session_root_abs + os.sep):
                return (
                    "ERROR: 写入被拒绝。文件不在本次会话工作目录内: "
                    f"{file_path}. 允许根目录: {session_root_abs}/"
                )
        # 自动创建上级目录
        parent.mkdir(parents=True, exist_ok=True)
        # 覆盖策略：
        # 1. 若文件不存在 -> 直接写入并加入 _WRITE_TRACK
        # 2. 若文件存在且不是 append: 允许以下任一条件直接覆盖
        #    a) 曾被 read 过 (兼容旧逻辑)
        #    b) 曾在本会话通过 write_file 成功写入 (_WRITE_TRACK 命中)
        #    否则阻止并提示先 read 一次以确认外部文件语义
        exists_before = os.path.exists(abs_path)
        if exists_before and mode != 'a':
            if abs_path not in _READ_CACHE and abs_path not in _WRITE_TRACK:
                return (
                    "ERROR: 试图覆盖已存在文件但之前未 read，且非本会话创建: "
                    f"{file_path}. 如为外部文件请先调用 read_file；若期望直接覆盖，先执行一次 read 或首次写入。"
                )
        with open(abs_path, mode, encoding='utf-8') as f:
            f.write(content)
        # 标记写入跟踪（无论追加或覆盖）
        _WRITE_TRACK.add(abs_path)
        action = "追加" if (mode == 'a' and exists_before) else ("覆盖" if exists_before else "创建")
        return f"成功{action}文件: {file_path}"
    except Exception as e:
        return f"ERROR: {str(e)}"



@mcp.tool()
def run_bash(command: str) -> str:
    r"""
    执行bash命令并返回输出的工具。

    args:
        command: 在bash中执行的命令


    returns:
        成功: 返回"命令运行成功"和输入的命令以及对应的结果
        失败: 返回"ERROR:"和错误信息
    """
    try:
        result = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            timeout=30
        )
        if result.returncode == 0:
            return f"命令运行成功,[命令:{command}; {result.stdout}]"
        else:
            return f"ERROR: 命令:{command}; {result.stderr}"
    except subprocess.TimeoutExpired:
        return "ERROR: 命令执行超时"
    except Exception as e:
        return f"ERROR: {str(e)}"


@mcp.tool()
def run_shell(command: str) -> str:
    r"""
    执行 shell 命令并返回结构化 JSON 字符串。

    返回字段(JSON):
        command: 原始命令
        returncode: 进程退出码 (int 或 'TIMEOUT')
        stdout: 标准输出前若干字符 (全部保留, 若过长可由上层截断)
        stderr: 标准错误输出 (可能为空)
        duration_ms: 执行耗时 (毫秒, 仅非超时)
        compile_log: 若检测到 run_pika 输出中的 compile_log 行, 给出真实路径
        run_log: 若检测到 run_pika 输出中的 run_log 行, 给出真实路径
        note: 辅助说明
    错误时也返回 JSON (不抛异常), 由上层 agent 自行判断。
    """
    start = time.time()
    try:
        proc = subprocess.run(
            command,
            shell=True,
            capture_output=True,
            text=True,
            timeout=120
        )
        dur_ms = int((time.time() - start) * 1000)
        stdout = proc.stdout or ""
        stderr = proc.stderr or ""
        compile_log_path = None
        run_log_path = None
        # 解析 run_pika.py 标准输出中的日志路径行
        # 典型行: [run_pika] compile_log: /abs/path/to/compile.log
        for line in stdout.splitlines():
            line_strip = line.strip()
            if 'compile_log:' in line_strip:
                # 拆分最后的路径部分
                try:
                    compile_log_path = line_strip.split('compile_log:')[-1].strip()
                except Exception:
                    pass
            if 'run_log:' in line_strip:
                try:
                    run_log_path = line_strip.split('run_log:')[-1].strip()
                except Exception:
                    pass
        payload = {
            "command": command,
            "returncode": proc.returncode,
            "stdout": stdout,
            "stderr": stderr,
            "duration_ms": dur_ms,
            "compile_log": compile_log_path,
            "run_log": run_log_path,
            "note": "ok" if proc.returncode == 0 else "non-zero returncode"
        }
        return json.dumps(payload, ensure_ascii=False)
    except subprocess.TimeoutExpired:
        payload = {
            "command": command,
            "returncode": "TIMEOUT",
            "stdout": "",
            "stderr": "",
            "duration_ms": 120000,
            "compile_log": None,
            "run_log": None,
            "note": "timeout"
        }
        return json.dumps(payload, ensure_ascii=False)
    except Exception as e:
        payload = {
            "command": command,
            "returncode": "EXCEPTION",
            "stdout": "",
            "stderr": str(e),
            "duration_ms": int((time.time() - start) * 1000),
            "compile_log": None,
            "run_log": None,
            "note": "raised"
        }
        return json.dumps(payload, ensure_ascii=False)


if __name__ == "__main__":
    mcp.run()
