from quart import Quart, render_template, request, jsonify, Response
from Client import PythonCTranspiler, AIMessage
import asyncio
import aiofiles
from contextlib import asynccontextmanager
from typing import Optional, AsyncIterator
import re
import json
from pathlib import Path

# 全局agent实例
agent_instance: Optional[PythonCTranspiler] = None

# 配置扫描路径
LOG_DIR = None  # 日志文件目录
FILE_DIR = None  # 文件扫描目录

# 存储已读取的日志信息和历史内容
file_history = []
processed_files = set()

app = Quart(__name__)

# 在应用启动前执行
@app.before_serving
async def startup():
    print("=== Application Starting ===")
    global agent_instance
    global LOG_DIR
    global FILE_DIR
    agent_instance = await PythonCTranspiler().initialize()
    # 获取日志存储路径
    if LOG_DIR == None:
        LOG_DIR = agent_instance.session_log_dir
        print(f"Get log_dir:{LOG_DIR}")
    # 获取文件存储路径
    if FILE_DIR == None:
        FILE_DIR = agent_instance.session_work_dir
        print(f"Get file_dir:{FILE_DIR}")
    
    print("Agent initialized!")

# 在应用关闭后执行  
@app.after_serving
async def shutdown():
    print("=== Application Shutting Down ===")
    if agent_instance:
        await agent_instance.close()
    print("Agent closed!")

# 构建网页界面
@app.route('/')
async def index():
    print("In index")
    return await render_template('index.html')



@app.route('/chat', methods=['POST'])
async def chat():
    print("In chat")
    try:
        # 检查agent是否已经初始化   
        if not agent_instance or not getattr(agent_instance, 'initialized', False):
            return jsonify({'error': 'Agent未初始化，请稍后重试'}), 503

        data = await request.get_json()
        user_input = data.get('message', '').strip()
        
        if not user_input:
            return jsonify({'error': '输入不能为空'}), 400
        
        if not agent_instance:
            return jsonify({'error': 'Agent未初始化'}), 500
        
        # 使用全局agent实例处理输入
        state = await agent_instance.process_input(user_input)
        
        # 提取最终的AI响应
        if state["messages"] and isinstance(state["messages"][-1], AIMessage):
            agent_response = state['messages'][-1].content
        
        return jsonify({
            'user_input': user_input,
            'agent_response': agent_response,
            'success': True
        })
        
    except Exception as e:
        return jsonify({'error': f'处理请求时出错: {str(e)}'}), 500

# 实时获取日志内容
@app.route('/scan_logs', methods=['GET'])
async def scan_logs():
    print("scan_logs start")
    try:
        path = LOG_DIR
        print(f"log_dir:{path}")

        # 检查路径是否存在
        if not path.exists() or path is None:
            return jsonify({
                'success': False,
                'error': '日志目录未创建'
            }), 400

        """SSE流式传输新文件内容"""
        async def generate():
            n_value = 1
            max_n = 0  # 跟踪最大n值
            encoding = 'utf-8'

            while True:
                # 使用正则表达式匹配日志文件名格式：*****_LLM[n_value].log
                log_pattern = re.compile(fr"_LLM{n_value}\.log$")
                new_file = None

                # 获取匹配的日志文件
                for file in path.iterdir():
                    if file.is_file() and log_pattern.search(file.name):
                        new_file = file
                        break

                # 检查是否存在完成文件
                completion_file_path = path / "complate_message.log"
                
                if completion_file_path.exists() and new_file is None:
                    try:
                        completion_file_content = completion_file_path.read_text(encoding=encoding)
                        data = {
                            'filename': "complate_message.log",
                            'content': completion_file_content,
                            'n': max_n + 1,
                            'is_completion': True
                        }
                        data_json = json.dumps(data, ensure_ascii=False)
                        print(f"发送完成文件: {data_json}")
                        yield f"data: {data_json}\n\n"
                        print("检测到完成文件: complate_message.log")
                        break   # 完成文件后退出循环
                    except Exception as e:
                        data = {
                            'filename': "complate_message.log",
                            'content': f'读取完成文件时出错: {str(e)}',
                            'n': max_n + 1,
                            'is_completion': True
                        }
                        data_json = json.dumps(data, ensure_ascii=False)
                        yield f"data: {data_json}\n\n"
                        break

                elif new_file:                 
                    try:
                        # 打开并读取日志文件
                        file_content = new_file.read_text(encoding=encoding)
                            
                        # 将JSON字符串转换为Python对象
                        file_info = json.loads(file_content)

                        # 过滤日志内容，只保留需要的字段
                        filtered_info = {
                            'model': file_info.get('model'),
                            'parameters': file_info.get('parameters'),
                            'current_message': file_info.get('current_message'),
                            'response': file_info.get('response')
                        }
                        
                        # 移除空的字段
                        filtered_info = {k: v for k, v in filtered_info.items() if v is not None}

                        # 更新max_n
                        if n_value > max_n:
                            max_n = n_value

                        # 发送SSE事件 - 修正：发送完整的data结构
                        data = {
                            'filename': new_file.name,
                            'content': filtered_info,
                            'n': n_value,
                            'is_completion': False
                        }
                        data_json = json.dumps(data, ensure_ascii=False)
                        yield f"data: {data_json}\n\n"
                        
                        # n_value加一
                        n_value += 1

                    except Exception as e:
                        print(f"处理文件出错: {str(e)}")
                        data = {
                            'error': f'处理文件错误: {str(e)}',
                            'filename': new_file.name
                        }
                        data_json = json.dumps(data, ensure_ascii=False)  # 修正：定义data_json
                        yield f"data: {data_json}\n\n"
                        n_value += 1
                        
                else:
                    # 如果没有找到新文件，发送心跳保持连接
                    yield "data: {\"heartbeat\": true}\n\n"
                    
                # 每2秒检查一次
                await asyncio.sleep(2)
        
        return Response(
            generate(),
            mimetype='text/event-stream',
            headers={
                'Cache-Control': 'no-cache',
                'Connection': 'keep-alive',
                'Access-Control-Allow-Origin': '*',
            }
        )

    except Exception as e:
        print(f"scan_logs顶层错误: {str(e)}")
        return jsonify({
            'success': False,
            'error': f'服务器错误: {str(e)}'
        }), 500


