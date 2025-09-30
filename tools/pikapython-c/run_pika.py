#!/usr/bin/env python3
"""run_pika.py

单一职责：临时替换入口 + （可选）注入一个已有根目录模块 (--module) + 构建 + 运行 + 恢复 + 清理。
无任何历史兼容或多模式逻辑；所有路径清晰：
1. 复制脚本 -> main.py
2. 若 --module 则复制 <module>.pyi 与 C -> 源树 & 强制刷新 cmake
3. 预构建 (存在工具才执行)
4. make
5. 运行 (可 --no-run 跳过)
6. 恢复 main.py & 删除临时注入文件
"""
from __future__ import annotations
import argparse
import os
import shutil
import subprocess
import sys
import datetime
from pathlib import Path

# --- 工具函数：打印文件最后 N 行 ---
def print_last_lines(file_path: Path, n: int):
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        for line in lines[-n:]:
            print(line.rstrip('\n'))
    except Exception as e:
        print(f"[run_pika][WARN] 读取日志失败 {file_path}: {e}")

PROJECT_ROOT = Path(__file__).parent.resolve()
PIKA_LINUX_DIR = PROJECT_ROOT / "pikapython-linux"
PIKA_DIR = PIKA_LINUX_DIR / "pikapython"
TARGET_MAIN = PIKA_DIR / "main.py"
LOG_ROOT = PROJECT_ROOT / "logs" / "run"

ENV_VERBOSE = os.getenv("VERBOSE", "0") == "1"

def log(msg: str):
    print(f"[run_pika] {msg}")

def debug(msg: str):
    if ENV_VERBOSE:
        print(f"[run_pika][DEBUG] {msg}")

class CommandError(RuntimeError):
    def __init__(self, cmd: list[str] | str, returncode: int):
        super().__init__(f"Command failed (exit {returncode}): {cmd}")
        self.cmd = cmd
        self.returncode = returncode


def run_cmd(cmd, cwd: Path | None = None, log_file: Path | None = None, env: dict | None = None):
    """运行命令，将所有输出写入 log_file，不直接打印，失败时抛出异常。

    返回：命令执行耗时（秒, float）。
    """
    if isinstance(cmd, (list, tuple)):
        display = " ".join(cmd)
    else:
        display = cmd
    debug(f"RUN: {display} (cwd={cwd})")
    start = datetime.datetime.now()
    with (open(log_file, "a", encoding="utf-8") if log_file else open(os.devnull, "w")) as lf:
        lf.write(f"\n[CMD] {display}\n")
        lf.flush()
        process = subprocess.Popen(
            cmd,
            cwd=str(cwd) if cwd else None,
            stdout=lf,
            stderr=subprocess.STDOUT,
            text=True,
            env=env or os.environ.copy(),
        )
        process.wait()
    dur = (datetime.datetime.now() - start).total_seconds()
    if process.returncode != 0:
        raise CommandError(display, process.returncode)
    return dur


def prepare_logs_dir() -> Path:
    ts = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    run_dir = LOG_ROOT / ts
    run_dir.mkdir(parents=True, exist_ok=True)
    return run_dir


def parse_args():
    parser = argparse.ArgumentParser(description="临时替换 main.py, 编译并运行 pikapython")
    parser.add_argument("script", help="要作为入口的 .py 文件 (相对或绝对路径)")
    parser.add_argument("--jobs", "-j", type=int, default=16, help="make 并行度 (默认 16)")
    parser.add_argument("--no-run", action="store_true", help="只编译不运行可执行文件")
    parser.add_argument("--tail-lines", type=int, default=5, help="终端仅显示编译/运行日志最后 N 行 (默认 5)")
    parser.add_argument("--fail-tail", type=int, default=20, help="失败时显示最后 N 行 (默认 20)")
    parser.add_argument("--module", dest="module", help="注入模块名 (目录名=模块名)，仅复制（无自动生成）")
    parser.add_argument("--module-dir", dest="module_dir", help="模块目录的父目录（默认在项目根目录查找同名目录）")
    return parser.parse_args()

# ---- 辅助输出格式化函数与失败路径 tail 处理 (需在 main 前定义) ----
def print_section_header(title: str, lines: int):
    bar = '=' * 8
    print(f"\n{bar} {title} (last {lines}) {bar}\n")


def print_section_footer():
    print("\n=============== END ===============")


def tail_failure_logs(args, compile_log: Path, run_log: Path):
    tail_n = getattr(args, 'fail_tail', 20)
    log("失败摘要输出：")
    if compile_log.exists():
        print_section_header("COMPILE FAIL TAIL", tail_n)
        print_last_lines(compile_log, tail_n)
        print_section_footer()
    if run_log.exists():
        print_section_header("RUN FAIL TAIL", tail_n)
        print_last_lines(run_log, tail_n)
        print_section_footer()
    log("日志路径汇总 =>")
    if compile_log.exists():
        log(f"compile_log: {compile_log}")

def main():
    args = parse_args()
    script_path = Path(args.script).resolve()
    if not script_path.exists() or script_path.suffix != '.py':
        log(f"[ERROR] 脚本不存在或不是 .py: {script_path}")
        return 2
    if not TARGET_MAIN.exists():
        log(f"[ERROR] 目标 main.py 不存在: {TARGET_MAIN}")
        return 2
    logs_dir = prepare_logs_dir()
    compile_log = logs_dir / 'compile.log'
    run_log = logs_dir / 'run.log'
    log(f"日志目录: {logs_dir}")
    backup_main = TARGET_MAIN.with_name(f"main.py.bak.{datetime.datetime.now().strftime('%Y%m%d%H%M%S')}")
    log(f"备份 {TARGET_MAIN} -> {backup_main}")
    shutil.copy2(TARGET_MAIN, backup_main)
    log(f"复制 {script_path} -> {TARGET_MAIN}")
    shutil.copy2(script_path, TARGET_MAIN)

    injected = None
    if getattr(args, 'module', None):
        mod_name = args.module.strip()
        import re
        if not re.match(r'^[A-Za-z][A-Za-z0-9_]*$', mod_name):
            log(f"[ERROR] --module 名称非法: {mod_name}")
            return 2
        base_dir = PROJECT_ROOT if not getattr(args, 'module_dir', None) else Path(args.module_dir).resolve()
        template_dir = base_dir / mod_name
        pyi_src = template_dir / f"{mod_name}.pyi"
        c_src_list = list(template_dir.glob(f"{mod_name}_*.c"))
        if not template_dir.is_dir() or not pyi_src.exists() or not c_src_list:
            log(f"[ERROR] 根目录缺少合法模块目录: {template_dir} (需要 {mod_name}.pyi 与 {mod_name}_*.c)")
            return 2
        # 目标路径（临时）
        pyi_dst = PIKA_DIR / f"{mod_name}.pyi"
        c_dst_dir = PIKA_DIR / "pikascript-lib" / mod_name
        # 若之前遗留同名（可能上次异常未清理），先移除再复制，保持单路径
        if pyi_dst.exists():
            try:
                pyi_dst.unlink()
                log(f"[WARN] 移除遗留 {pyi_dst}")
            except Exception as e:
                log(f"[ERROR] 无法删除遗留 pyi: {e}")
                return 2
        if c_dst_dir.exists():
            try:
                shutil.rmtree(c_dst_dir)
                log(f"[WARN] 移除遗留目录 {c_dst_dir}")
            except Exception as e:
                log(f"[ERROR] 无法删除遗留目录: {e}")
                return 2
        try:
            shutil.copy2(pyi_src, pyi_dst)
            c_dst_dir.mkdir(parents=True, exist_ok=False)
            for cfile in c_src_list:
                shutil.copy2(cfile, c_dst_dir / cfile.name)
            # 确保 main.py 顶部 import
            main_code = TARGET_MAIN.read_text(encoding='utf-8')
            if f"import {mod_name}" not in main_code.splitlines()[:10]:
                TARGET_MAIN.write_text(f"import {mod_name}\n" + main_code, encoding='utf-8')
            injected = { 'mod_name': mod_name, 'pyi': pyi_dst, 'c_dir': c_dst_dir }
            log(f"已注入模块(复制): {mod_name}")
        except Exception as ie:
            log(f"[ERROR] 注入失败: {ie}")
            # 回滚已复制
            if pyi_dst.exists():
                try: pyi_dst.unlink()
                except Exception: pass
            if c_dst_dir.exists():
                try: shutil.rmtree(c_dst_dir)
                except Exception: pass
            return 2

    def restore():
        if backup_main.exists():
            shutil.move(str(backup_main), str(TARGET_MAIN))
            log("已恢复 main.py")
        if injected:
            # 清理复制的临时模块
            if injected['pyi'].exists():
                try: injected['pyi'].unlink()
                except Exception as e: log(f"[WARN] 删除临时 pyi 失败: {e}")
            if injected['c_dir'].exists():
                try: shutil.rmtree(injected['c_dir'])
                except Exception as e: log(f"[WARN] 删除临时模块目录失败: {e}")
            log(f"已清理注入模块 {injected['mod_name']}")

    try:
        build_dir = PIKA_LINUX_DIR / 'build'
        # 新策略：始终强制干净构建。删除已有 build/ 目录并重新 cmake。
        if build_dir.exists():
            log('检测到已有 build/ -> 强制删除以确保干净构建')
            try:
                shutil.rmtree(build_dir)
                log('已删除旧 build/ 目录')
            except Exception as e:
                log(f'[ERROR] 无法删除 build/ 目录: {e}')
                return 2
        # 同步清理自动生成的 pikascript-api/ 目录，确保绑定文件全量重新生成
        api_dir = PIKA_DIR / 'pikascript-api'
        if api_dir.exists():
            log('检测到已有 pikascript-api/ -> 强制删除以确保绑定干净')
            try:
                shutil.rmtree(api_dir)
                log('已删除旧 pikascript-api/ 目录')
            except Exception as e:
                log(f'[ERROR] 无法删除 pikascript-api/ 目录: {e}')
                return 2
        # 顺序调整：预构建工具需要在 cmake 之前生成/刷新绑定 & 源文件，再进行 cmake 配置。
        pre_tool = PIKA_DIR / 'rust-msc-latest-win10.exe'
        if pre_tool.exists():
            log('执行预构建 (cmake 前, 输出写入 compile.log)')
            run_cmd(['wine', pre_tool.name], cwd=PIKA_DIR, log_file=compile_log)
        else:
            log(f'[WARN] 预构建工具不存在: {pre_tool}')
        log('执行 cmake 初始化 (预构建后, 强制干净构建, 输出写入 compile.log)')
        build_dir.mkdir(parents=True, exist_ok=True)
        run_cmd(['cmake', '..'], cwd=build_dir, log_file=compile_log)
        log('执行 make (输出写入 compile.log)')
        run_cmd(['make', f'-j{args.jobs}'], cwd=build_dir, log_file=compile_log)
        exe = build_dir / 'pikapython'
        if not args.no_run:
            if exe.exists():
                log('运行 pikapython (输出写入 run.log)')
                run_cmd([str(exe)], cwd=PIKA_LINUX_DIR, log_file=run_log)
            else:
                log(f'[ERROR] 可执行文件不存在: {exe}')
                return 3
    except CommandError as e:
        log(f'[ERROR] {e}')
        tail_failure_logs(args, compile_log, run_log)
        return e.returncode if e.returncode != 0 else 1
    except Exception as e:
        log(f'[ERROR] 未处理异常: {e}')
        tail_failure_logs(args, compile_log, run_log)
        return 1
    finally:
        restore()

    log('执行完成，摘要输出：')
    print_section_header('COMPILE TAIL', args.tail_lines)
    print_last_lines(compile_log, args.tail_lines)
    print_section_footer()
    if not args.no_run and run_log.exists():
        print_section_header('RUN TAIL', args.tail_lines)
        print_last_lines(run_log, args.tail_lines)
        print_section_footer()
    log('日志路径汇总 =>')
    log(f'compile_log: {compile_log}')
    if run_log.exists() and not args.no_run:
        log(f'run_log:     {run_log}')
    return 0

if __name__ == '__main__':
    sys.exit(main())