# 获取file_create内文件内容
@app.route('/refresh_files', methods=['POST'])
async def refresh_files():
    try:
        files_data = []
        
        # 确保文件目录存在
        if not FILE_DIR.exists():
            return jsonify({'success': False, 'error': f'文件目录不存在: {FILE_DIR}'})
        
        # 递归扫描目录
        await scan_directory(FILE_DIR, files_data)
        
        return jsonify({'success': True, 'files': files_data})
    
    except Exception as e:
        return jsonify({'success': False, 'error': str(e)})


async def scan_directory(directory: Path, files_data: list, depth=0):
    """递归扫描目录"""
    try:
        # 首先添加当前目录的信息
        if depth > 0:  # 不显示根目录
            files_data.append({
                'type': 'directory',
                'name': directory.name,
                'path': str(directory.relative_to(FILE_DIR)),
                'depth': depth,
                'content': f"目录: {directory.name}"
            })
        
        # 遍历目录中的所有项目
        for item in directory.iterdir():
            if item.is_dir():
                # 递归扫描子目录
                await scan_directory(item, files_data, depth + 1)
            elif item.is_file():
                # 处理文件
                await process_file(item, files_data, depth + 1)
                
    except PermissionError:
        files_data.append({
            'type': 'error',
            'name': directory.name,
            'path': str(directory.relative_to(FILE_DIR)),
            'depth': depth,
            'content': '[权限不足，无法访问此目录]'
        })

async def process_file(file_path: Path, files_data: list, depth: int):
    """处理单个文件"""
    try:
        # 检查文件是否为二进制文件
        if await is_binary_file(file_path):
            content = '[二进制文件，无法显示内容]'
        else:
            # 尝试读取文件内容
            try:
                async with aiofiles.open(file_path, 'r', encoding='utf-8') as f:
                    content = await f.read()
            except UnicodeDecodeError:
                # 如果 UTF-8 解码失败，尝试其他编码
                try:
                    async with aiofiles.open(file_path, 'r', encoding='latin-1') as f:
                        content = await f.read()
                except Exception as e:
                    content = f'[读取文件时出错: {str(e)}]'
            except Exception as e:
                content = f'[读取文件时出错: {str(e)}]'
        
        files_data.append({
            'type': 'file',
            'name': file_path.name,
            'path': str(file_path.relative_to(FILE_DIR)),
            'depth': depth,
            'content': content
        })
        
    except Exception as e:
        files_data.append({
            'type': 'error',
            'name': file_path.name,
            'path': str(file_path.relative_to(FILE_DIR)),
            'depth': depth,
            'content': f'[处理文件时出错: {str(e)}]'
        })

async def is_binary_file(file_path: Path) -> bool:
    """检查文件是否为二进制文件"""
    try:
        # 检查文件扩展名（常见的文本文件扩展名）
        text_extensions = {'.txt', '.py', '.js', '.html', '.css', '.json', '.xml', 
                          '.csv', '.md', '.rst', '.yml', '.yaml', '.ini', '.cfg',
                          '.conf', '.log', '.sh', '.bat', '.ps1', '.java', '.c',
                          '.cpp', '.h', '.hpp', '.cs', '.php', '.rb', '.go', '.rs'}
        
        if file_path.suffix.lower() in text_extensions:
            return False
        
        # 进一步检查文件内容
        async with aiofiles.open(file_path, 'rb') as f:
            chunk = await f.read(1024)
        
        if not chunk:
            return False
        
        # 检查是否包含空字节（二进制文件的标志）
        if b'\0' in chunk:
            return True
        
        # 检查文本字符比例
        text_characters = bytearray({7, 8, 9, 10, 12, 13, 27} | set(range(0x20, 0x100)) - {0x7f})
        non_text = chunk.translate(None, text_characters)
        return float(len(non_text)) / len(chunk) > 0.3
        
    except Exception:
        return True

def is_text_file(file_path):
    """简单判断是否为文本文件"""
    text_extensions = {'.txt', '.log', '.py', '.js', '.html', '.css', '.json', '.xml', '.csv', '.md'}
    return file_path.suffix.lower() in text_extensions

if __name__ == '__main__':
    print("=== SCRIPT START ===")
    app.run(debug=True, host="0.0.0.0", port=5001)
    print("=== SCRIPT END ===")
    