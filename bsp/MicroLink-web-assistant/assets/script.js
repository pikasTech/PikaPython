/**
 * MicroLink Web Serial Terminal
 * 使用 Web Serial API 实现浏览器直接访问串口
 */

class MicroLinkTerminal {
    constructor() {
        this.port = null;
        this.reader = null;
        this.writer = null;
        this.isConnected = false;
        this.customCommands = [];

        // 数据缓冲相关
        this.dataBuffer = '';
        this.lineTimeoutId = null;
        this.bufferStartTime = null; // 记录缓冲区开始接收数据的时间

        // 终端输入相关
        this.commandHistory = [];
        this.historyIndex = -1;
        this.logMessages = [];

        // 虚拟终端配置
        this.virtualTerminalMode = true; // 启用虚拟终端模式，实时发送按键
        this.virtualTerminalModeOriginalValue = true; // 保存用户原始设置值
        this.currentLine = ''; // 当前行缓冲

        this.initializeElements();
        this.bindEvents();
        this.updateSendOptions(); // 初始化发送选项状态
        this.loadParameters();
        this.checkWebSerialSupport();
    }

    initializeElements() {
        // 串口配置元素
        this.baudRateSelect = document.getElementById('baudRate');
        this.customBaudRateInput = document.getElementById('customBaudRate');
        this.dataBitsSelect = document.getElementById('dataBits');
        this.paritySelect = document.getElementById('parity');
        this.stopBitsSelect = document.getElementById('stopBits');

        // 连接控制元素
        this.connectBtn = document.getElementById('connectBtn');
        this.disconnectBtn = document.getElementById('disconnectBtn');
        this.clearBtn = document.getElementById('clearBtn');
        this.saveLogBtn = document.getElementById('saveLogBtn');
        this.connectionStatus = document.getElementById('connectionStatus');

        // 显示选项元素
        this.hexModeCheckbox = document.getElementById('hexMode');
        this.showTimestampCheckbox = document.getElementById('showTimestamp');
        this.autoScrollCheckbox = document.getElementById('autoScroll');
        this.enableBufferCheckbox = document.getElementById('enableBuffer');
        this.virtualTerminalModeCheckbox = document.getElementById('virtualTerminalMode');
        this.processAnsiSequencesCheckbox = document.getElementById('processAnsiSequences');
        this.debugModeCheckbox = document.getElementById('debugMode');
        this.lineTimeoutInput = document.getElementById('lineTimeout');

        // 终端元素
        this.terminal = document.getElementById('terminal');
        this.terminalOutput = document.getElementById('terminalOutput');
        this.terminalInputField = document.getElementById('terminalInputField');

        // API 控制元素
        this.rttAddrInput = document.getElementById('rttAddr');
        this.rttSizeInput = document.getElementById('rttSize');
        this.rttChannelInput = document.getElementById('rttChannel');
        this.startRTTBtn = document.getElementById('startRTT');
        this.stopRTTBtn = document.getElementById('stopRTT');

        this.svAddrInput = document.getElementById('svAddr');
        this.svSizeInput = document.getElementById('svSize');
        this.svChannelInput = document.getElementById('svChannel');
        this.startSystemViewBtn = document.getElementById('startSystemView');

        this.flmPathInput = document.getElementById('flmPath');
        this.baseAddrInput = document.getElementById('baseAddr');
        this.ramAddrInput = document.getElementById('ramAddr');
        this.loadFLMBtn = document.getElementById('loadFLM');

        this.binPathInput = document.getElementById('binPath');
        this.binAddrInput = document.getElementById('binAddr');
        this.loadBinBtn = document.getElementById('loadBin');
        this.offlineDownloadBtn = document.getElementById('offlineDownload');

        this.ymodemFileInput = document.getElementById('ymodemFile');
        this.ymodemSendBtn = document.getElementById('ymodemSend');

        this.customCommandInput = document.getElementById('customCommand');
        this.sendCustomBtn = document.getElementById('sendCustom');
        this.addCustomBtn = document.getElementById('addCustom');
        this.customCommandsList = document.getElementById('customCommandsList');

        this.saveParamsBtn = document.getElementById('saveParams');
        this.saveToFileBtn = document.getElementById('saveToFile');
        this.loadParamsBtn = document.getElementById('loadParams');
        this.resetParamsBtn = document.getElementById('resetParams');
        this.loadConfigFileBtn = document.getElementById('loadConfigFile');
        this.configFileInput = document.getElementById('configFileInput');
    }

    bindEvents() {
        // 波特率选择事件
        this.baudRateSelect.addEventListener('change', () => {
            if (this.baudRateSelect.value === 'custom') {
                this.customBaudRateInput.style.display = 'block';
            } else {
                this.customBaudRateInput.style.display = 'none';
            }
        });

        // 连接控制事件
        this.connectBtn.addEventListener('click', () => this.connectSerial());
        this.disconnectBtn.addEventListener('click', () => this.disconnectSerial());
        this.clearBtn.addEventListener('click', () => this.clearTerminal());
        this.saveLogBtn.addEventListener('click', () => this.saveLog());

        // 终端输入事件
        this.terminalInputField.addEventListener('keydown', (e) => this.handleTerminalInput(e));
        this.terminalInputField.addEventListener('focus', () => this.scrollToBottom());
        
        // 终端发送按钮事件
        this.terminalSendBtn = document.getElementById('terminalSendBtn');
        if (this.terminalSendBtn) {
            this.terminalSendBtn.addEventListener('click', () => this.executeTerminalCommand());
        }

        // 虚拟终端配置事件
        this.virtualTerminalModeCheckbox.addEventListener('change', () => {
            this.virtualTerminalMode = this.virtualTerminalModeCheckbox.checked;
            // 只有在非HEX模式下才保存用户的原始设置
            if (!this.hexModeCheckbox.checked) {
                this.virtualTerminalModeOriginalValue = this.virtualTerminalModeCheckbox.checked;
            }
            this.updateTerminalPlaceholder();
        });

        // HEX模式切换事件
        this.hexModeCheckbox.addEventListener('change', () => {
            this.updateSendOptions();
        });

        // API 控制事件
        this.startRTTBtn.addEventListener('click', () => this.startRTT());
        this.stopRTTBtn.addEventListener('click', () => this.stopRTT());
        this.startSystemViewBtn.addEventListener('click', () => this.startSystemView());
        this.loadFLMBtn.addEventListener('click', () => this.loadFLM());
        this.loadBinBtn.addEventListener('click', () => this.loadBin());
        this.offlineDownloadBtn.addEventListener('click', () => this.offlineDownload());
        this.ymodemSendBtn.addEventListener('click', () => this.ymodemSend());
        this.sendCustomBtn.addEventListener('click', () => this.sendCustomCommand());
        this.addCustomBtn.addEventListener('click', () => this.addCustomCommand());

        // 参数管理事件
        this.saveParamsBtn.addEventListener('click', () => this.saveParameters());
        this.saveToFileBtn.addEventListener('click', () => this.saveParametersToFile());
        this.loadParamsBtn.addEventListener('click', () => this.loadParameters());
        this.resetParamsBtn.addEventListener('click', () => this.resetParameters());
        this.loadConfigFileBtn.addEventListener('click', () => this.loadConfigFile());
        this.configFileInput.addEventListener('change', (e) => this.handleConfigFileSelect(e));

        // 统一YMODEM相关按钮事件绑定
        const flmYmodemSendBtn = document.getElementById('flmYmodemSendBtn');
        if (flmYmodemSendBtn) {
            flmYmodemSendBtn.addEventListener('click', () => this.handleFlmYmodemSend());
        }
        // Python发送按钮事件绑定在setupPythonScriptPanel中处理
    }

    checkWebSerialSupport() {
        if (!('serial' in navigator)) {
            this.addMessage('错误: 您的浏览器不支持 Web Serial API。请使用 Chrome 89+ 或 Edge 89+', 'error');
            this.connectBtn.disabled = true;
            this.connectBtn.innerHTML = '<i class="fas fa-times"></i> 不支持';
        }
    }

    async connectSerial() {
        try {
            // 请求串口权限
            this.port = await navigator.serial.requestPort();

            // 获取串口配置
            const baudRate = this.baudRateSelect.value === 'custom'
                ? parseInt(this.customBaudRateInput.value)
                : parseInt(this.baudRateSelect.value);

            const dataBits = parseInt(this.dataBitsSelect.value);
            const stopBits = parseInt(this.stopBitsSelect.value);
            const parity = this.paritySelect.value;

            // 打开串口
            await this.port.open({
                baudRate: baudRate,
                dataBits: dataBits,
                stopBits: stopBits,
                parity: parity
            });

            this.isConnected = true;
            this.updateConnectionStatus(true);

            // 格式化校验位显示
            const parityDisplay = parity === 'none' ? 'N' : parity === 'even' ? 'E' : 'O';
            this.addMessage(`串口连接成功 - 波特率: ${baudRate}, 数据位: ${dataBits}${parityDisplay}${stopBits}`, 'info');

            // 开始读取数据
            this.startReading();

        } catch (error) {
            this.addMessage(`连接失败: ${error.message}`, 'error');
        }
    }

    async disconnectSerial() {
        try {
            // 先设置断开标志，停止读取循环
            this.isConnected = false;

            // 释放reader锁
            if (this.reader) {
                try {
                    await this.reader.cancel();
                } catch (e) {
                    // 忽略cancel错误，继续释放锁
                    console.log('Reader cancel error:', e);
                }

                try {
                    this.reader.releaseLock();
                } catch (e) {
                    // 忽略releaseLock错误
                    console.log('Reader releaseLock error:', e);
                }
                this.reader = null;
            }

            // 释放writer锁
            if (this.writer) {
                try {
                    this.writer.releaseLock();
                } catch (e) {
                    console.log('Writer releaseLock error:', e);
                }
                this.writer = null;
            }

            // 关闭串口
            if (this.port) {
                await this.port.close();
                this.port = null;
            }

            this.updateConnectionStatus(false);

            // 清理数据缓冲区
            this.flushBuffer();

            this.addMessage('串口已断开', 'info');

        } catch (error) {
            this.addMessage(`断开连接失败: ${error.message}`, 'error');
            // 强制重置状态
            this.isConnected = false;
            this.reader = null;
            this.writer = null;
            this.port = null;
            this.updateConnectionStatus(false);
        }
    }

    async startReading() {
        if (!this.port) return;

        try {
            this.reader = this.port.readable.getReader();

            while (this.isConnected) {
                const { value, done } = await this.reader.read();
                if (done) break;

                this.handleReceivedData(value);
            }
        } catch (error) {
            if (this.isConnected) {
                this.addMessage(`读取数据错误: ${error.message}`, 'error');
            }
        } finally {
            if (this.reader) {
                try {
                    this.reader.releaseLock();
                } catch (e) {
                    console.log('Reader releaseLock in finally:', e);
                }
                this.reader = null;
            }
        }
    }

    handleReceivedData(data) {
        // 记录数据接收时间
        const receiveTime = new Date();

        // 检查是否处于曲线绘制模式
        if (chartDrawEnabled && window.chartData) {
            // 曲线绘制模式：直接处理串口数据，不显示在主监控
            // console.log(`[曲线模式] 接收到 ${data.length} 字节数据:`, Array.from(data).map(b => b.toString(16).padStart(2, '0').toUpperCase()).join(' '));
            
            // 处理分片数据，确保4字节对齐
            handleRealtimeHexDataChunkedSimple(data);
            
            // 不显示在主监控，直接返回
            return;
        }

        if (this.hexModeCheckbox.checked) {
            // HEX 模式显示
            const hexText = Array.from(data)
                .map(byte => byte.toString(16).padStart(2, '0').toUpperCase())
                .join(' ');

            if (this.enableBufferCheckbox.checked) {
                // HEX模式也使用缓冲和超时机制
                this.bufferHexData(hexText);
            } else {
                // 直接显示，不缓冲
                this.addMessage(hexText, 'received', receiveTime);
            }
        } else {
            // 文本模式显示
            const text = new TextDecoder().decode(data);

            if (this.enableBufferCheckbox.checked) {
                // 使用缓冲和超时机制
                this.bufferData(text);
            } else {
                // 直接显示，不缓冲，使用接收时间作为时间戳
                this.addMessage(text, 'received', receiveTime);
            }
        }
    }

    bufferData(text) {
        // 调试模式：显示接收到的原始数据
        if (this.debugModeCheckbox.checked) {
            const debugInfo = `[DEBUG] 接收数据 (${text.length}字节): ${JSON.stringify(text)}`;
            this.addMessage(debugInfo, 'info');
        }

        // 记录缓冲区开始时间（只在缓冲区为空时记录）
        if (this.dataBuffer.length === 0) {
            this.bufferStartTime = new Date();
        }

        // 将新数据添加到缓冲区
        this.dataBuffer += text;

        // 清除之前的超时（这是关键！每次新数据都重置超时）
        if (this.lineTimeoutId) {
            clearTimeout(this.lineTimeoutId);
            this.lineTimeoutId = null;

            if (this.debugModeCheckbox.checked) {
                this.addMessage('[DEBUG] 重置超时计时器', 'info');
            }
        }

        // 检查是否包含明确的结束标志
        const hasLineEnding = this.dataBuffer.includes('\n') || this.dataBuffer.includes('\r');

        // 检查是否是单独的shell提示符（应该保持在同一行）
        const isStandalonePrompt = this.dataBuffer.match(/^sh:\/\$\s*$/) ||
                                  this.dataBuffer.match(/^>>>\s*$/) ||
                                  this.dataBuffer.match(/^>\s*$/);

        // 检查是否包含完整的命令输出（以换行符+提示符结尾）
        const hasCompleteOutput = this.dataBuffer.match(/\n\s*sh:\/\$\s*$/) ||
                                 this.dataBuffer.match(/\r\n\s*sh:\/\$\s*$/) ||
                                 this.dataBuffer.match(/\n\s*>>>\s*$/) ||
                                 this.dataBuffer.match(/\r\n\s*>>>\s*$/);

        // 检查是否包含完整的命令列表结束
        const hasCommandListEnd = this.dataBuffer.includes('Command List:') &&
                                 (this.dataBuffer.includes('\nsh:/$ ') || this.dataBuffer.includes('\r\nsh:/$ '));

        // 检查缓冲区大小，如果太大就强制刷新
        const bufferTooLarge = this.dataBuffer.length > 2000;

        // 立即刷新的条件
        if (hasCompleteOutput || hasCommandListEnd || bufferTooLarge) {
            // 立即刷新，不等待超时
            this.flushBuffer();
            return;
        }

        // 如果是单独的提示符，不立即刷新，等待后续内容
        if (isStandalonePrompt) {
            // 使用较长的超时等待后续内容
            const timeout = parseInt(this.lineTimeoutInput.value) || 50;
            this.lineTimeoutId = setTimeout(() => {
                this.flushBuffer();
            }, timeout * 2); // 使用双倍超时时间
            return;
        }

        // 如果包含换行符，检查是否是完整的行
        if (hasLineEnding) {
            // 检查是否以换行符结尾（完整的行）
            const endsWithNewline = this.dataBuffer.endsWith('\n') || this.dataBuffer.endsWith('\r\n') || this.dataBuffer.endsWith('\r');

            if (endsWithNewline) {
                // 完整的行，使用较短的超时
                if (this.debugModeCheckbox.checked) {
                    this.addMessage(`[DEBUG] 检测到完整行，使用短超时(10ms)`, 'info');
                }
                this.lineTimeoutId = setTimeout(() => {
                    this.flushBuffer();
                }, 10);
            } else {
                // 包含换行符但不以换行符结尾，可能还有更多数据，使用配置的超时
                const timeout = parseInt(this.lineTimeoutInput.value) || 50;
                if (this.debugModeCheckbox.checked) {
                    this.addMessage(`[DEBUG] 包含换行符但不完整，使用配置超时(${timeout}ms)`, 'info');
                }
                this.lineTimeoutId = setTimeout(() => {
                    this.flushBuffer();
                }, timeout);
            }
            return;
        }

        // 默认情况下，设置超时等待更多数据
        const baseTimeout = parseInt(this.lineTimeoutInput.value) || 50;

        // 根据数据特征调整超时时间
        let timeout = baseTimeout;

        // 如果包含ANSI序列，使用更短的超时
        if (this.dataBuffer.includes('\x1b[')) {
            timeout = Math.min(baseTimeout, 20);
            if (this.debugModeCheckbox.checked) {
                this.addMessage(`[DEBUG] 检测到ANSI序列，调整超时为${timeout}ms`, 'info');
            }
        }

        // 如果数据看起来像是命令输出的一部分，使用更短的超时
        if (this.dataBuffer.includes('CMD') || this.dataBuffer.includes('--------')) {
            timeout = Math.min(baseTimeout, 15);
            if (this.debugModeCheckbox.checked) {
                this.addMessage(`[DEBUG] 检测到命令输出，调整超时为${timeout}ms`, 'info');
            }
        }

        if (this.debugModeCheckbox.checked) {
            this.addMessage(`[DEBUG] 设置默认超时: ${timeout}ms (基础: ${baseTimeout}ms)`, 'info');
        }

        this.lineTimeoutId = setTimeout(() => {
            this.flushBuffer();
        }, timeout);
    }

    bufferHexData(hexText) {
        // 调试模式：显示接收到的原始HEX数据
        if (this.debugModeCheckbox.checked) {
            const debugInfo = `[DEBUG] 接收HEX数据: ${hexText}`;
            this.addMessage(debugInfo, 'info');
        }

        // 记录缓冲区开始时间（只在缓冲区为空时记录）
        if (this.dataBuffer.length === 0) {
            this.bufferStartTime = new Date();
        }

        // 将新的HEX数据添加到缓冲区（用空格分隔）
        if (this.dataBuffer.length > 0) {
            this.dataBuffer += ' ' + hexText;
        } else {
            this.dataBuffer = hexText;
        }

        // 清除之前的超时（每次新数据都重置超时）
        if (this.lineTimeoutId) {
            clearTimeout(this.lineTimeoutId);
            this.lineTimeoutId = null;

            if (this.debugModeCheckbox.checked) {
                this.addMessage('[DEBUG] 重置HEX超时计时器', 'info');
            }
        }

        // HEX模式下的缓冲逻辑相对简单，主要基于超时
        const timeout = parseInt(this.lineTimeoutInput.value) || 50;

        // 检查缓冲区大小，如果太大就强制刷新
        const bufferTooLarge = this.dataBuffer.length > 1000; // HEX数据较长，适当增加限制

        if (bufferTooLarge) {
            if (this.debugModeCheckbox.checked) {
                this.addMessage('[DEBUG] HEX缓冲区过大，强制刷新', 'info');
            }
            this.flushBuffer();
            return;
        }

        if (this.debugModeCheckbox.checked) {
            this.addMessage(`[DEBUG] 设置HEX超时: ${timeout}ms`, 'info');
        }

        this.lineTimeoutId = setTimeout(() => {
            this.flushBuffer();
        }, timeout);
    }

    flushBuffer() {
        if (this.dataBuffer.length > 0) {
            if (this.debugModeCheckbox.checked) {
                this.addMessage(`[DEBUG] 刷新缓冲区 (${this.dataBuffer.length}字节)`, 'info');
            }

            // 根据配置决定是否处理ANSI序列
            let processedData = this.processAnsiSequencesCheckbox.checked ?
                this.processAnsiSequences(this.dataBuffer) :
                this.dataBuffer;

            // 显示处理后的数据，使用缓冲区开始时间作为时间戳
            this.addMessage(processedData, 'received', this.bufferStartTime);

            // 清空缓冲区和时间戳
            this.dataBuffer = '';
            this.bufferStartTime = null;
        }

        // 清除超时
        if (this.lineTimeoutId) {
            clearTimeout(this.lineTimeoutId);
            this.lineTimeoutId = null;
        }
    }

    processAnsiSequences(text) {
        // 处理ANSI转义序列，使其在HTML中正确显示
        let processed = text;

        // 首先处理特殊的控制序列
        // 处理 [2K (清除整行)
        processed = processed.replace(/\x1b\[2K/g, '');

        // 处理光标移动序列
        processed = processed.replace(/\x1b\[[0-9]+;[0-9]+H/g, '');
        processed = processed.replace(/\x1b\[[0-9]+;[0-9]+f/g, '');

        // 处理回车符覆盖行为（这是关键！）
        // 当遇到 \r 但没有 \n 时，表示要覆盖当前行
        processed = this.processCarriageReturn(processed);

        // 处理ANSI颜色和格式代码
        processed = this.processAnsiColors(processed);

        // 移除其他未处理的ANSI转义序列
        processed = processed.replace(/\x1b\[[0-9;]*[a-zA-Z]/g, '');

        // 移除一些控制字符，但保留换行符和制表符
        processed = processed.replace(/[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]/g, '');

        // 清理多余的空行（连续的换行符）
        processed = processed.replace(/\n{3,}/g, '\n\n');

        return processed;
    }

    processCarriageReturn(text) {
        // 处理回车符的覆盖行为
        let processed = text;

        // 检查是否包含需要处理的覆盖模式
        if (processed.includes('\r') && !processed.includes('\r\n')) {
            // 只处理特定的shell提示符覆盖模式

            // 模式1: 调试信息被提示符覆盖 (如: "2025-05-30 11:21:01 DEBUG: MQTT...\rsh:/$ ")
            processed = processed.replace(/(.*DEBUG[^\r]*)\r(sh:\/\$\s*)/g, (match, content, prompt) => {
                // 如果调试信息很长，保留它并添加换行，然后显示提示符
                if (content.length > 10) {
                    return content + '\n' + prompt;
                } else {
                    return prompt; // 短内容直接被覆盖
                }
            });

            // 模式2: 其他内容被提示符覆盖，但保留有意义的内容
            processed = processed.replace(/([^\r\n]{10,})\r(sh:\/\$\s*|>>>\s*|>\s*)$/gm, '$1\n$2');

            // 模式3: 短内容被提示符覆盖（真正的覆盖行为）
            processed = processed.replace(/([^\r\n]{1,9})\r(sh:\/\$\s*|>>>\s*|>\s*)$/gm, '$2');
        }

        return processed;
    }

    processAnsiColors(text) {
        let processed = text;

        // 定义ANSI颜色映射（使用终端友好的颜色）
        const ansiColors = {
            // 重置
            '0': { action: 'reset' },

            // 文本样式
            '1': { style: 'font-weight: bold;' },           // 粗体
            '2': { style: 'opacity: 0.6;' },               // 暗淡
            '3': { style: 'font-style: italic;' },         // 斜体
            '4': { style: 'text-decoration: underline;' }, // 下划线
            '5': { style: 'animation: blink 1s infinite;' }, // 闪烁
            '7': { style: 'filter: invert(1);' },          // 反色
            '9': { style: 'text-decoration: line-through;' }, // 删除线

            // 前景色（标准颜色）
            '30': { color: '#2e3436' },  // 黑色
            '31': { color: '#cc0000' },  // 红色
            '32': { color: '#4e9a06' },  // 绿色
            '33': { color: '#c4a000' },  // 黄色
            '34': { color: '#3465a4' },  // 蓝色
            '35': { color: '#75507b' },  // 洋红
            '36': { color: '#06989a' },  // 青色
            '37': { color: '#d3d7cf' },  // 白色

            // 前景色（高亮颜色）
            '90': { color: '#555753' },  // 亮黑色（灰色）
            '91': { color: '#ef2929' },  // 亮红色
            '92': { color: '#8ae234' },  // 亮绿色
            '93': { color: '#fce94f' },  // 亮黄色
            '94': { color: '#729fcf' },  // 亮蓝色
            '95': { color: '#ad7fa8' },  // 亮洋红
            '96': { color: '#34e2e2' },  // 亮青色
            '97': { color: '#eeeeec' },  // 亮白色

            // 背景色（标准颜色）
            '40': { backgroundColor: '#2e3436' },  // 黑色背景
            '41': { backgroundColor: '#cc0000' },  // 红色背景
            '42': { backgroundColor: '#4e9a06' },  // 绿色背景
            '43': { backgroundColor: '#c4a000' },  // 黄色背景
            '44': { backgroundColor: '#3465a4' },  // 蓝色背景
            '45': { backgroundColor: '#75507b' },  // 洋红背景
            '46': { backgroundColor: '#06989a' },  // 青色背景
            '47': { backgroundColor: '#d3d7cf' },  // 白色背景

            // 背景色（高亮颜色）
            '100': { backgroundColor: '#555753' },  // 亮黑色背景
            '101': { backgroundColor: '#ef2929' },  // 亮红色背景
            '102': { backgroundColor: '#8ae234' },  // 亮绿色背景
            '103': { backgroundColor: '#fce94f' },  // 亮黄色背景
            '104': { backgroundColor: '#729fcf' },  // 亮蓝色背景
            '105': { backgroundColor: '#ad7fa8' },  // 亮洋红背景
            '106': { backgroundColor: '#34e2e2' },  // 亮青色背景
            '107': { backgroundColor: '#eeeeec' },  // 亮白色背景
        };

        // 处理ANSI转义序列
        processed = processed.replace(/\x1b\[([0-9;]*)m/g, (match, codes) => {
            if (!codes) codes = '0'; // 空代码默认为重置

            const codeList = codes.split(';');
            let styles = [];
            let hasReset = false;
            let i = 0;

            while (i < codeList.length) {
                const code = codeList[i];

                // 处理256色和RGB颜色
                if (code === '38' || code === '48') { // 前景色或背景色
                    const isBackground = code === '48';
                    i++;

                    if (i < codeList.length && codeList[i] === '5') {
                        // 256色模式: ESC[38;5;n m 或 ESC[48;5;n m
                        i++;
                        if (i < codeList.length) {
                            const colorIndex = parseInt(codeList[i]);
                            const color = this.get256Color(colorIndex);
                            if (isBackground) {
                                styles.push(`background-color: ${color}`);
                            } else {
                                styles.push(`color: ${color}`);
                            }
                        }
                    } else if (i < codeList.length && codeList[i] === '2') {
                        // RGB模式: ESC[38;2;r;g;b m 或 ESC[48;2;r;g;b m
                        i++;
                        if (i + 2 < codeList.length) {
                            const r = parseInt(codeList[i]);
                            const g = parseInt(codeList[i + 1]);
                            const b = parseInt(codeList[i + 2]);
                            const color = `rgb(${r}, ${g}, ${b})`;
                            if (isBackground) {
                                styles.push(`background-color: ${color}`);
                            } else {
                                styles.push(`color: ${color}`);
                            }
                            i += 2;
                        }
                    }
                } else {
                    // 处理标准ANSI代码
                    const ansiCode = ansiColors[code];
                    if (ansiCode) {
                        if (ansiCode.action === 'reset') {
                            hasReset = true;
                            break;
                        } else {
                            if (ansiCode.color) styles.push(`color: ${ansiCode.color}`);
                            if (ansiCode.backgroundColor) styles.push(`background-color: ${ansiCode.backgroundColor}`);
                            if (ansiCode.style) styles.push(ansiCode.style);
                        }
                    }
                }
                i++;
            }

            if (hasReset) {
                return '</span>';
            } else if (styles.length > 0) {
                return `<span style="${styles.join('; ')};">`;
            } else {
                return ''; // 未知代码，移除
            }
        });

        return processed;
    }

    get256Color(index) {
        // 256色调色板
        if (index < 16) {
            // 标准16色
            const colors = [
                '#000000', '#800000', '#008000', '#808000', '#000080', '#800080', '#008080', '#c0c0c0',
                '#808080', '#ff0000', '#00ff00', '#ffff00', '#0000ff', '#ff00ff', '#00ffff', '#ffffff'
            ];
            return colors[index];
        } else if (index < 232) {
            // 216色立方体 (6x6x6)
            const i = index - 16;
            const r = Math.floor(i / 36);
            const g = Math.floor((i % 36) / 6);
            const b = i % 6;

            const toHex = (n) => {
                const values = [0, 95, 135, 175, 215, 255];
                return values[n].toString(16).padStart(2, '0');
            };

            return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
        } else {
            // 24级灰度
            const gray = 8 + (index - 232) * 10;
            const hex = gray.toString(16).padStart(2, '0');
            return `#${hex}${hex}${hex}`;
        }
    }

    escapeRegExp(string) {
        return string.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    }



    // MicroLink API 方法
    async startRTT() {
        const addr = this.rttAddrInput.value;
        const size = this.rttSizeInput.value;
        const channel = this.rttChannelInput.value;
        const command = `RTTView.start(${addr}, ${size}, ${channel})`;
        await this.sendCommand(command);
    }

    async stopRTT() {
        const command = 'RTTView.stop()';
        await this.sendCommand(command);
    }

    async startSystemView() {
        const addr = this.svAddrInput.value;
        const size = this.svSizeInput.value;
        const channel = this.svChannelInput.value;
        const command = `SystemView.start(${addr}, ${size}, ${channel})`;
        await this.sendCommand(command);
    }

    async loadFLM() {
        const flmPath = this.flmPathInput.value;
        const baseAddr = this.baseAddrInput.value;
        const ramAddr = this.ramAddrInput.value;
        const command = `ReadFlm.load("${flmPath}", ${baseAddr}, ${ramAddr})`;
        await this.sendCommand(command);
    }

    async loadBin() {
        const binPath = this.binPathInput.value;
        const binAddr = this.binAddrInput.value;
        const command = `load.bin("${binPath}", ${binAddr})`;
        await this.sendCommand(command);
    }

    async offlineDownload() {
        const command = 'load.offline()';
        await this.sendCommand(command);
    }

    async ymodemSend() {
        const filePath = this.ymodemFileInput.value;
        const command = `ym.send("${filePath}")`;
        await this.sendCommand(command);
    }

    async sendCustomCommand() {
        const command = this.customCommandInput.value.trim();
        if (command) {
            if (this.hexModeCheckbox.checked) {
                // HEX模式：发送十六进制数据
                await this.sendHexData(command);
            } else {
                // 文本模式：发送普通命令
                await this.sendCommand(command);
            }
        }
    }

    async sendCommand(command) {
        if (!this.isConnected) {
            this.addMessage('请先连接串口', 'error');
            return;
        }

        try {
            const dataToSend = new TextEncoder().encode(command + '\r\n');

            if (!this.writer) {
                this.writer = this.port.writable.getWriter();
            }

            await this.writer.write(dataToSend);
            this.addMessage(`命令: ${command}`, 'sent');

        } catch (error) {
            this.addMessage(`发送命令失败: ${error.message}`, 'error');
        }
    }

    addCustomCommand() {
        const command = this.customCommandInput.value.trim();
        if (command && !this.customCommands.includes(command)) {
            this.customCommands.push(command);
            this.updateCustomCommandsList();
            this.customCommandInput.value = '';
        }
    }

    updateCustomCommandsList() {
        this.customCommandsList.innerHTML = '';
        this.customCommands.forEach((command, index) => {
            const item = document.createElement('div');
            item.className = 'custom-command-item';
            item.textContent = command;
            item.addEventListener('click', () => {
                this.customCommandInput.value = command;
            });
            this.customCommandsList.appendChild(item);
        });
    }

    addMessage(text, type = 'info', customTimestamp = null) {
        const message = document.createElement('div');
        message.className = `message message-${type}`;

        // 使用自定义时间戳或当前时间
        const messageTime = customTimestamp || new Date();

        let content = '';
        if (this.showTimestampCheckbox.checked) {
            const hours = messageTime.getHours().toString().padStart(2, '0');
            const minutes = messageTime.getMinutes().toString().padStart(2, '0');
            const seconds = messageTime.getSeconds().toString().padStart(2, '0');
            const milliseconds = messageTime.getMilliseconds().toString().padStart(3, '0');
            const timestamp = `${hours}:${minutes}:${seconds}.<span class="milliseconds">${milliseconds}</span>`;
            content += `<span class="timestamp">[${timestamp}]</span>`;
        }

        content += text;
        message.innerHTML = content;

        // 保存到日志记录中，使用消息的实际时间戳
        this.logMessages.push({
            timestamp: messageTime,
            type: type,
            content: text
        });

        this.terminalOutput.appendChild(message);

        if (this.autoScrollCheckbox.checked) {
            this.scrollToBottom();
        }
    }

    scrollToBottom() {
        this.terminalOutput.scrollTop = this.terminalOutput.scrollHeight;
    }

    updateTerminalPlaceholder() {
        if (this.isConnected) {
            if (this.hexModeCheckbox.checked) {
                this.terminalInputField.placeholder = 'HEX模式：输入十六进制数据（如：41 42 43 或 414243）后按Enter发送...';
            } else {
                this.terminalInputField.placeholder = '输入命令后点击发送或按Enter键发送...';
            }
        } else {
            this.terminalInputField.placeholder = '请先连接串口...';
        }
    }

    updateSendOptions() {
        const isHexMode = this.hexModeCheckbox.checked;

        if (isHexMode) {
            // HEX模式下：保存当前设置并禁用虚拟终端模式
            this.virtualTerminalModeOriginalValue = this.virtualTerminalModeCheckbox.checked;
            this.virtualTerminalModeCheckbox.disabled = true;
            this.virtualTerminalMode = false; // 功能上禁用，但保持复选框原始状态

            // 添加视觉提示
            const label = this.virtualTerminalModeCheckbox.parentElement;
            label.style.opacity = '0.5';
            label.title = 'HEX模式下虚拟终端功能不可用';
        } else {
            // 文本模式下：恢复虚拟终端模式
            this.virtualTerminalModeCheckbox.disabled = false;
            this.virtualTerminalModeCheckbox.checked = this.virtualTerminalModeOriginalValue;
            this.virtualTerminalMode = this.virtualTerminalModeOriginalValue;

            // 移除视觉提示
            const label = this.virtualTerminalModeCheckbox.parentElement;
            label.style.opacity = '1';
            label.title = '';
        }

        // 更新终端占位符
        this.updateTerminalPlaceholder();
    }



    clearTerminal() {
        if (confirm('确定要清空终端内容吗？')) {
            this.terminalOutput.innerHTML = '';
            this.logMessages = [];
            this.addMessage('终端已清空', 'info');
        }
    }

    updateConnectionStatus(connected) {
        if (connected) {
            this.connectionStatus.className = 'status-connected';
            this.connectionStatus.innerHTML = '<i class="fas fa-circle"></i> 已连接';
            this.connectBtn.disabled = true;
            this.disconnectBtn.disabled = false;
            this.terminalInputField.disabled = false;
            if (this.terminalSendBtn) this.terminalSendBtn.disabled = false;
            this.updateTerminalPlaceholder();
        } else {
            this.connectionStatus.className = 'status-disconnected';
            this.connectionStatus.innerHTML = '<i class="fas fa-circle"></i> 未连接';
            this.connectBtn.disabled = false;
            this.disconnectBtn.disabled = true;
            this.terminalInputField.disabled = true;
            if (this.terminalSendBtn) this.terminalSendBtn.disabled = true;
            this.updateTerminalPlaceholder();
        }
    }

    // 参数管理方法
    saveParameters() {
        const params = {
            baudRate: this.baudRateSelect.value,
            customBaudRate: this.customBaudRateInput.value,
            dataBits: this.dataBitsSelect.value,
            parity: this.paritySelect.value,
            stopBits: this.stopBitsSelect.value,
            rttAddr: this.rttAddrInput.value,
            rttSize: this.rttSizeInput.value,
            rttChannel: this.rttChannelInput.value,
            svAddr: this.svAddrInput.value,
            svSize: this.svSizeInput.value,
            svChannel: this.svChannelInput.value,
            flmPath: this.flmPathInput.value,
            baseAddr: this.baseAddrInput.value,
            ramAddr: this.ramAddrInput.value,
            binPath: this.binPathInput.value,
            binAddr: this.binAddrInput.value,
            ymodemFile: this.ymodemFileInput.value,
            customCommands: this.customCommands,
            virtualTerminalMode: this.virtualTerminalModeOriginalValue,
            processAnsiSequences: this.processAnsiSequencesCheckbox.checked
        };

        localStorage.setItem('microlinkParams', JSON.stringify(params));
        this.addMessage('✅ 参数已保存到本地存储', 'info');
    }

    async saveParametersToFile() {
        try {
            // 收集当前参数
            const params = this.getCurrentParameters();

            // 生成配置文件内容
            const configContent = this.generateConfigFileContent(params);

            // 检查是否支持File System Access API
            if ('showSaveFilePicker' in window) {
                await this.saveWithFilePicker(configContent);
            } else {
                // 降级到传统下载方式
                this.saveWithDownload(configContent);
            }

        } catch (error) {
            if (error.name === 'AbortError') {
                this.addMessage('💡 用户取消了文件保存', 'info');
            } else {
                this.addMessage(`❌ 保存参数到文件失败: ${error.message}`, 'error');
            }
        }
    }

    async saveWithFilePicker(configContent) {
        try {
            // 使用File System Access API让用户选择保存位置
            const fileHandle = await window.showSaveFilePicker({
                suggestedName: 'microlink_web_scp.txt',
                types: [
                    {
                        description: 'MicroLink配置文件',
                        accept: {
                            'text/plain': ['.txt'],
                        },
                    },
                ],
            });

            // 创建可写流并写入内容
            const writable = await fileHandle.createWritable();
            await writable.write(configContent);
            await writable.close();

            this.addMessage('✅ 参数已保存到指定位置', 'info');

        } catch (error) {
            if (error.name === 'AbortError') {
                throw error; // 重新抛出取消错误
            } else {
                // 如果File System Access API失败，降级到下载方式
                this.addMessage('⚠️ 文件选择器不可用，使用下载方式保存', 'info');
                this.saveWithDownload(configContent);
            }
        }
    }

    saveWithDownload(configContent) {
        // 传统的下载方式
        const blob = new Blob([configContent], { type: 'text/plain;charset=utf-8' });
        const url = URL.createObjectURL(blob);

        // 创建下载链接并触发下载
        const a = document.createElement('a');
        a.href = url;
        a.download = 'microlink_web_scp.txt';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);

        // 清理URL对象
        URL.revokeObjectURL(url);

        this.addMessage('✅ 参数已保存到下载文件夹 microlink_web_scp.txt', 'info');
    }

    getCurrentParameters() {
        return {
            baudRate: this.baudRateSelect.value,
            customBaudRate: this.customBaudRateInput.value,
            dataBits: this.dataBitsSelect.value,
            parity: this.paritySelect.value,
            stopBits: this.stopBitsSelect.value,
            rttAddr: this.rttAddrInput.value,
            rttSize: this.rttSizeInput.value,
            rttChannel: this.rttChannelInput.value,
            svAddr: this.svAddrInput.value,
            svSize: this.svSizeInput.value,
            svChannel: this.svChannelInput.value,
            flmPath: this.flmPathInput.value,
            baseAddr: this.baseAddrInput.value,
            ramAddr: this.ramAddrInput.value,
            binPath: this.binPathInput.value,
            binAddr: this.binAddrInput.value,
            ymodemFile: this.ymodemFileInput.value,
            customCommands: this.customCommands,
            virtualTerminalMode: this.virtualTerminalModeOriginalValue,
            processAnsiSequences: this.processAnsiSequencesCheckbox.checked
        };
    }

    generateConfigFileContent(params) {
        // 生成配置文件格式的内容
        const lines = [
            '# MicroLink Web Serial Configuration Parameters',
            '# 串口配置',
            'port=COM3',
            `baudrate=${params.baudRate}`,
            `databits=${params.dataBits}`,
            `parity=${params.parity === 'none' ? 'N' : params.parity === 'even' ? 'E' : 'O'}`,
            `stopbits=${params.stopBits}`,
            '',
            '# RTT配置',
            `rtt_addr=${params.rttAddr}`,
            `rtt_size=${params.rttSize}`,
            `rtt_channel=${params.rttChannel}`,
            '',
            '# SystemView配置',
            `systemview_addr=${params.svAddr}`,
            `systemview_size=${params.svSize}`,
            `systemview_channel=${params.svChannel}`,
            '',
            '# FLM配置',
            `flm_path=${params.flmPath}`,
            `base_addr=${params.baseAddr}`,
            `ram_addr=${params.ramAddr}`,
            '',
            '# 下载配置',
            `bin_file_path=${params.binPath}`,
            `bin_addr=${params.binAddr}`,
            '',
            '# 自定义命令',
            `custom_commands=${params.customCommands.join(';')}`,
            ''
        ];

        return lines.join('\n');
    }

    async loadParameters() {
        // 直接从HTML配置加载
        this.loadConfigFromHTML();

        // 然后检查是否有用户保存的参数覆盖
        const saved = localStorage.getItem('microlinkParams');
        if (saved) {
            try {
                const params = JSON.parse(saved);
                this.applyParameters(params);
                this.addMessage('✅ 用户保存的参数已加载并覆盖HTML配置', 'info');
            } catch (error) {
                this.addMessage('❌ 用户参数格式错误，使用HTML配置参数', 'error');
            }
        }
    }



    async loadConfigFile() {
        // 手动重新加载HTML配置（按钮触发）
        this.addMessage('🔄 手动重新加载HTML配置...', 'info');
        
        try {
            const configElement = document.getElementById('embedded-config');
            if (configElement) {
                const configText = configElement.textContent;
                const params = this.parseConfigFile(configText);
                this.applyParameters(params);
                this.addMessage('✅ 参数已从HTML配置重新加载', 'info');
                return;
            } else {
                this.addMessage('❌ HTML配置元素未找到', 'error');
            }
        } catch (error) {
            this.addMessage(`从HTML配置加载失败: ${error.message}`, 'error');
        }

        // 如果HTML配置失败，使用备用配置
        this.addMessage('⚠️ HTML配置加载失败，使用备用配置', 'info');
        this.loadEmbeddedConfig();
    }

    loadConfigFromHTML() {
        try {
            // 从HTML中的script标签读取配置
            const configElement = document.getElementById('embedded-config');
            if (configElement) {
                const configText = configElement.textContent;
                const params = this.parseConfigFile(configText);
                this.applyParameters(params);
                this.addMessage('✅ 参数已从HTML配置加载', 'info');
            } else {
                // 如果HTML中没有配置，使用硬编码的备用配置
                this.loadEmbeddedConfig();
            }
        } catch (error) {
            this.addMessage(`从HTML配置加载失败: ${error.message}`, 'error');
            this.loadEmbeddedConfig();
        }
    }

    loadEmbeddedConfig() {
        // 备用的硬编码配置
        const embeddedConfig = `# MicroLink Web Serial Configuration Parameters
# 串口配置
port=COM3
baudrate=115200
databits=8
parity=N
stopbits=1

# RTT配置
rtt_addr=0x20000000
rtt_size=0x4000
rtt_channel=0

# SystemView配置
systemview_addr=0x20000000
systemview_size=0x4000
systemview_channel=1

# FLM配置
flm_path=STM32/STM32F4xx_1024.FLM.o
base_addr=0x08000000
ram_addr=0x20000000

# 下载配置
bin_file_path=firmware.bin
bin_addr=0x08000000

# 自定义命令
custom_commands=RTTView.start(0x20000000,1024,0);SystemView.start(0x20000000,1024,1);load.offline()`;

        const params = this.parseConfigFile(embeddedConfig);
        this.applyParameters(params);
        this.addMessage('参数已从备用配置加载', 'info');
    }

    handleConfigFileSelect(event) {
        const file = event.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = (e) => {
                try {
                    const configText = e.target.result;
                    const params = this.parseConfigFile(configText);
                    this.applyParameters(params);
                    this.addMessage(`参数已从文件 "${file.name}" 加载`, 'info');
                } catch (error) {
                    this.addMessage(`解析配置文件失败: ${error.message}`, 'error');
                }
            };
            reader.readAsText(file);
        }
    }

    parseConfigFile(configText) {
        const params = {};
        const lines = configText.split('\n');

        for (const line of lines) {
            const trimmedLine = line.trim();
            if (trimmedLine && !trimmedLine.startsWith('#')) {
                const [key, value] = trimmedLine.split('=');
                if (key && value) {
                    const trimmedKey = key.trim();
                    const trimmedValue = value.trim();

                    // 映射配置文件的键到内部参数名
                    switch (trimmedKey) {
                        case 'baudrate':
                            params.baudRate = trimmedValue;
                            break;
                        case 'databits':
                            params.dataBits = trimmedValue;
                            break;
                        case 'parity':
                            params.parity = trimmedValue.toLowerCase() === 'n' ? 'none' :
                                          trimmedValue.toLowerCase() === 'e' ? 'even' : 'odd';
                            break;
                        case 'stopbits':
                            params.stopBits = trimmedValue;
                            break;
                        case 'rtt_addr':
                            params.rttAddr = trimmedValue;
                            break;
                        case 'rtt_size':
                            params.rttSize = trimmedValue;
                            break;
                        case 'rtt_channel':
                            params.rttChannel = trimmedValue;
                            break;
                        case 'systemview_addr':
                            params.svAddr = trimmedValue;
                            break;
                        case 'systemview_size':
                            params.svSize = trimmedValue;
                            break;
                        case 'systemview_channel':
                            params.svChannel = trimmedValue;
                            break;
                        case 'flm_path':
                            params.flmPath = trimmedValue;
                            break;
                        case 'base_addr':
                            params.baseAddr = trimmedValue;
                            break;
                        case 'ram_addr':
                            params.ramAddr = trimmedValue;
                            break;
                        case 'bin_file_path':
                            params.binPath = trimmedValue;
                            break;
                        case 'bin_addr':
                            params.binAddr = trimmedValue;
                            break;
                        case 'custom_commands':
                            params.customCommands = trimmedValue.split(';').filter(cmd => cmd.trim());
                            break;
                    }
                }
            }
        }

        return params;
    }

    resetParameters() {
        // 重置为默认值（与HTML配置保持一致）
        const defaultParams = {
            baudRate: '115200',
            customBaudRate: '',
            dataBits: '8',
            parity: 'none',
            stopBits: '1',
            rttAddr: '0x20000000',
            rttSize: '0x4000',
            rttChannel: '0',
            svAddr: '0x20000000',
            svSize: '0x4000',
            svChannel: '1',
            flmPath: 'STM32/STM32F4xx_1024.FLM.o',
            baseAddr: '0x08000000',
            ramAddr: '0x20000000',
            binPath: 'firmware.bin',
            binAddr: '0x08000000',
            ymodemFile: 'update.bin',
            customCommands: [],
            virtualTerminalMode: true,
            processAnsiSequences: true
        };

        this.applyParameters(defaultParams);
        this.addMessage('参数已重置为默认值', 'info');
    }

    applyParameters(params) {
        this.baudRateSelect.value = params.baudRate || '115200';
        this.customBaudRateInput.value = params.customBaudRate || '';
        this.dataBitsSelect.value = params.dataBits || '8';
        this.paritySelect.value = params.parity || 'none';
        this.stopBitsSelect.value = params.stopBits || '1';

        this.rttAddrInput.value = params.rttAddr || '0x20000000';
        this.rttSizeInput.value = params.rttSize || '0x4000';
        this.rttChannelInput.value = params.rttChannel || '0';

        this.svAddrInput.value = params.svAddr || '0x20000000';
        this.svSizeInput.value = params.svSize || '0x4000';
        this.svChannelInput.value = params.svChannel || '1';

        this.flmPathInput.value = params.flmPath || 'STM32/STM32F4xx_1024.FLM.o';
        this.baseAddrInput.value = params.baseAddr || '0x08000000';
        this.ramAddrInput.value = params.ramAddr || '0x20000000';

        this.binPathInput.value = params.binPath || 'firmware.bin';
        this.binAddrInput.value = params.binAddr || '0x08000000';

        this.ymodemFileInput.value = params.ymodemFile || 'update.bin';

        this.customCommands = params.customCommands || [];
        this.updateCustomCommandsList();

        // 应用虚拟终端配置
        if (params.virtualTerminalMode !== undefined) {
            this.virtualTerminalModeCheckbox.checked = params.virtualTerminalMode;
            this.virtualTerminalMode = params.virtualTerminalMode;
            this.virtualTerminalModeOriginalValue = params.virtualTerminalMode;
        }
        if (params.processAnsiSequences !== undefined) {
            this.processAnsiSequencesCheckbox.checked = params.processAnsiSequences;
        }

        // 处理自定义波特率显示
        if (this.baudRateSelect.value === 'custom') {
            this.customBaudRateInput.style.display = 'block';
        } else {
            this.customBaudRateInput.style.display = 'none';
        }
    }

    // 终端输入处理方法
    handleTerminalInput(event) {
        if (!this.isConnected) {
            return; // 未连接时不处理按键
        }

        // 现在统一使用传统模式，不再实时发送
        this.handleLocalTerminalKey(event);
    }

    // 虚拟终端按键处理
    async handleVirtualTerminalKey(event) {
        event.preventDefault();

        let keySequence = null;
        let shouldClearInput = false;

        switch (event.key) {
            case 'Enter':
                keySequence = '\r\n';
                this.currentLine = '';
                shouldClearInput = true;
                break;
            case 'Tab':
                keySequence = '\t';
                shouldClearInput = true;
                break;
            case 'ArrowUp':
                keySequence = '\x1b[A'; // ANSI escape sequence for up arrow
                shouldClearInput = true;
                break;
            case 'ArrowDown':
                keySequence = '\x1b[B'; // ANSI escape sequence for down arrow
                shouldClearInput = true;
                break;
            case 'ArrowLeft':
                keySequence = '\x1b[D'; // ANSI escape sequence for left arrow
                shouldClearInput = true;
                break;
            case 'ArrowRight':
                keySequence = '\x1b[C'; // ANSI escape sequence for right arrow
                shouldClearInput = true;
                break;
            case 'Backspace':
                keySequence = '\x08'; // Backspace character
                if (this.currentLine.length > 0) {
                    this.currentLine = this.currentLine.slice(0, -1);
                }
                shouldClearInput = true;
                break;
            case 'Delete':
                keySequence = '\x7f'; // DEL character
                shouldClearInput = true;
                break;
            case 'Home':
                keySequence = '\x1b[H';
                shouldClearInput = true;
                break;
            case 'End':
                keySequence = '\x1b[F';
                shouldClearInput = true;
                break;
            case 'PageUp':
                keySequence = '\x1b[5~';
                shouldClearInput = true;
                break;
            case 'PageDown':
                keySequence = '\x1b[6~';
                shouldClearInput = true;
                break;
            case 'Escape':
                keySequence = '\x1b';
                shouldClearInput = true;
                break;
            default:
                // 普通字符
                if (event.key.length === 1 && !event.ctrlKey && !event.altKey && !event.metaKey) {
                    keySequence = event.key;
                    this.currentLine += event.key;
                    shouldClearInput = true;
                }
                // Ctrl组合键
                else if (event.ctrlKey && event.key.length === 1) {
                    const char = event.key.toLowerCase();
                    if (char >= 'a' && char <= 'z') {
                        keySequence = String.fromCharCode(char.charCodeAt(0) - 96); // Ctrl+A = 0x01, etc.
                        shouldClearInput = true;
                    }
                }
                break;
        }

        if (keySequence) {
            // 发送按键序列到串口
            await this.sendRawData(keySequence);

            // 在虚拟终端模式下，清除输入框内容，让远程设备控制显示
            if (shouldClearInput) {
                this.terminalInputField.value = '';
                this.currentLine = '';
            }
        }
    }

    // 传统终端按键处理
    handleLocalTerminalKey(event) {
        switch (event.key) {
            case 'Enter':
                event.preventDefault();
                this.executeTerminalCommand();
                break;
            case 'ArrowUp':
                event.preventDefault();
                this.navigateHistory(-1);
                break;
            case 'ArrowDown':
                event.preventDefault();
                this.navigateHistory(1);
                break;
            case 'Tab':
                event.preventDefault();
                // Tab键插入Tab字符
                this.insertTabCharacter();
                break;
        }
    }

    // 发送原始数据到串口
    async sendRawData(data) {
        if (!this.isConnected || !this.port) {
            return;
        }

        try {
            const dataToSend = new TextEncoder().encode(data);

            if (!this.writer) {
                this.writer = this.port.writable.getWriter();
            }

            await this.writer.write(dataToSend);

        } catch (error) {
            this.addMessage(`发送数据失败: ${error.message}`, 'error');
        }
    }

    // 发送HEX格式数据
    async sendHexData(hexString) {
        if (!this.isConnected) {
            this.addMessage('请先连接串口', 'error');
            return;
        }

        try {
            // 清理输入：移除空格、换行符等
            const cleanHex = hexString.replace(/[\s\r\n]/g, '');

            // 验证是否为有效的十六进制字符串
            if (!/^[0-9A-Fa-f]*$/.test(cleanHex)) {
                this.addMessage('❌ 无效的十六进制数据，只能包含0-9和A-F字符', 'error');
                return;
            }

            // 确保是偶数长度（每个字节需要2个十六进制字符）
            const paddedHex = cleanHex.length % 2 === 0 ? cleanHex : '0' + cleanHex;

            if (paddedHex.length === 0) {
                this.addMessage('❌ 请输入有效的十六进制数据', 'error');
                return;
            }

            // 转换为字节数组
            const bytes = [];
            for (let i = 0; i < paddedHex.length; i += 2) {
                const byteValue = parseInt(paddedHex.substr(i, 2), 16);
                bytes.push(byteValue);
            }

            // 创建Uint8Array并发送
            const dataToSend = new Uint8Array(bytes);

            if (!this.writer) {
                this.writer = this.port.writable.getWriter();
            }

            await this.writer.write(dataToSend);

            // 显示发送的数据（格式化为易读的HEX格式）
            const formattedHex = paddedHex.toUpperCase().replace(/(.{2})/g, '$1 ').trim();
            this.addMessage(`HEX发送 (${bytes.length}字节): ${formattedHex}`, 'sent');

        } catch (error) {
            this.addMessage(`发送HEX数据失败: ${error.message}`, 'error');
        }
    }

    // 插入Tab字符（传统模式使用）
    insertTabCharacter() {
        const input = this.terminalInputField;
        const start = input.selectionStart;
        const end = input.selectionEnd;
        const value = input.value;

        const tabChar = '\t'; // 使用真实Tab字符
        const newValue = value.substring(0, start) + tabChar + value.substring(end);

        input.value = newValue;

        // 将光标移动到Tab字符之后
        const newCursorPos = start + tabChar.length;
        input.setSelectionRange(newCursorPos, newCursorPos);

        // 触发input事件以确保任何监听器都能收到通知
        input.dispatchEvent(new Event('input', { bubbles: true }));
    }

    executeTerminalCommand() {
        const command = this.terminalInputField.value.trim();
        if (!command) return;

        // 添加到命令历史
        if (this.commandHistory[this.commandHistory.length - 1] !== command) {
            this.commandHistory.push(command);
            // 限制历史记录数量
            if (this.commandHistory.length > 100) {
                this.commandHistory.shift();
            }
        }
        this.historyIndex = -1;

        if (this.hexModeCheckbox.checked) {
            // HEX模式：发送十六进制数据
            this.sendHexData(command);
        } else {
            // 文本模式：发送普通命令
            this.addMessage(`$ ${command}`, 'sent');
            this.sendCommand(command);
        }

        // 清空输入框
        this.terminalInputField.value = '';
    }

    navigateHistory(direction) {
        if (this.commandHistory.length === 0) return;

        if (direction === -1) {
            // 向上箭头 - 显示更早的命令
            if (this.historyIndex === -1) {
                this.historyIndex = this.commandHistory.length - 1;
            } else if (this.historyIndex > 0) {
                this.historyIndex--;
            }
        } else if (direction === 1) {
            // 向下箭头 - 显示更新的命令
            if (this.historyIndex === -1) {
                return;
            } else if (this.historyIndex < this.commandHistory.length - 1) {
                this.historyIndex++;
            } else {
                this.historyIndex = -1;
                this.terminalInputField.value = '';
                return;
            }
        }

        if (this.historyIndex >= 0 && this.historyIndex < this.commandHistory.length) {
            this.terminalInputField.value = this.commandHistory[this.historyIndex];
            // 将光标移到末尾
            setTimeout(() => {
                this.terminalInputField.setSelectionRange(
                    this.terminalInputField.value.length,
                    this.terminalInputField.value.length
                );
            }, 0);
        }
    }

    // 日志保存方法
    async saveLog() {
        try {
            if (this.logMessages.length === 0) {
                this.addMessage('没有日志内容可保存', 'info');
                return;
            }

            const logContent = this.generateLogContent();

            // 检查是否支持File System Access API
            if ('showSaveFilePicker' in window) {
                await this.saveLogWithFilePicker(logContent);
            } else {
                // 降级到传统下载方式
                this.saveLogWithDownload(logContent);
            }

        } catch (error) {
            if (error.name === 'AbortError') {
                this.addMessage('💡 用户取消了日志保存', 'info');
            } else {
                this.addMessage(`❌ 保存日志失败: ${error.message}`, 'error');
            }
        }
    }

    generateLogContent() {
        const lines = [];
        lines.push('# MicroLink Web Serial Terminal 日志');
        lines.push(`# 生成时间: ${new Date().toLocaleString()}`);
        lines.push(`# 总计消息数: ${this.logMessages.length}`);
        lines.push('# ========================================');
        lines.push('');

        for (const msg of this.logMessages) {
            const timestamp = msg.timestamp.toLocaleString();
            const typeLabel = this.getTypeLabel(msg.type);
            lines.push(`[${timestamp}] ${typeLabel} ${msg.content}`);
        }

        lines.push('');
        lines.push('# ========================================');
        lines.push('# 日志结束');

        return lines.join('\n');
    }

    getTypeLabel(type) {
        const labels = {
            'sent': '[发送]',
            'received': '[接收]',
            'error': '[错误]',
            'info': '[信息]'
        };
        return labels[type] || '[未知]';
    }

    async saveLogWithFilePicker(logContent) {
        try {
            const now = new Date();
            const dateStr = now.toISOString().slice(0, 19).replace(/[T:]/g, '-');
            const suggestedName = `microlink-log-${dateStr}.txt`;

            const fileHandle = await window.showSaveFilePicker({
                suggestedName: suggestedName,
                types: [
                    {
                        description: 'MicroLink日志文件',
                        accept: {
                            'text/plain': ['.txt'],
                        },
                    },
                ],
            });

            const writable = await fileHandle.createWritable();
            await writable.write(logContent);
            await writable.close();

            this.addMessage('✅ 日志已保存到指定位置', 'info');

        } catch (error) {
            if (error.name === 'AbortError') {
                throw error;
            } else {
                this.addMessage('⚠️ 文件选择器不可用，使用下载方式保存', 'info');
                this.saveLogWithDownload(logContent);
            }
        }
    }

    saveLogWithDownload(logContent) {
        const now = new Date();
        const dateStr = now.toISOString().slice(0, 19).replace(/[T:]/g, '-');
        const filename = `microlink-log-${dateStr}.txt`;

        const blob = new Blob([logContent], { type: 'text/plain;charset=utf-8' });
        const url = URL.createObjectURL(blob);

        const a = document.createElement('a');
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);

        URL.revokeObjectURL(url);

        this.addMessage(`✅ 日志已保存到下载文件夹 ${filename}`, 'info');
    }


}

// ========== YMODEM协议实现（移植自add.html，适配window.microLinkTerminal.port） ==========

// CRC16校验计算 (用于YMODEM)
function calculateCRC16(data) {
    let crc = 0x0000;
    const polynomial = 0x1021; // CRC-16-CCITT
    for (let i = 0; i < data.length; i++) {
        crc ^= (data[i] << 8);
        for (let j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc = crc << 1;
            }
        }
        crc &= 0xFFFF;
    }
    return crc;
}

// 调试函数：验证YMODEM数据包格式
function debugYMODEMPacket(packet, packetType, blockNumber = 0) {
    const packetInfo = {
        type: packetType,
        blockNumber: blockNumber,
        totalLength: packet.length,
        header: {
            SOH: packet[0],
            blockNumber: packet[1],
            blockNumberComplement: packet[2]
        },
        dataArea: Array.from(packet.slice(3, 131)), // 128字节数据区（第4字节到第131字节）
        crc: {
            value: (packet[131] << 8) | packet[132]
        }
    };
    
    console.log(`=== YMODEM ${packetType} 包调试信息 ===`);
    console.log(`包类型: ${packetType}`);
    console.log(`包号: ${blockNumber}`);
    console.log(`总长度: ${packet.length} 字节`);
    console.log(`帧头: SOH=${packet[0].toString(16)}, 包号=${packet[1].toString(16)}, 反码=${packet[2].toString(16)}`);
    console.log(`数据区: 128字节（第4-131字节）`);
    console.log(`数据区前16字节: ${packetInfo.dataArea.slice(0, 16).map(b => b.toString(16).padStart(2, '0')).join(' ')}`);
    console.log(`数据区后16字节: ${packetInfo.dataArea.slice(-16).map(b => b.toString(16).padStart(2, '0')).join(' ')}`);
    console.log(`CRC值: ${packetInfo.crc.value.toString(16).padStart(4, '0')}`);
    
    // 验证协议规范
    const validations = [];
    
    // 验证帧头
    if (packet[0] === 0x01) validations.push('✓ SOH正确');
    else validations.push('✗ SOH错误');
    
    if (packet[1] === blockNumber) validations.push('✓ 包号正确');
    else validations.push('✗ 包号错误');
    
    if (packet[2] === (255 - blockNumber)) validations.push('✓ 包号反码正确');
    else validations.push('✗ 包号反码错误');
    
    // 验证数据区长度
    if (packetInfo.dataArea.length === 128) validations.push('✓ 数据区长度正确(128字节)');
    else validations.push('✗ 数据区长度错误');
    
    // 验证数据区内容
    if (packetType === '起始帧') {
        // 起始帧：文件名 + 0x00 + 文件大小 + 0x00 + 0x00填充
        const hasNullTerminators = packetInfo.dataArea.includes(0x00);
        if (hasNullTerminators) validations.push('✓ 包含NULL终止符');
        else validations.push('✗ 缺少NULL终止符');
    } else if (packetType === '数据帧') {
        // 数据帧：有效数据 + 0x1A填充
        const hasSubPadding = packetInfo.dataArea.slice(-10).some(b => b === 0x1A);
        if (hasSubPadding) validations.push('✓ 包含SUB填充(0x1A)');
        else validations.push('✗ 缺少SUB填充');
    } else if (packetType === '结束帧') {
        // 结束帧：全0x00
        const allZeros = packetInfo.dataArea.every(b => b === 0x00);
        if (allZeros) validations.push('✓ 数据区全为0x00');
        else validations.push('✗ 数据区不全为0x00');
    }
    
    // 验证包长度
    if (packet.length === 133) validations.push('✓ 包长度正确(133字节)');
    else validations.push('✗ 包长度错误');
    
    console.log('协议验证:', validations.join(', '));
    console.log('=====================================');
    
    return packetInfo;
}

// 修改后的数据包创建函数，全部使用CRC校验
function createYMODEMHeaderPacket(fileName, fileSize) {
    const headerSize = 128;
    const packetSize = headerSize + 5; // 3字节帧头 + 128字节数据区 + 2字节CRC
    const header = new Uint8Array(packetSize);
    
    // 严格按照YMODEM协议规范
    header[0] = 0x01;  // SOH - 起始帧固定使用SOH
    header[1] = 0x00;  // 包号固定为0x00
    header[2] = 0xFF;  // 包号反码 0xFF
    
    // 构建文件头信息：文件名 + 0x00 + 文件大小 + 0x00
    const headerInfo = `${fileName}\x00${fileSize}\x00`;
    const headerBytes = new TextEncoder().encode(headerInfo);
    
    // 复制文件头信息到数据区（从第4字节开始）
    header.set(headerBytes, 3);
    
    // 填充剩余字节为0x00（从文件信息结束到128字节数据区结束）
    // 数据区范围：第4字节到第131字节（共128字节）
    for (let i = headerBytes.length + 3; i < headerSize + 3; i++) {
        header[i] = 0x00;
    }
    
    // CRC模式：计算CRC16
    // 数据区：第4字节到第131字节（128字节）
    const dataForCRC = header.slice(3, headerSize + 3);
    const crc = calculateCRC16(dataForCRC);
    header[headerSize + 3] = (crc >> 8) & 0xFF;  // CRC高字节
    header[headerSize + 4] = crc & 0xFF;         // CRC低字节
    
    // 调试输出
    if (typeof console !== 'undefined' && console.log) {
        debugYMODEMPacket(header, '起始帧', 0);
    }
    
    return header;
}

function createYMODEMDataPacket(data, blockNumber) {
    const SOH_BLOCK_SIZE = 128;  // SOH固定128字节
    const packetSize = SOH_BLOCK_SIZE + 5; // 3字节帧头 + 128字节数据区 + 2字节CRC
    const packet = new Uint8Array(packetSize);
    
    // 严格按照YMODEM协议规范
    packet[0] = 0x01;  // SOH - 数据帧使用SOH（128字节块）
    packet[1] = blockNumber;  // 包号
    packet[2] = 255 - blockNumber;  // 包号反码
    
    // 复制有效数据到数据区（从第4字节开始）
    packet.set(data, 3);
    
    // 关键修正：SOH帧数据区严格为128字节，有效数据不足时用0x1A填充
    // 数据区范围：第4字节到第131字节（共128字节）
    for (let i = data.length + 3; i < SOH_BLOCK_SIZE + 3; i++) {
        packet[i] = 0x1A;  // 用0x1A填充剩余字节
    }
    
    // CRC模式：计算CRC16
    // 数据区：第4字节到第131字节（128字节）
    const dataForCRC = packet.slice(3, SOH_BLOCK_SIZE + 3);
    const crc = calculateCRC16(dataForCRC);
    packet[SOH_BLOCK_SIZE + 3] = (crc >> 8) & 0xFF;  // CRC高字节
    packet[SOH_BLOCK_SIZE + 4] = crc & 0xFF;         // CRC低字节
    
    // 调试输出
    if (typeof console !== 'undefined' && console.log) {
        debugYMODEMPacket(packet, '数据帧', blockNumber);
    }
    
    return packet;
}

function createYMODEMEndPacket() {
    const headerSize = 128;
    const packetSize = headerSize + 5; // 3字节帧头 + 128字节数据区 + 2字节CRC
    const header = new Uint8Array(packetSize);
    
    // 严格按照YMODEM协议规范 - 结束帧
    header[0] = 0x01;  // SOH - 结束帧固定使用SOH
    header[1] = 0x00;  // 包号固定为0x00
    header[2] = 0xFF;  // 包号反码 0xFF
    
    // 数据区全部填充0x00（空包）
    // 数据区范围：第4字节到第131字节（共128字节）
    for (let i = 3; i < headerSize + 3; i++) {
        header[i] = 0x00;
    }
    
    // CRC模式：计算CRC16
    // 数据区：第4字节到第131字节（128字节）
    const dataForCRC = header.slice(3, headerSize + 3);
    const crc = calculateCRC16(dataForCRC);
    header[headerSize + 3] = (crc >> 8) & 0xFF;  // CRC高字节
    header[headerSize + 4] = crc & 0xFF;         // CRC低字节
    
    // 调试输出
    if (typeof console !== 'undefined' && console.log) {
        debugYMODEMPacket(header, '结束帧', 0);
    }
    
    return header;
}

async function waitForStartSignal(reader, writer, onLog) {
    const timeout = 30000;
    const startTime = Date.now();
    let lastLogTime = 0;
    let consecutiveChars = 0;
    onLog && onLog('正在等待接收方启动信号...');
    while (Date.now() - startTime < timeout) {
        try {
            const { value, done } = await reader.read();
            if (done) break;
            if (value && value.length > 0) {
                for (let i = 0; i < value.length; i++) {
                    const byte = value[i];
                    if (byte === 0x15) return 'NAK';
                    if (byte === 0x18) throw new Error('接收方取消了传输');
                    if (byte === 0x43) return 'C';
                    if (byte === 0x06) return 'ACK';
                    if (byte === 0x2b) {
                        consecutiveChars++;
                        if (consecutiveChars >= 3) {
                            await writer.write(new Uint8Array([0x15]));
                            return 'PLUS_TRIGGER';
                        }
                    } else if (byte >= 0x20 && byte <= 0x7e) {
                        consecutiveChars++;
                        if (consecutiveChars >= 5) {
                            await writer.write(new Uint8Array([0x15]));
                            return 'CHAR_TRIGGER';
                        }
                    } else {
                        consecutiveChars = 0;
                    }
                }
            }
        } catch (error) {
            onLog && onLog('等待启动信号时出错: ' + error.message);
        }
        const currentTime = Date.now();
        if (currentTime - lastLogTime > 5000) {
            const elapsed = Math.round((currentTime - startTime) / 1000);
            onLog && onLog(`等待中... (${elapsed}s/${timeout/1000}s)`);
            lastLogTime = currentTime;
        }
        await new Promise(resolve => setTimeout(resolve, 100));
    }
    throw new Error('等待启动信号超时，请确保接收方已准备就绪');
}

async function sendYMODEMPacketWithACK(writer, reader, packet, blockNumber, onLog, maxRetries = 10, isHeaderPacket = false) {
    let retries = 0;
    let consecutiveErrors = 0; // 连续错误计数
    
    while (retries < maxRetries) {
        // 详细的包信息日志
        const packetType = blockNumber === 0 ? (packet.slice(3).every(b => b === 0) ? '结束帧' : '起始帧') : '数据帧';
        onLog && onLog(`准备发送${packetType} (包号: ${blockNumber}, 长度: ${packet.length}字节)`);
        
        // 验证包格式
        if (packet[0] !== 0x01) {
            onLog && onLog(`❌ 包格式错误: SOH应为0x01，实际为0x${packet[0].toString(16)}`);
            throw new Error('包格式错误: SOH不正确');
        }
        
        if (packet[1] !== blockNumber) {
            onLog && onLog(`❌ 包格式错误: 包号应为${blockNumber}，实际为${packet[1]}`);
            throw new Error('包格式错误: 包号不正确');
        }
        
        if (packet[2] !== (255 - blockNumber)) {
            onLog && onLog(`❌ 包格式错误: 包号反码应为${255 - blockNumber}，实际为${packet[2]}`);
            throw new Error('包格式错误: 包号反码不正确');
        }
        
        // 验证包长度
        const expectedLength = 133; // CRC模式固定133字节
        if (packet.length !== expectedLength) {
            onLog && onLog(`❌ 包长度错误: 应为${expectedLength}字节，实际为${packet.length}字节`);
            throw new Error('包长度错误');
        }
        
        try {
            await writer.write(packet);
            onLog && onLog(`✅ ${packetType}已发送，等待ACK...`);
            
            // 等待ACK
            await waitForACK(reader, onLog, isHeaderPacket);
            onLog && onLog(`✅ ${packetType}确认成功`);
            consecutiveErrors = 0; // 重置连续错误计数
            return;
            
        } catch (error) {
            consecutiveErrors++;
            
            if (error.message.includes('C字符')) {
                onLog && onLog('检测到C字符，设备端请求重启传输');
                throw new Error('RESTART_HEADER');
            } else if (error.message.includes('CAN') || error.message.includes('取消')) {
                onLog && onLog('检测到传输取消信号');
                throw new Error('TRANSMISSION_CANCELLED');
            } else if (error.message.includes('NAK')) {
                onLog && onLog('收到NAK，数据包校验失败');
                // NAK错误，继续重试
            } else if (error.message.includes('超时')) {
                onLog && onLog('等待ACK超时，设备端可能处理缓慢');
                // 超时错误，可能是设备端处理缓慢
            }
            
            retries++;
            onLog && onLog(`❌ ${packetType}确认失败，重试 ${retries}/${maxRetries} (连续错误: ${consecutiveErrors})`);
            
            if (retries >= maxRetries) {
                throw new Error(`${packetType}发送失败，超过最大重试次数`);
            }
            
            // 根据连续错误次数调整重试延时
            let retryDelay = 1000; // 基础延时1秒
            if (consecutiveErrors >= 3) {
                retryDelay = 3000; // 连续错误较多时，增加延时到3秒
                onLog && onLog(`连续错误较多，增加重试延时到${retryDelay}ms`);
            } else if (consecutiveErrors >= 5) {
                retryDelay = 5000; // 连续错误很多时，增加延时到5秒
                onLog && onLog(`连续错误很多，增加重试延时到${retryDelay}ms`);
            }
            
            onLog && onLog(`等待${retryDelay}ms后重试...`);
            await new Promise(resolve => setTimeout(resolve, retryDelay));
        }
    }
}

async function waitForACK(reader, onLog, isHeaderPacket = false) {
    const timeout = 15000; // 增加超时时间到15秒
    const startTime = Date.now();
    let buffer = '';
    while (Date.now() - startTime < timeout) {
        try {
            const { value, done } = await reader.read();
            if (done) {
                onLog && onLog('waitForACK: 串口流已关闭 (done=true)');
                break;
            }
            if (value && value.length > 0) {
                const text = new TextDecoder().decode(value);
                buffer += text;
                for (let i = 0; i < value.length; i++) {
                    const byte = value[i];
                    if (byte === 0x06) {
                        onLog && onLog('收到ACK确认');
                        return; // 成功收到ACK
                    }
                    if (byte === 0x15) {
                        onLog && onLog('收到NAK，传输失败');
                        throw new Error('收到NAK，传输失败');
                    }
                    if (byte === 0x18) {
                        onLog && onLog('收到CAN，传输被取消');
                        throw new Error('收到CAN，传输被取消');
                    }
                    if (byte === 0x43) {
                        if (isHeaderPacket) {
                            onLog && onLog('头包后收到C，协议正常，进入数据包1发送');
                            return; // 头包后收到C，视为正常
                        } else {
                            onLog && onLog('收到C字符，需要切换到CRC模式或重启传输');
                            throw new Error('收到C字符，需要切换到CRC模式');
                        }
                    }
                }
                if (onLog && value.length < 50) {
                    const hexBytes = Array.from(value).map(b => b.toString(16).padStart(2, '0')).join(' ');
                    onLog(`waitForACK: 收到内容: ${hexBytes}`);
                }
            }
        } catch (error) {
            if (error.message.includes('NAK') || error.message.includes('CAN') || error.message.includes('C字符')) {
                throw error;
            }
            onLog && onLog(`waitForACK读取错误: ${error.message}`);
        }
        await new Promise(resolve => setTimeout(resolve, 100));
    }
    throw new Error('等待ACK超时');
}

async function waitForACKOrNAK(reader, onLog) {
    const timeout = 10000;
    const startTime = Date.now();
    while (Date.now() - startTime < timeout) {
        try {
            const { value, done } = await reader.read();
            if (done) break;
            if (value && value.length > 0) {
                for (let i = 0; i < value.length; i++) {
                    const byte = value[i];
                    if (byte === 0x06) return 'ACK';
                    if (byte === 0x15) return 'NAK';
                    if (byte === 0x18) throw new Error('收到CAN，传输被取消');
                    if (byte === 0x43) throw new Error('收到C字符，需要切换到CRC模式');
                }
            }
        } catch (error) {
            if (error.message.includes('CAN') || error.message.includes('C字符')) {
                throw error;
            }
        }
        await new Promise(resolve => setTimeout(resolve, 50));
    }
    throw new Error('等待ACK或NAK超时');
}

// 发送EOT并等待ACK/NAK/C/CAN，支持重试
async function sendEOTWithACKRetry(writer, reader, config, onLog) {
    const eotTimeout = 1000; // EOT等待ACK最大1秒
    const maxEOTRetries = 5;
    for (let retry = 0; retry < maxEOTRetries; retry++) {
        onLog && onLog(`[DEBUG] 第${retry+1}次发送EOT信号...`);
        await writer.write(new Uint8Array([0x04]));
        onLog && onLog('发送EOT信号，等待ACK...');
        try {
            const resp = await waitForACKorC(reader, onLog, eotTimeout);
            if (resp === 'ACK') {
                onLog && onLog('EOT确认成功');
                return true;
            }
            if (resp === 'C') throw new Error('RESTART_HEADER');
        } catch (e) {
            if (e.message === 'RESTART_HEADER') throw e;
            onLog && onLog(`EOT未确认，重试${retry + 1}/${maxEOTRetries}`);
            await new Promise(r => setTimeout(r, config.retryDelay + retry * 200));
        }
    }
    onLog && onLog('❌ EOT多次重试失败，设备端未响应ACK，传输中止。');
    throw new Error('EOT多次重试失败');
}

// EOT发送函数：发送EOT后等待ACK，支持重试
async function sendEOT(writer, reader, onLog, maxRetries = 5) {
    for (let retry = 0; retry < maxRetries; retry++) {
        onLog && onLog(`[DEBUG] 第${retry+1}次发送EOT信号...`);
        await writer.write(new Uint8Array([0x04]));
        onLog && onLog('发送EOT信号，等待ACK...');
        try {
            const resp = await waitForACKorC(reader, onLog, 1000); // 1秒超时
            if (resp === 'ACK') {
                onLog && onLog('EOT确认成功');
                return true;
            }
            if (resp === 'C') throw new Error('RESTART_HEADER');
        } catch (e) {
            if (e.message === 'RESTART_HEADER') throw e;
            onLog && onLog(`EOT未确认，重试${retry + 1}/${maxRetries}`);
            await new Promise(r => setTimeout(r, 200 + retry * 100));
        }
    }
    onLog && onLog('❌ EOT多次重试失败，设备端未响应ACK，传输中止。');
    throw new Error('EOT多次重试失败');
}

// waitForACKorC函数，支持自定义超时
async function waitForACKorC(reader, onLog, timeout = 1000) {
    const startTime = Date.now();
    while (Date.now() - startTime < timeout) {
        try {
            const { value, done } = await reader.read();
            if (done) break;
            if (value && value.length > 0) {
                for (let i = 0; i < value.length; i++) {
                    const byte = value[i];
                    if (byte === 0x06) return 'ACK';
                    if (byte === 0x15) return 'NAK';
                    if (byte === 0x18) throw new Error('收到CAN，传输被取消');
                    if (byte === 0x43) return 'C';
                }
            }
        } catch (error) {
            if (error.message.includes('CAN') || error.message.includes('C字符')) {
                throw error;
            }
        }
        await new Promise(resolve => setTimeout(resolve, 50));
    }
    throw new Error('等待ACK或C超时');
}

// 主入口：window.ymodemSendFileViaSerial
window.ymodemSendFileViaSerial = async function(uint8Array, fileName, timeout, onProgress, onLog, options = {}) {
    const port = window.microLinkTerminal && window.microLinkTerminal.port;
    if (!port) throw new Error('串口未连接');
    const config = {
        retryDelay: options.retryDelay || 1000,
        maxRetries: options.maxRetries || 20,
        packetTimeout: options.packetTimeout || 15000,
        restartDelay: options.restartDelay || 2000,
        packetInterval: options.packetInterval || 1000,
        ...options
    };
    const SOH_BLOCK_SIZE = 128;
    let writer = null, reader = null;
    let restartCount = 0;
    const maxRestarts = 3;
    // === 头包构造函数优先用options.buildHeaderPacket ===
    const buildHeaderPacket = options.buildHeaderPacket || buildHeaderPacketYmodem;
    try {
        writer = port.writable.getWriter();
        reader = port.readable.getReader();
        onLog && onLog(`准备发送文件: ${fileName}, 大小: ${uint8Array.length} 字节`);
        onLog && onLog(`数据包大小: ${SOH_BLOCK_SIZE} 字节 (SOH)`);
        onLog && onLog(`预计数据包数量: ${Math.ceil(uint8Array.length / SOH_BLOCK_SIZE)}`);
        onLog && onLog(`包间延时: ${config.packetInterval}ms`);
        onLog && onLog(`包超时: ${config.packetTimeout}ms`);
        onLog && onLog(`校验方式: CRC16 (固定)`);
        await new Promise(r => setTimeout(r, 1000));
        await writer.write(new Uint8Array([0x00]));
        await new Promise(r => setTimeout(r, 200));
        await writer.write(new Uint8Array([0x43]));
        await new Promise(r => setTimeout(r, 200));
        onLog && onLog('已发送触发字符序列，等待接收方C信号...');
        while (restartCount <= maxRestarts) {
            try {
                await window.waitForCSignal(reader, onLog);
                await performYMODEMTransfer();
                onLog && onLog('YMODEM传输成功完成！');
                return;
            } catch (error) {
                // === 修正：EOT后收到C时直接发空头包，不再整体重启 ===
                if (error.message === 'RESTART_HEADER' && restartCount < maxRestarts) {
                    onLog && onLog('EOT后收到C，直接发送空头包...');
                    let endPacket = createYMODEMEndPacket();
                    const endAck = await sendEndPacketWithRetry(writer, reader, endPacket, onLog, 10);
                    if (!endAck) throw new Error('空头包多次重试失败，传输中止');
                    return;
                } else if (error.message === 'RESTART_HEADER') {
                    throw new Error('EOT后收到C，结束帧多次失败，传输中止');
                } else if (error.message === 'TRANSMISSION_CANCELLED') {
                    throw new Error('传输被取消');
                } else {
                    throw error;
                }
            }
        }
        throw new Error(`传输失败，已尝试${maxRestarts}次重启`);
        // 内部传输函数
        async function performYMODEMTransfer() {
            onLog && onLog('=== 开始YMODEM传输流程 ===');
            // 1. 发送起始帧
            let headerPacket = buildHeaderPacket(fileName, uint8Array.length);
            await sendPacketWithACKRetry(writer, reader, headerPacket, 0, config, onLog, '起始帧');
            onLog && onLog(`[DEBUG] 头包ACK后，准备延时${config.packetInterval}ms`);
            await new Promise(r => setTimeout(r, config.packetInterval)); // 头包和数据包1之间加延时
            onLog && onLog(`[DEBUG] 延时结束，准备发送数据包1`);
            // 2. 发送数据帧
            const totalBlocks = Math.ceil(uint8Array.length / SOH_BLOCK_SIZE);
            let transferred = 0;
            for (let blockNum = 1; blockNum <= totalBlocks; blockNum++) {
                const startIndex = (blockNum - 1) * SOH_BLOCK_SIZE;
                const endIndex = Math.min(startIndex + SOH_BLOCK_SIZE, uint8Array.length);
                const blockData = uint8Array.slice(startIndex, endIndex);
                // === 强制用buildYmodemPacketYmodem构造数据包 ===
                let dataPacket = buildYmodemPacketYmodem(blockNum, blockData);
                await sendPacketWithACKRetry(writer, reader, dataPacket, blockNum, config, onLog, `数据帧${blockNum}`);
                transferred += blockData.length;
                const progress = Math.round((transferred / uint8Array.length) * 100);
                onProgress && onProgress(progress);
                onLog && onLog(`✅ 数据帧${blockNum}/${totalBlocks} 传输完成 (${progress}%)`);
                onLog && onLog(`[DEBUG] 数据包${blockNum} ACK后，准备延时${config.packetInterval}ms`);
                await new Promise(r => setTimeout(r, config.packetInterval)); // 包间延时
                onLog && onLog(`[DEBUG] 延时结束，准备发送下一个数据包`);
            }
            // 3. 发送EOT，等待ACK
            await sendEOTWithACKRetry(writer, reader, config, onLog);
            await new Promise(r => setTimeout(r, config.packetInterval)); // 包间延时
            // === 关键：EOT和空头包之间增加延时 ===
            await new Promise(r => setTimeout(r, 500));
            // 4. 发送结束帧（空头包）
            let endPacket = createYMODEMEndPacket();
            const endAck = await sendEndPacketWithRetry(writer, reader, endPacket, onLog, 10);
            if (!endAck) throw new Error('空头包多次重试失败，传输中止');
            await new Promise(r => setTimeout(r, config.packetInterval)); // 包间延时
            // === 关键：结束包后严格等待ACK ===
            onLog && onLog('等待设备端ACK确认结束...');
            let gotAck = false;
            const ackStart = Date.now();
            while (Date.now() - ackStart < 5000) { // 最多等5秒
                const { value, done } = await Promise.race([
                    reader.read(),
                    new Promise(resolve => setTimeout(() => resolve({value: null, done: false}), 200))
                ]);
                if (value) {
                    for (let i = 0; i < value.length; i++) {
                        if (value[i] === 0x06) { // ACK
                            gotAck = true;
                            onLog('✅ 设备端ACK确认，YMODEM流程完成');
                            break;
                        }
                    }
                    if (gotAck) break;
                    const text = new TextDecoder().decode(value);
                    onLog('结束后收到内容: ' + text);
                }
                await new Promise(r => setTimeout(r, 100));
            }
            if (!gotAck) onLog('⚠️ 结束后未收到ACK，可能已完成也可能异常');
            onLog && onLog('✅ YMODEM传输流程完成');
        }
    } finally {
        if (writer) try { writer.releaseLock(); } catch (e) {}
        if (reader) try { reader.releaseLock(); } catch (e) {}
    }
};

// 简化的YMODEM发送函数，专门处理设备端问题
window.ymodemSendFileViaSerialSimple = async function(uint8Array, fileName, timeout, onProgress, onLog, options = {}) {
    const port = window.microLinkTerminal && window.microLinkTerminal.port;
    if (!port) throw new Error('串口未连接');
    let writer = null, reader = null;
    // === 1. 先彻底暂停主终端监听 ===
    let wasConnected = false;
    window.isYmodemActive = true; // YMODEM流程期间屏蔽主终端
    if (window.microLinkTerminal) {
        // --- YMODEM前清空主终端缓冲 ---
        if (typeof window.microLinkTerminal.flushBuffer === 'function') {
            window.microLinkTerminal.flushBuffer();
        }
        wasConnected = window.microLinkTerminal.isConnected;
        window.microLinkTerminal.isConnected = false;
        if (window.microLinkTerminal.reader) {
            try { await window.microLinkTerminal.reader.cancel(); } catch (e) {}
            try { window.microLinkTerminal.reader.releaseLock(); } catch (e) {}
            window.microLinkTerminal.reader = null;
        }
        // 等待主终端读取循环彻底退出
        await new Promise(r => setTimeout(r, 300));
    }
    // === 头包构造函数优先用options.buildHeaderPacket ===
    const buildHeaderPacket = options.buildHeaderPacket || buildHeaderPacketYmodem;
    try {
        // === 2. 再获取writer/reader并发送ym.receive()指令 ===
        writer = port.writable.getWriter();
        reader = port.readable.getReader();
        onLog && onLog('发送 ym.receive() 指令...');
        await writer.write(new TextEncoder().encode('ym.receive()\n'));
        // 2. 等待接收端发送 'C'
        onLog && onLog('等待接收端发送 "C"...');
        if (!(await waitForByteYmodem(reader, 0x43, 10000, onLog))) {
            onLog && onLog('未收到接收端 "C"，发送中止');
            throw new Error('未收到接收端 "C"');
        }
        // 3. 发送头包
        onLog && onLog('发送 Ymodem 文件头包...');
        if (!(await sendAndWaitAckYmodem(writer, reader, buildHeaderPacket(fileName, uint8Array.length), onLog))) {
            onLog && onLog('头包发送失败，发送中止');
            throw new Error('头包发送失败');
        }
        // 4. 等待接收端再次发送 'C'
        onLog && onLog('等待接收端再次发送 "C"...');
        if (!(await waitForByteYmodem(reader, 0x43, 10000, onLog))) {
            onLog && onLog('未收到接收端第二个 "C"，发送中止');
            throw new Error('未收到接收端第二个 "C"');
        }
        // 5. 发送数据包
        let seq = 1;
        for (let offset = 0; offset < uint8Array.length; offset += 128) {
            let chunk = uint8Array.slice(offset, offset + 128);
            // === 强制用buildYmodemPacketYmodem构造数据包 ===
            if (!(await sendAndWaitAckYmodem(writer, reader, buildYmodemPacketYmodem(seq, chunk), onLog))) {
                onLog && onLog(`数据包 #${seq} 发送失败，发送中止`);
                throw new Error(`数据包 #${seq} 发送失败`);
            }
            seq++;
            if (onProgress) onProgress(Math.round((offset + chunk.length) / uint8Array.length * 100));
        }
        // 6. 发送EOT，等待ACK
        onLog && onLog('发送 EOT...');
        for (let i = 0; i < 10; i++) {
            await writer.write(new Uint8Array([0x04]));
            let b = await readByteYmodem(reader, 3000, onLog);
            if (b === 0x06) break;
        }
        // 7. 等待接收端发送 'C'
        onLog && onLog('等待接收端发送 "C"...');
        if (!(await waitForByteYmodem(reader, 0x43, 10000, onLog))) {
            onLog && onLog('未收到接收端最后一个 "C"，发送中止');
            throw new Error('未收到接收端最后一个 "C"');
        }
        // 8. 发送空头包
        onLog && onLog('发送空头包...');
        if (!(await sendEndPacketWithRetry(writer, reader, buildEndPacketYmodem(), onLog, 10))) {
            onLog && onLog('空头包发送失败');
            throw new Error('空头包发送失败');
        }
        onLog && onLog('文件发送完成！');
    } finally {
        if (writer) try { writer.releaseLock(); } catch (e) {}
        if (reader) try { reader.releaseLock(); } catch (e) {}
        // === 恢复主终端监听 ===
        if (window.microLinkTerminal) {
            window.microLinkTerminal.isConnected = wasConnected;
            if (wasConnected) window.microLinkTerminal.startReading();
        }
        window.isYmodemActive = false; // 恢复主终端
    }
};

function crc16_ccitt(buf) {
    let crc = 0x0000;
    for (let i = 0; i < buf.length; i++) {
        crc ^= (buf[i] << 8);
        for (let j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
            crc &= 0xFFFF;
        }
    }
    return crc;
}

function buildYmodemPacketYmodem(seq, data) {
    let packet = new Uint8Array(133);
    packet[0] = 0x01; // SOH
    packet[1] = seq & 0xFF;
    packet[2] = (~seq) & 0xFF;
    for (let i = 0; i < 128; i++) {
        packet[3 + i] = data[i] !== undefined ? data[i] : 0x1A;
    }
    let crc = crc16_ccitt(packet.slice(3, 131));
    packet[131] = (crc >> 8) & 0xFF;
    packet[132] = crc & 0xFF;
    return packet;
}

// === 极简YMODEM头包构造（前缀逻辑保持不变，外部传入name） ===
function buildHeaderPacketYmodem(name, size) {
    // 检查是否为FLM文件，如果是则自动加前缀
    if (name && !name.startsWith('FLM/') && name.endsWith('.FLM.o')) {
        name = 'FLM/' + name;
    }
    let data = new Uint8Array(128);
    let nameBytes = new TextEncoder().encode(name);
    data.set(nameBytes, 0);
    let sizeBytes = new TextEncoder().encode(size.toString());
    data.set(sizeBytes, nameBytes.length + 1);
    return buildYmodemPacketYmodem(0, data);
}
// === 极简YMODEM结束包构造 ===
function buildEndPacketYmodem() {
    let data = new Uint8Array(128);
    return buildYmodemPacketYmodem(0, data);
}
// === 极简YMODEM主流程（可被各tab直接调用） ===
window.sendFileViaYmodem = async function(port, fileBuffer, fileName, fileSize, onLog) {
    let writer = port.writable.getWriter();
    let reader = port.readable.getReader();
    function log(msg) { onLog && onLog(msg); }
    // 1. 发送ym.receive()
    log('发送 ym.receive() 指令...');
    await writer.write(new TextEncoder().encode('ym.receive()\n'));
    // 2. 等待C
    log('等待接收端发送 "C"...');
    if (!(await waitForByteYmodem(reader, 0x43, 10000, log))) {
        log('未收到接收端 "C"，发送中止');
        return false;
    }
    // 3. 发送头包
    log('发送 Ymodem 文件头包...');
    if (!(await sendAndWaitAckYmodem(writer, reader, buildHeaderPacketYmodem(fileName, fileSize), log))) {
        log('头包发送失败，发送中止');
        return false;
    }
    // 4. 等待C
    log('等待接收端再次发送 "C"...');
    if (!(await waitForByteYmodem(reader, 0x43, 10000, log))) {
        log('未收到接收端第二个 "C"，发送中止');
        return false;
    }
    // 5. 发送数据包
    let seq = 1;
    for (let offset = 0; offset < fileBuffer.length; offset += 128) {
        let chunk = fileBuffer.slice(offset, offset + 128);
        log(`发送数据包 #${seq}...`);
        if (!(await sendAndWaitAckYmodem(writer, reader, buildYmodemPacketYmodem(seq, chunk), log))) {
            log(`数据包 #${seq} 发送失败，发送中止`);
            return false;
        }
        seq++;
    }
    // 6. 发送EOT
    log('发送 EOT...');
    for (let i = 0; i < 10; i++) {
        await writer.write(new Uint8Array([0x04]));
        let b = await readByteYmodem(reader, 3000, log);
        if (b === 0x06) break;
    }
    // 7. 等待C
    log('等待接收端发送 "C"...');
    if (!(await waitForByteYmodem(reader, 0x43, 10000, log))) {
        log('未收到接收端最后一个 "C"，发送中止');
        return false;
    }
    // 8. 发送空头包
    log('发送空头包...');
    if (!(await sendEndPacketWithRetry(writer, reader, buildEndPacketYmodem(), log))) {
        log('空头包发送失败');
        return false;
    }
    log('文件发送完成！');
    writer.releaseLock();
    reader.releaseLock();
    return true;
}

async function readByteYmodem(reader, timeout = 3000, onLog) {
    const timer = setTimeout(() => reader.cancel(), timeout);
    try {
        const { value } = await reader.read();
        clearTimeout(timer);
        return value ? value[0] : null;
    } catch {
        clearTimeout(timer);
        return null;
    }
}

async function waitForByteYmodem(reader, target, timeout = 10000, onLog) {
    let start = Date.now();
    while (Date.now() - start < timeout) {
        let b = await readByteYmodem(reader, timeout, onLog);
        if (b === target) return true;
    }
    return false;
}

async function sendAndWaitAckYmodem(writer, reader, packet, onLog, retry = 10) {
    for (let i = 0; i < retry; i++) {
        await writer.write(packet);
        let b = await readByteYmodem(reader, 3000, onLog);
        if (b === 0x06) return true; // ACK
        if (b === 0x15) continue;    // NAK
    }
    return false;
}

// 测试函数：验证YMODEM数据包格式
window.testYMODEMPacketFormat = function() {
    console.log('=== YMODEM数据包格式测试（CRC模式） ===');
    
    // 测试起始帧
    console.log('\n1. 测试起始帧格式');
    const headerPacket = createYMODEMHeaderPacket('test.bin', 1024);
    debugYMODEMPacket(headerPacket, '起始帧', 0);
    
    // 验证起始帧长度
    console.log(`起始帧长度验证: ${headerPacket.length}字节 (期望: 133字节)`);
    console.log(`数据区长度验证: ${headerPacket.slice(3, 131).length}字节 (期望: 128字节)`);
    
    // 测试数据帧 - 完整数据
    console.log('\n2. 测试数据帧格式（完整数据）');
    const fullData = new Uint8Array(128);
    for (let i = 0; i < 128; i++) {
        fullData[i] = i;
    }
    const fullDataPacket = createYMODEMDataPacket(fullData, 1);
    debugYMODEMPacket(fullDataPacket, '数据帧(完整)', 1);
    
    // 测试数据帧 - 部分数据（需要填充）
    console.log('\n3. 测试数据帧格式（部分数据，需要0x1A填充）');
    const partialData = new Uint8Array(64);
    for (let i = 0; i < 64; i++) {
        partialData[i] = i;
    }
    const partialDataPacket = createYMODEMDataPacket(partialData, 2);
    debugYMODEMPacket(partialDataPacket, '数据帧(部分)', 2);
    
    // 验证填充逻辑
    const dataArea = partialDataPacket.slice(3, 131);
    const originalData = dataArea.slice(0, 64);
    const paddingData = dataArea.slice(64);
    console.log(`原始数据长度: ${originalData.length}字节`);
    console.log(`填充数据长度: ${paddingData.length}字节`);
    console.log(`填充数据是否全为0x1A: ${paddingData.every(b => b === 0x1A)}`);
    
    // 测试结束帧
    console.log('\n4. 测试结束帧格式');
    const endPacket = createYMODEMEndPacket();
    debugYMODEMPacket(endPacket, '结束帧', 0);
    
    // 验证结束帧数据区
    const endDataArea = endPacket.slice(3, 131);
    console.log(`结束帧数据区是否全为0x00: ${endDataArea.every(b => b === 0x00)}`);
    
    console.log('\n=== 测试完成 ===');
    console.log('关键验证点:');
    console.log('- 所有SOH帧数据区严格为128字节');
    console.log('- 数据帧不足128字节时用0x1A填充');
    console.log('- 所有包使用CRC16校验');
    console.log('- 所有包长度133字节');
    
    return true;
};

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', () => {
    window.microLinkTerminal = new MicroLinkTerminal();
    
    // 在开发模式下自动运行测试
    if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
        console.log('开发模式：运行YMODEM数据包格式测试');
        setTimeout(() => {
            window.testYMODEMPacketFormat();
        }, 1000);
    }
});

// --- handleReceivedData 屏蔽逻辑 ---
const origHandleReceivedData = MicroLinkTerminal.prototype.handleReceivedData;
MicroLinkTerminal.prototype.handleReceivedData = function(data) {
    if (window.isYmodemActive) return;
    // 自动接入变量分析曲线
            // 旧的handleRealtimeHexData已删除，现在使用Worker方式
    return origHandleReceivedData.call(this, data);
};
// --- handleReceivedData 屏蔽逻辑 ---

// --- flushBuffer 屏蔽逻辑 ---
const origFlushBuffer = MicroLinkTerminal.prototype.flushBuffer;
MicroLinkTerminal.prototype.flushBuffer = function() {
    if (window.isYmodemActive) return;
    return origFlushBuffer.call(this);
};
// --- flushBuffer 屏蔽逻辑 ---

// --- startReading 屏蔽逻辑 ---
const origStartReading = MicroLinkTerminal.prototype.startReading;
MicroLinkTerminal.prototype.startReading = function() {
    if (window.isYmodemActive) return; // YMODEM期间禁止重启监听
    return origStartReading.call(this);
};
// --- startReading 屏蔽逻辑 ---

function waitForCSignal(reader, onLog) {
    // ...原有实现...
}
window.waitForCSignal = waitForCSignal;

function sendPacketWithACKRetry(writer, reader, packet, blockNumber, config, onLog, packetType, isHeaderPacket) {
    // ...原有实现...
}
window.sendPacketWithACKRetry = sendPacketWithACKRetry;

// === 关键：结束包后严格等待ACK，超时重发，最多10次 ===
async function sendEndPacketWithRetry(writer, reader, endPacket, onLog, maxRetry = 10) {
    for (let i = 0; i < maxRetry; i++) {
        await writer.write(endPacket);
        onLog && onLog(`发送空头包（第${i+1}次），等待ACK...`);
        let gotAck = false;
        const ackStart = Date.now();
        while (Date.now() - ackStart < 5000) { // 最多等5秒
            const { value, done } = await Promise.race([
                reader.read(),
                new Promise(resolve => setTimeout(() => resolve({value: null, done: false}), 200))
            ]);
            if (value) {
                for (let j = 0; j < value.length; j++) {
                    if (value[j] === 0x06) { // ACK
                        gotAck = true;
                        onLog && onLog('✅ 设备端ACK确认，YMODEM流程完成');
                        return true;
                    }
                }
            }
            await new Promise(r => setTimeout(r, 100));
        }
        onLog && onLog(`⚠️ 空头包第${i+1}次未收到ACK，准备重发...`);
    }
    onLog && onLog('❌ 空头包多次重试失败，设备端未响应ACK，传输中止。');
    return false;
}

// 多文件配置功能
let fileTableBody, addFileBtn, clearFilesBtn;

// 确保DOM加载完成后再初始化
function initMultiFileConfig() {
    fileTableBody = document.getElementById('fileTableBody');
    addFileBtn = document.getElementById('addFileBtn');
    clearFilesBtn = document.getElementById('clearFilesBtn');
    
    if (!fileTableBody || !addFileBtn || !clearFilesBtn) {
        console.error('多文件配置元素未找到，延迟初始化...');
        setTimeout(initMultiFileConfig, 100);
        return;
    }
    
    // 初始化表格
    initFileTable();
    
    // 事件监听器
    addFileBtn.addEventListener('click', function() {
        console.log('添加文件按钮被点击');
        addFileRow();
        if (window.updateCodePreview) {
            window.updateCodePreview();
        }
    });
    
    // 测试按钮是否正常工作
    console.log('多文件配置初始化完成', {
        fileTableBody: !!fileTableBody,
        addFileBtn: !!addFileBtn,
        clearFilesBtn: !!clearFilesBtn
    });
    
    clearFilesBtn.addEventListener('click', function() {
        if (confirm('确定要清空所有文件配置吗？')) {
            clearAllFiles();
        }
    });
}

// 初始化文件表格
function initFileTable() {
    if (!fileTableBody) return;
    fileTableBody.innerHTML = '';
    if (window.config && window.config.files) {
        window.config.files.forEach((file, index) => {
            addFileRow(file, index);
        });
    }
}

// 添加文件行
function addFileRow(file = null, index = null) {
    console.log('添加文件行被调用', { file, index });
    if (!fileTableBody) {
        console.error('fileTableBody 未找到');
        return;
    }
    const row = document.createElement('div');
    row.className = 'file-table-row';
    row.dataset.index = index !== null ? index : (window.config && window.config.files ? window.config.files.length : 0);
    
    const fileNameInput = document.createElement('input');
    fileNameInput.type = 'text';
    fileNameInput.placeholder = '例如: boot.bin';
    fileNameInput.value = file ? file.fileName : '';
    
    const addressInput = document.createElement('input');
    addressInput.type = 'text';
    addressInput.placeholder = '例如: 0x08000000';
    addressInput.value = file ? file.address : '';
    
    const algorithmInput = document.createElement('input');
    algorithmInput.type = 'text';
    algorithmInput.placeholder = '例如: STM32F7x_1024.FLM.o';
    algorithmInput.value = file ? file.algorithm : '';
    
    const deleteBtn = document.createElement('button');
    deleteBtn.className = 'delete-file-btn';
    deleteBtn.innerHTML = '<i class="fas fa-trash"></i>';
    deleteBtn.title = '删除此行';
    
    row.appendChild(fileNameInput);
    row.appendChild(addressInput);
    row.appendChild(algorithmInput);
    row.appendChild(deleteBtn);
    
    fileTableBody.appendChild(row);
    
    // 添加事件监听器
    fileNameInput.addEventListener('input', function() {
        updateFileConfig();
    });
    addressInput.addEventListener('input', function() {
        updateFileConfig();
    });
    algorithmInput.addEventListener('input', function() {
        updateFileConfig();
    });
    deleteBtn.addEventListener('click', function() {
        deleteFileRow(row);
    });
    
    // 如果是新行，添加到配置中
    if (!file && window.config && window.config.files) {
        window.config.files.push({
            fileName: '',
            address: '',
            algorithm: ''
        });
    }
}

// 删除文件行
function deleteFileRow(row) {
    if (!fileTableBody || !row) return;
    const index = parseInt(row.dataset.index);
    if (window.config && window.config.files && index >= 0 && index < window.config.files.length) {
        window.config.files.splice(index, 1);
    }
    fileTableBody.removeChild(row);
    updateRowIndices();
    if (window.updateCodePreview) {
        window.updateCodePreview();
    }
}

// 更新行索引
function updateRowIndices() {
    if (!fileTableBody) return;
    const rows = fileTableBody.querySelectorAll('.file-table-row');
    rows.forEach((row, index) => {
        row.dataset.index = index;
    });
}

// 更新文件配置
function updateFileConfig() {
    if (!fileTableBody || !window.config || !window.config.files) return;
    const rows = fileTableBody.querySelectorAll('.file-table-row');
    window.config.files = [];
    rows.forEach(row => {
        const inputs = row.querySelectorAll('input');
        if (inputs.length >= 3) {
            window.config.files.push({
                fileName: inputs[0].value,
                address: inputs[1].value,
                algorithm: inputs[2].value
            });
        }
    });
    if (window.updateCodePreview) {
        window.updateCodePreview();
    }
}

// 清空所有文件
function clearAllFiles() {
    if (!fileTableBody) return;
    fileTableBody.innerHTML = '';
    if (window.config && window.config.files) {
        window.config.files.length = 0;
    }
    if (window.updateCodePreview) {
        window.updateCodePreview();
    }
}

// 三栏布局：sidebar切换逻辑
function setupSidebarPanelSwitch() {
    const sidebarBtns = document.querySelectorAll('.sidebar-btn');
    const panels = ['serialPanel', 'flmPanel', 'scriptPanel', 'varPanel'];
    const monitorPanel = document.querySelector('.monitor-panel');

    sidebarBtns.forEach(btn => {
        btn.addEventListener('click', function() {
            // 切换按钮active
            sidebarBtns.forEach(b => b.classList.remove('active'));
            this.classList.add('active');
            // 切换内容区
            panels.forEach(pid => {
                const panel = document.getElementById(pid);
                if (panel) panel.style.display = (this.dataset.panel === pid) ? 'block' : 'none';
            });
            
            // 控制右侧监控面板的显示/隐藏
            if (monitorPanel) {
                if (this.dataset.panel === 'varPanel') {
                    // 变量分析页面：隐藏监控面板
                    monitorPanel.style.display = 'none';
                } else {
                    // 其他页面：显示监控面板
                    monitorPanel.style.display = '';
                }
            }
            
            // 切换到Python脚本配置时初始化
            if (this.dataset.panel === 'scriptPanel') {
                setupPythonScriptPanel();
                // 初始化多文件配置
                initMultiFileConfig();
            }
            // 切换到变量分析时初始化
            if (this.dataset.panel === 'varPanel') {
                setupVarAnalysisPanel();
            }
        });
    });
}
document.addEventListener('DOMContentLoaded', function() {
    setupSidebarPanelSwitch();
});

// ... 现有代码 ...
function setupPythonScriptPanel() {
    // 防止重复绑定
    if (window._pythonPanelInited) return;
    window._pythonPanelInited = true;
    const swdClockSpeedMap = { '10M': '10000000', '5M': '5000000', '2M': '2000000', '1M': '1000000', '500K': '500000', '200K': '200000', '100K': '100000', '50K': '50000', '20K': '20000', '10K': '10000', '5K': '5000' };
    const customFlmInput = document.getElementById('customFlm');
    const address1Input = document.getElementById('address1');
    const address2Input = document.getElementById('address2');
    const binFileNameInput = document.getElementById('binFileName');
    const swdClockSpeedSelect = document.getElementById('swdClockSpeed');
    const codePreview = document.getElementById('codePreview');
    const dragCodePreview = document.getElementById('dragCodePreview');
    // 删除下载按钮相关代码
    const pyYmodemSendBtn = document.getElementById('pyYmodemSendBtn');
    const pyYmodemProgress = document.getElementById('pyYmodemProgress');
    const pyYmodemLog = document.getElementById('pyYmodemLog');
    // 使用全局config对象
    if (typeof window.config === 'undefined') {
        window.config = { 
            flmFile: (customFlmInput && customFlmInput.value) || 'custom_flm.FLM.o', 
            address1: (address1Input && address1Input.value) || '0X08000000', 
            address2: (address2Input && address2Input.value) || '0x20000000', 
            binFileName: (binFileNameInput && binFileNameInput.value) || 'ILI9341_HAL.bin', 
            swdClockSpeed: (swdClockSpeedSelect && swdClockSpeedSelect.value) || '10M',
            files: [
                {
                    fileName: 'boot.bin',
                    address: '0x08000000',
                    algorithm: 'STM32F7x_1024.FLM.o'
                },
                {
                    fileName: 'rtthread.bin',
                    address: '0x08020000', 
                    algorithm: 'STM32F7x_1024.FLM.o'
                },
                {
                    fileName: 'HZK.bin',
                    address: '0x90000000',
                    algorithm: 'STM32F767_W25QXX.FLM.o'
                }
            ]
        };
    }
    const config = window.config;
    
    // 确保config对象有files数组
    if (!config.files) {
        config.files = [
            {
                fileName: 'boot.bin',
                address: '0x08000000',
                algorithm: 'STM32F7x_1024.FLM.o'
            },
            {
                fileName: 'rtthread.bin',
                address: '0x08020000', 
                algorithm: 'STM32F7x_1024.FLM.o'
            },
            {
                fileName: 'HZK.bin',
                address: '0x90000000',
                algorithm: 'STM32F767_W25QXX.FLM.o'
            }
        ];
    }
    // 全局updateCodePreview函数
    window.updateCodePreview = function() {
        const flmFile = config.flmFile;
        const pythonSwdSpeed = swdClockSpeedMap[config.swdClockSpeed] || '10000000';
        
        // 生成多文件烧录代码
        let offlineCode = `import FLMConfig\nimport PikaStdLib\nimport PikaStdDevice\nimport time\n\ntime = PikaStdDevice.Time()\nbuzzer = PikaStdDevice.GPIO()\nbuzzer.setPin('PA4')\nbuzzer.setMode('out')\n\n# 设置SWD下载速度\ncmd.set_swd_clock(${pythonSwdSpeed})\n\nReadFlm = FLMConfig.ReadFlm()`;
        
        // 按算法分组文件
        const algorithmGroups = {};
        if (config.files && config.files.length > 0) {
            config.files.forEach(file => {
                if (file.algorithm && file.fileName && file.address) {
                    if (!algorithmGroups[file.algorithm]) {
                        algorithmGroups[file.algorithm] = [];
                    }
                    algorithmGroups[file.algorithm].push(file);
                }
            });
            
            // 为每个算法生成加载和烧录代码
            Object.keys(algorithmGroups).forEach((algorithm, index) => {
                const files = algorithmGroups[algorithm];
                if (files.length > 0) {
                    // 加载算法
                    offlineCode += `\n# 加载 ${algorithm} 下载算法文件\nresult = ReadFlm.load("FLM/${algorithm}", ${config.address1}, ${config.address2})\nif result != 0:\n    return`;
                    
                    // 烧录该算法下的所有文件
                    files.forEach(file => {
                        offlineCode += `\n\n# 烧写 ${file.fileName}\nresult = load.bin("${file.fileName}", ${file.address})\nif result != 0:\n    return`;
                    });
                }
            });
        } else {
            // 如果没有多文件配置，使用默认的单文件配置
            offlineCode += `\n# 加载 FLM 文件\nresult = ReadFlm.load("FLM/${flmFile}", ${config.address1}, ${config.address2})\nif result != 0:\n    return \n\n# 烧写固件文件\nresult = load.bin("${config.binFileName || 'firmware.bin'}", ${config.address1})\nif result != 0:\n    return`;
        }
        
        offlineCode += `\n\n# 蜂鸣器响一声，表示烧写完成\nbuzzer.enable()\nbuzzer.high()\ntime.sleep_ms(500)\nbuzzer.low()\ntime.sleep_ms(500)`;
        
        const dragCode = `import FLMConfig\ncmd.set_swd_clock(${pythonSwdSpeed})\nReadFlm = FLMConfig.ReadFlm()\nres1 = ReadFlm.load("FLM/${flmFile}",${config.address1},${config.address2})`;
        
        if (codePreview) {
            codePreview.textContent = offlineCode;
        }
        if (dragCodePreview) {
            dragCodePreview.textContent = dragCode;
        }
        
        // 高亮显示
        let highlightedCode = offlineCode;
        if (config.files && config.files.length > 0) {
            config.files.forEach(file => {
                if (file.fileName) {
                    highlightedCode = highlightedCode.replace(
                        new RegExp(`"${file.fileName}"`, 'g'), 
                        `<span class="highlight">"${file.fileName}"</span>`
                    );
                }
                if (file.address) {
                    highlightedCode = highlightedCode.replace(
                        new RegExp(file.address.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), 
                        `<span class="highlight">${file.address}</span>`
                    );
                }
                if (file.algorithm) {
                    highlightedCode = highlightedCode.replace(
                        new RegExp(`"FLM/${file.algorithm}"`, 'g'), 
                        `<span class="highlight">"FLM/${file.algorithm}"</span>`
                    );
                }
            });
        }
        
        // 高亮其他配置项
        highlightedCode = highlightedCode.replace(`"FLM/${flmFile}"`, `<span class="highlight">"FLM/${flmFile}"</span>`)
            .replace(new RegExp(config.address1.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${config.address1}</span>`)
            .replace(new RegExp(config.address2.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${config.address2}</span>`)
            .replace(new RegExp(pythonSwdSpeed.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${pythonSwdSpeed}</span>`);
        
        if (codePreview) {
            codePreview.innerHTML = highlightedCode;
        }
        if (dragCodePreview) {
            dragCodePreview.innerHTML = dragCode.replace(`"FLM/${flmFile}"`, `<span class="highlight">"FLM/${flmFile}"</span>`)
                .replace(new RegExp(config.address1.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${config.address1}</span>`)
                .replace(new RegExp(config.address2.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${config.address2}</span>`)
                .replace(new RegExp(pythonSwdSpeed.replace(/[.*+?^${}()|[\\]\\]/g, '\\$&'), 'g'), `<span class="highlight">${pythonSwdSpeed}</span>`);
        }
    }
    if (customFlmInput) {
        customFlmInput.addEventListener('input', function() { config.flmFile = this.value || 'custom_flm.FLM.o'; window.updateCodePreview(); });
    }
    if (address1Input) {
        address1Input.addEventListener('input', function() { config.address1 = this.value || '0X08000000'; window.updateCodePreview(); });
    }
    if (address2Input) {
        address2Input.addEventListener('input', function() { config.address2 = this.value || '0x20000000'; window.updateCodePreview(); });
    }
    if (binFileNameInput) {
        binFileNameInput.addEventListener('input', function() { config.binFileName = this.value || 'firmware.bin'; window.updateCodePreview(); });
    }
    if (swdClockSpeedSelect) {
        swdClockSpeedSelect.addEventListener('change', function() { config.swdClockSpeed = this.value; window.updateCodePreview(); });
    }
    // 删除下载按钮事件监听器
    // YMODEM发送按钮状态和事件
    function updatePyYmodemSendBtnState() {
        const tab = document.querySelector('.script-tab.active');
        if (tab && tab.textContent.includes('离线')) {
            pyYmodemSendBtn.disabled = false;
            pyYmodemSendBtn.setAttribute('data-pytype', 'offline');
        } else if (tab && tab.textContent.includes('拖拽')) {
            pyYmodemSendBtn.disabled = false;
            pyYmodemSendBtn.setAttribute('data-pytype', 'drag');
        } else {
            pyYmodemSendBtn.disabled = true;
            pyYmodemSendBtn.removeAttribute('data-pytype');
        }
    }
    const scriptTabs = document.querySelectorAll('.script-tab');
    if (scriptTabs.length > 0) {
        scriptTabs.forEach(tab => {
            tab.addEventListener('click', updatePyYmodemSendBtnState);
        });
        updatePyYmodemSendBtnState();
    }
    // Python日志输出到主终端
    // Python日志输出到主终端
    function pyYlog(msg, color) {
        appendToTerminalOutput(`<div class='log-prefix-python'>[PYTHON] ${msg}</div>`);
    }
    function pyYlogClear() {
        // 不清空主终端
    }
    
    // 统一的Python YMODEM发送函数
    async function sendPythonYmodem(code, fileName) {
        if (!window.microLinkTerminal || !window.microLinkTerminal.isConnected || !window.microLinkTerminal.port) {
            pyYlog('请先连接串口', '#f66'); return;
        }
        pyYmodemSendOfflineBtn && (pyYmodemSendOfflineBtn.disabled = true);
        pyYmodemSendDragBtn && (pyYmodemSendDragBtn.disabled = true);
        pyYlogClear();
        pyYlog('准备发送...', '#0ff');
        let wasConnected = false;
        if (window.microLinkTerminal) {
            if (window.microLinkTerminal.reader) {
                try { window.microLinkTerminal.reader.cancel(); } catch(e){}
                try { window.microLinkTerminal.reader.releaseLock(); } catch(e){}
                window.microLinkTerminal.reader = null;
            }
            wasConnected = window.microLinkTerminal.isConnected;
            window.microLinkTerminal.isConnected = false;
            await new Promise(r => setTimeout(r, 300));
        }
        try {
            const uint8Array = new TextEncoder().encode(code);
            const port = window.microLinkTerminal && window.microLinkTerminal.port;
            if (!port) throw new Error('串口未连接');
            let ok = await window.sendFileViaYmodem(
                port,
                uint8Array,
                fileName,
                uint8Array.length,
                msg => pyYlog(msg)
            );
            if (ok) {
                pyYlog('✅ 发送完成', '#0f0');
            } else {
                pyYlog('❌ 发送失败', '#f66');
            }
        } catch (e) {
            pyYlog('❌ 发送失败: ' + e.message, '#f66');
            if (e && e.stack) pyYlog('错误堆栈: ' + e.stack, '#f66');
        } finally {
            if (window.microLinkTerminal) {
                window.microLinkTerminal.isConnected = wasConnected;
                if (wasConnected && typeof window.microLinkTerminal.startReading === 'function') {
                    window.microLinkTerminal.startReading();
                }
            }
            pyYmodemSendOfflineBtn && (pyYmodemSendOfflineBtn.disabled = false);
            pyYmodemSendDragBtn && (pyYmodemSendDragBtn.disabled = false);
        }
    }
    
    function getOfflineCode() {
        const flmFile = config.flmFile;
        const pythonSwdSpeed = swdClockSpeedMap[config.swdClockSpeed] || '10000000';
        
        // 生成多文件烧录代码
        let offlineCode = `import FLMConfig\nimport PikaStdLib\nimport PikaStdDevice\nimport time\n\ntime = PikaStdDevice.Time()\nbuzzer = PikaStdDevice.GPIO()\nbuzzer.setPin('PA4')\nbuzzer.setMode('out')\n\n# 设置SWD下载速度\ncmd.set_swd_clock(${pythonSwdSpeed})\n\nReadFlm = FLMConfig.ReadFlm()`;
        
        // 按算法分组文件
        const algorithmGroups = {};
        config.files.forEach(file => {
            if (file.algorithm && file.fileName && file.address) {
                if (!algorithmGroups[file.algorithm]) {
                    algorithmGroups[file.algorithm] = [];
                }
                algorithmGroups[file.algorithm].push(file);
            }
        });
        
        // 为每个算法生成加载和烧录代码
        Object.keys(algorithmGroups).forEach((algorithm, index) => {
            const files = algorithmGroups[algorithm];
            if (files.length > 0) {
                // 加载算法
                offlineCode += `\n# 加载 ${algorithm} 下载算法文件\nresult = ReadFlm.load("FLM/${algorithm}", ${config.address1}, ${config.address2})\nif result != 0:\n    return`;
                
                // 烧录该算法下的所有文件
                files.forEach(file => {
                    offlineCode += `\n\n# 烧写 ${file.fileName}\nresult = load.bin("${file.fileName}", ${file.address})\nif result != 0:\n    return`;
                });
            }
        });
        
        offlineCode += `\n\n# 蜂鸣器响一声，表示烧写完成\nbuzzer.enable()\nbuzzer.high()\ntime.sleep_ms(500)\nbuzzer.low()\ntime.sleep_ms(500)`;
        
        return offlineCode;
    }
    function getDragCode() {
        const flmFile = config.flmFile;
        const pythonSwdSpeed = swdClockSpeedMap[config.swdClockSpeed] || '10000000';
        return `import FLMConfig\ncmd.set_swd_clock(${pythonSwdSpeed})\nReadFlm = FLMConfig.ReadFlm()\nres1 = ReadFlm.load(\"FLM/${flmFile}\",${config.address1},${config.address2})`;
    }
    // 删除旧的pyYmodemSendBtn事件绑定
    
    // 统一事件绑定，确保使用正确的脚本内容
    const pyYmodemSendOfflineBtn = document.getElementById('pyYmodemSendOfflineBtn');
    const pyYmodemSendDragBtn = document.getElementById('pyYmodemSendDragBtn');
    if (pyYmodemSendOfflineBtn) {
        pyYmodemSendOfflineBtn.addEventListener('click', async () => {
            await sendPythonYmodem(getOfflineCode(), 'Python/offline_download.py');
        });
    }
    if (pyYmodemSendDragBtn) {
        pyYmodemSendDragBtn.addEventListener('click', async () => {
            await sendPythonYmodem(getDragCode(), 'Python/drag_download.py');
        });
    }
    
    window.updateCodePreview();
}
// ... 现有代码 ...

// ... 只展示相关修改 ...
        // 事件绑定移到函数定义之后

// 删除重复的全局getOfflineCode函数定义
function getDragCode() {
    const customFlmInput = document.getElementById('customFlm');
    const address1Input = document.getElementById('address1');
    const address2Input = document.getElementById('address2');
    const swdClockSpeedSelect = document.getElementById('swdClockSpeed');
    const flmFile = customFlmInput ? customFlmInput.value || 'custom_flm.FLM.o' : 'custom_flm.FLM.o';
    const address1 = address1Input ? address1Input.value || '0X08000000' : '0X08000000';
    const address2 = address2Input ? address2Input.value || '0x20000000' : '0x20000000';
    const swdClockSpeedMap = { '10M': '10000000', '5M': '5000000', '2M': '2000000', '1M': '1000000', '500K': '500000', '200K': '200000', '100K': '100000', '50K': '50000', '20K': '20000', '10K': '10000', '5K': '5000' };
    const pythonSwdSpeed = swdClockSpeedMap[swdClockSpeedSelect ? swdClockSpeedSelect.value : '10M'] || '10000000';
    return `import FLMConfig\ncmd.set_swd_clock(${pythonSwdSpeed})\nReadFlm = FLMConfig.ReadFlm()\nres1 = ReadFlm.load(\"FLM/${flmFile}\",${address1},${address2})`;
}
// 删除重复的sendPythonYmodem函数定义
// 删除重复的事件绑定，统一在setupPythonScriptPanel中处理

// FLM .o文件 YMODEM发送
async function handleFlmYmodemSend() {
    const flmYmodemSendBtn = document.getElementById('flmYmodemSendBtn');
    const flmYmodemProgress = document.getElementById('flmYmodemProgress');
    const log = document.getElementById('log');
    // 依赖 convertedBlob, flmFileName, isSerialConnected
    if (!window.convertedBlob) {
        if (log) log.textContent += '\n请先生成.o文件';
        return;
    }
    if (!window.microLinkTerminal || !window.microLinkTerminal.isConnected || !window.microLinkTerminal.port) {
        if (log) log.textContent += '\n请先连接串口';
        return;
    }
    flmYmodemSendBtn.disabled = true;
    flmYmodemProgress.style.display = '';
    flmYmodemProgress.value = 0;
    if (log) log.textContent += '\n准备发送...';
    let wasConnected = false;
    if (window.microLinkTerminal) {
        if (window.microLinkTerminal.reader) {
            try { window.microLinkTerminal.reader.cancel(); } catch(e){}
            try { window.microLinkTerminal.reader.releaseLock(); } catch(e){}
            window.microLinkTerminal.reader = null;
        }
        wasConnected = window.microLinkTerminal.isConnected;
        window.microLinkTerminal.isConnected = false;
        await new Promise(r => setTimeout(r, 300));
    }
    try {
        const arrayBuffer = await window.convertedBlob.arrayBuffer();
        const uint8Array = new Uint8Array(arrayBuffer);
        // 修改点：加上 FLM/ 前缀
        const fileName = 'FLM/' + (window.flmFileName ? (window.flmFileName + '.FLM.o') : 'firmware.FLM.o');
        const port = window.microLinkTerminal && window.microLinkTerminal.port;
        let ok = await window.sendFileViaYmodem(
            port,
            uint8Array,
            fileName,
            uint8Array.length,
            progress => { flmYmodemProgress.value = progress; },
            msg => { if (log) log.textContent += '\n' + msg; }
        );
        if (ok) {
            if (log) log.textContent += '\n✅ 文件发送完成';
        } else {
            if (log) log.textContent += '\n❌ 发送失败';
        }
    } catch (e) {
        if (log) log.textContent += '\n❌ 发送失败: ' + e.message;
    } finally {
        if (window.microLinkTerminal) {
            window.microLinkTerminal.isConnected = wasConnected;
            if (wasConnected && typeof window.microLinkTerminal.startReading === 'function') {
                window.microLinkTerminal.startReading();
            }
        }
        flmYmodemSendBtn.disabled = false;
        flmYmodemProgress.style.display = 'none';
    }
}

// Python脚本 YMODEM发送
async function handlePyYmodemSend(type) {
    const pyYmodemSendOfflineBtn = document.getElementById('pyYmodemSendOfflineBtn');
    const pyYmodemSendDragBtn = document.getElementById('pyYmodemSendDragBtn');
    const pyYmodemProgress = document.getElementById('pyYmodemProgress');
    const pyYmodemLog = document.getElementById('pyYmodemLog');
    function pyYlog(msg, color) {
        appendToTerminalOutput(`<div class='log-prefix-python'>[PYTHON] ${msg}</div>`);
    }
    function pyYlogClear() {
        // 不清空主终端
    }
    if (!window.microLinkTerminal || !window.microLinkTerminal.isConnected || !window.microLinkTerminal.port) {
        pyYlog('请先连接串口', '#f66'); return;
    }
    pyYmodemSendOfflineBtn && (pyYmodemSendOfflineBtn.disabled = true);
    pyYmodemSendDragBtn && (pyYmodemSendDragBtn.disabled = true);
    pyYmodemProgress && (pyYmodemProgress.value = 0);
    pyYmodemProgress && (pyYmodemProgress.style.display = '');
    pyYlogClear();
    pyYlog('准备发送...', '#0ff');
    let wasConnected = false;
    if (window.microLinkTerminal) {
        if (window.microLinkTerminal.reader) {
            try { window.microLinkTerminal.reader.cancel(); } catch(e){}
            try { window.microLinkTerminal.reader.releaseLock(); } catch(e){}
            window.microLinkTerminal.reader = null;
        }
        wasConnected = window.microLinkTerminal.isConnected;
        window.microLinkTerminal.isConnected = false;
        await new Promise(r => setTimeout(r, 300));
    }
    try {
        let code = '';
        let fileName = '';
        if (type === 'offline') {
            code = window.getOfflineCode ? window.getOfflineCode() : '';
            fileName = 'Python/offline_download.py';
        } else if (type === 'drag') {
            code = window.getDragCode ? window.getDragCode() : '';
            fileName = 'Python/drag_download.py';
        } else {
            pyYlog('只允许发送离线下载脚本或拖拽下载脚本', '#f66');
            return;
        }
        const uint8Array = new TextEncoder().encode(code);
        const port = window.microLinkTerminal && window.microLinkTerminal.port;
        let ok = await window.sendFileViaYmodem(
            port,
            uint8Array,
            fileName,
            uint8Array.length,
            msg => pyYlog(msg)
        );
        if (ok) {
            pyYlog('✅ 发送完成', '#0f0');
        } else {
            pyYlog('❌ 发送失败', '#f66');
        }
    } catch (e) {
        pyYlog('❌ 发送失败: ' + e.message, '#f66');
        if (e && e.stack) pyYlog('错误堆栈: ' + e.stack, '#f66');
    } finally {
        if (window.microLinkTerminal) {
            window.microLinkTerminal.isConnected = wasConnected;
            if (wasConnected && typeof window.microLinkTerminal.startReading === 'function') {
                window.microLinkTerminal.startReading();
            }
        }
        pyYmodemSendOfflineBtn && (pyYmodemSendOfflineBtn.disabled = false);
        pyYmodemSendDragBtn && (pyYmodemSendDragBtn.disabled = false);
        pyYmodemProgress && (pyYmodemProgress.style.display = 'none');
    }
}

// ... existing code ...
function setupVarAnalysisPanel() {
    // 只绑定一次
    if (window._varPanelInited) return;
    window._varPanelInited = true;
    const fileInput = document.getElementById('axfFile');
    const analyzeBtn = document.getElementById('analyzeBtn');
    if (!fileInput || !analyzeBtn) return;
    fileInput.addEventListener('change', function(e) {
        if (this.files.length > 0) {
            analyzeBtn.disabled = false;
            const fileInfo = document.getElementById('fileInfo');
            if (fileInfo) fileInfo.classList.add('d-none');
        } else {
            analyzeBtn.disabled = true;
        }
    });
}
// ... existing code ...

// ========== 变量分析tab实时曲线功能 ==========
let chartData = [];
let chartInstance = null;
let chartDrawEnabled = false;
  // 将chartDrawEnabled挂载到全局，供数据处理函数使用
  window.chartDrawEnabled = chartDrawEnabled;
function setupRealtimeChart() {
    const chartDom = document.getElementById('realtimeChart');
    if (!chartDom) return;
    if (!window.echarts) return;
    
    // 初始化图表数据
    if (!window.chartData) {
        window.chartData = [];
    }
    
    chartInstance = echarts.init(chartDom);
    
    // 延迟初始化多变量图表管理器，确保DOM元素已创建
    setTimeout(() => {
        window.multiChartManager = new MultiChartManager();
        console.log('多变量图表管理器初始化完成');
    }, 200);
    

    
    // 初始化后强制resize，确保图表尺寸正确
    setTimeout(() => {
        if (chartInstance) {
            chartInstance.resize();
        }
    }, 100);
    

    
    // 采用test.html的ECharts配置
    chartInstance.setOption({
        xAxis: { 
            type: 'value',
            name: '时间 (秒)',
            nameLocation: 'middle',
            nameGap: 30
        },
        yAxis: { 
            type: 'value', 
            scale: true,
            name: '数值',
            nameLocation: 'middle',
            nameGap: 40
        },
        series: [{ 
            type: 'line', 
            data: [], 
            smooth: true, 
            symbol: 'none', 
            lineStyle: { width: 2, color: '#3498db' },
            name: '实时数据'
        }],
        grid: { 
            left: 80, 
            right: 40, 
            top: 50, 
            bottom: 80,
            containLabel: true
        },
        animation: false,
        tooltip: {
            trigger: 'axis',
            formatter: function(params) {
                const data = params[0];
                return `时间: ${data.value[0].toFixed(2)}s<br/>数值: ${data.value[1]}`;
            }
        }
    });
    document.getElementById('clearChartBtn').onclick = function() {
        window.chartData = [];
        chartTimeData = []; // 清除时间数据
        chartDataBuffer = []; // 清除缓冲区
        dataIntegrityErrors = 0; // 重置错误计数
        expectedDataPattern = null; // 重置数据模式
        lastProcessedTime = 0; // 重置时间戳
        
        // 停止Worker
        if (chartDataWorker) {
            chartDataWorker.postMessage({ type: 'stop' });
        }
        if (chartRenderWorker) {
            chartRenderWorker.postMessage({ type: 'stop' });
        }
        
        // 清空多变量图表
        if (window.multiChartManager) {
            window.multiChartManager.clearAllCharts();
        }
        
        // 清空图表数据，不调用updateRealtimeChart避免覆盖时间轴逻辑
        if (chartInstance) {
            chartInstance.setOption({
                series: [{
                    data: []
                }]
            });
        }
        // 重置开始绘制按钮状态
        const startBtn = document.getElementById('startBtn');
        if (startBtn) {
            startBtn.textContent = '开始绘制';
            startBtn.className = 'btn btn-success';
        }
        chartDrawEnabled = false;
          window.chartDrawEnabled = chartDrawEnabled;
        console.log('[曲线模式] 已清除数据并停止绘制');
        console.log('[曲线缓冲] 已清除缓冲区');
        console.log('[数据完整性] 已重置错误计数器和数据模式');
        console.log('[Worker] 已停止数据处理Worker');
        console.log('[数据帧解析] 已清空数据帧缓冲区');
        

    };
    // 开始绘制按钮
    const startBtn = document.getElementById('startChartBtn');
    if (startBtn) {
        startBtn.onclick = function() {
            if (!chartDrawEnabled) {
                // 开始绘制
                chartDrawEnabled = true;
                  window.chartDrawEnabled = chartDrawEnabled;
                startBtn.textContent = '暂停绘制';
                startBtn.className = 'btn btn-warning';
                
                // 自动开启HEX模式
                const hexModeCheckbox = document.getElementById('hexMode');
                if (hexModeCheckbox && !hexModeCheckbox.checked) {
                    hexModeCheckbox.checked = true;
                    hexModeCheckbox.dispatchEvent(new Event('change'));
                }
                
                // 在控制台显示曲线模式已启动
                console.log('[曲线模式] 已启动 - 串口数据将直接用于曲线绘制，不显示在主监控');
                console.log('[曲线模式] 请确保设备发送4字节对齐的HEX数据');
                
                // 清除缓冲区，准备接收新数据
                chartDataBuffer = [];
                chartTimeData = []; // 清除时间数据
                dataIntegrityErrors = 0; // 重置错误计数
                expectedDataPattern = null; // 重置数据模式
                lastProcessedTime = 0; // 重置时间戳
                

                
                // 初始化Worker
                initChartWorkers();
                
                // 启动Worker
                if (chartDataWorker) {
                    chartDataWorker.postMessage({ type: 'start' });
                }
                if (chartRenderWorker) {
                    chartRenderWorker.postMessage({ type: 'start' });
                }
                
                console.log('[曲线缓冲] 已清除缓冲区，准备接收新数据');
                console.log('[数据完整性] 已重置错误计数器和数据模式');
                console.log('[Worker] 已启动数据处理Worker');
                
            } else {
                // 暂停绘制
                chartDrawEnabled = false;
                  window.chartDrawEnabled = chartDrawEnabled;
                startBtn.textContent = '继续绘制';
                startBtn.className = 'btn btn-success';
                
                // 停止Worker
                if (chartDataWorker) {
                    chartDataWorker.postMessage({ type: 'stop' });
                }
                if (chartRenderWorker) {
                    chartRenderWorker.postMessage({ type: 'stop' });
                }
                
                console.log('[曲线模式] 已暂停 - 恢复正常监控显示');
                console.log('[Worker] 已停止数据处理Worker');
            }
        };
    }
    

    // 终止绘制按钮逻辑
    const stopChartBtn = document.getElementById('stopChartBtn');
    if (stopChartBtn) {
        stopChartBtn.onclick = function() {
            console.log('[终止绘制] 按钮被点击');
            
            // 1. 清空曲线数据
            window.chartData = [];
            chartTimeData = []; // 清除时间数据
            chartDataBuffer = []; // 清除缓冲区
            dataIntegrityErrors = 0; // 重置错误计数
            expectedDataPattern = null; // 重置数据模式
            lastProcessedTime = 0; // 重置时间戳
            
            // 2. 停止曲线绘制
            if (window.chartDrawEnabled) {
                window.chartDrawEnabled = false;
                chartDrawEnabled = false;
                
                // 更新按钮状态
                const startBtn = document.getElementById('startChartBtn');
                if (startBtn) {
                    startBtn.textContent = '开始绘制';
                    startBtn.className = 'btn btn-success';
                }
                
                // 停止Worker
                if (window.chartDataWorker) {
                    window.chartDataWorker.postMessage({ type: 'stop' });
                }
                if (window.chartRenderWorker) {
                    window.chartRenderWorker.postMessage({ type: 'stop' });
                }
                
                console.log('[终止绘制] 曲线绘制已停止');
            }
            
            // 3. 清空多变量图表
            if (window.multiChartManager) {
                window.multiChartManager.clearAllCharts();
            }
            
            // 4. 清空图表显示
            if (chartInstance) {
                chartInstance.setOption({
                    series: [{
                        data: []
                    }]
                });
            }
            
            // 5. 发送终止绘制命令（如果串口已连接）
            const stopCommand = 'cmd.read_ram(0x20000000,1,0)';
            if (window.microLinkTerminal && window.microLinkTerminal.isConnected) {
                window.microLinkTerminal.sendCommand(stopCommand);
                console.log('[终止绘制] 已发送停止命令:', stopCommand);
            } else {
                console.log('[终止绘制] 串口未连接，跳过发送命令');
            }
            
            // 6. 关闭监听框的HEX模式
            const hexModeCheckbox = document.getElementById('hexMode');
            if (hexModeCheckbox && hexModeCheckbox.checked) {
                hexModeCheckbox.checked = false;
                hexModeCheckbox.dispatchEvent(new Event('change'));
                console.log('[终止绘制] 已关闭HEX模式');
            }
            
            console.log('[终止绘制] 操作完成 - 已清空曲线并停止绘制');
        };
    }
    // 保证全局可用（每次都强制挂载，防止tab切换后失效）
    window.chartData = window.chartData || [];
    window.updateRealtimeChart = updateRealtimeChart;
    // 旧的handleRealtimeHexData已删除，现在使用Worker方式
    window.chartInstance = chartInstance;
    // 再次初始化终端区监听，防止tab切换后丢失
    if (!window._terminalHexChartSyncInited) {
        setupTerminalHexChartSync();
        window._terminalHexChartSyncInited = true;
    }
}
// 全局时间轴数据
let chartTimeData = [];
// 将chartTimeData挂载到全局，供多变量图表使用
window.chartTimeData = chartTimeData;

function updateRealtimeChart() {
    if (!chartInstance) return;
    // 确保有足够的数据点来显示X轴
    const data = window.chartData || [];
    const timeData = chartTimeData || [];
    
    // 如果时间数据不足，补充时间数据
    while (timeData.length < data.length) {
        const currentTime = new Date();
        timeData.push(currentTime.toLocaleTimeString('zh-CN', { 
            hour12: false, 
            hour: '2-digit', 
            minute: '2-digit', 
            second: '2-digit',
            fractionalSecondDigits: 3
        }));
    }
    
    // 动态计算标签间隔，避免重叠
    const maxLabels = 15; // 最多显示15个时间标签
    const interval = Math.max(0, Math.floor(timeData.length / maxLabels));
    
    chartInstance.setOption({
        xAxis: { 
            type: 'category',
            data: timeData,
            axisLabel: {
                show: true,
                interval: interval,
                rotate: 45, // 旋转标签避免重叠
                fontSize: 10,
                margin: 12,
                formatter: function(value) {
                    // 只显示时:分:秒，不显示毫秒
                    return value.split('.')[0];
                }
            },
            axisTick: {
                show: true,
                interval: interval
            },
            name: '时间',
            nameLocation: 'middle',
            nameGap: 30
        },
        yAxis: { 
            type: 'value', 
            scale: true,
            axisLabel: {
                show: true,
                fontSize: 10
            },
            name: '数值',
            nameLocation: 'middle',
            nameGap: 40
        },
        series: [{ 
            type: 'line',
            data: data,
            smooth: true,
            symbol: 'none',
            lineStyle: { width: 2, color: '#3498db' },
            name: '实时数据'
        }],
        tooltip: {
            trigger: 'axis',
            formatter: function(params) {
                const data = params[0];
                const timeStr = timeData[data.dataIndex] || '未知时间';
                return `时间: ${timeStr}<br/>数值: ${data.value}`;
            }
        }
    });
    
    // 强制重新计算布局
    setTimeout(() => {
        if (chartInstance) {
            chartInstance.resize();
            // 非全屏时也强制重新计算，模拟全屏的效果
            setTimeout(() => {
                chartInstance.resize();
            }, 50);
        }
    }, 100);
}
// 全局缓冲区用于处理分片数据
let chartDataBuffer = [];
let lastProcessedTime = 0;
let dataIntegrityErrors = 0;
let expectedDataPattern = null; // 用于检测数据模式
let chartDataWorker = null; // 数据处理Worker
let chartRenderWorker = null; // 渲染Worker

// 初始化Worker
function initChartWorkers() {
    if (chartDataWorker || chartRenderWorker) return;
    
    // 数据Worker脚本 - 采用test.html的简单策略
    const dataWorkerScript = `
        let dataBuffer = [];
        let isRunning = false;
        let lastReportTime = 0;
        
        onmessage = function(e) {
            switch (e.data.type) {
                case 'start':
                    isRunning = true;
                    dataBuffer = [];
                    lastReportTime = performance.now();
                    break;
                    
                case 'stop':
                    isRunning = false;
                    break;
                    
                case 'serial_data':
                    if (isRunning) {
                        // 使用传入的时间戳，完全采用test.html的方式
                        e.data.values.forEach(val => {
                            dataBuffer.push({ timestamp: e.data.timestamp, value: val });
                        });
                        
                        // 每100个数据点或100ms发送一次（参考test.html）
                        if (dataBuffer.length >= 100 || performance.now() - lastReportTime > 100) {
                            postMessage({
                                type: 'data_batch',
                                data: dataBuffer.splice(0, dataBuffer.length)
                            });
                            lastReportTime = performance.now();
                        }
                    }
                    break;
            }
        };
    `;
    
    // 渲染Worker脚本 - 完全采用test.html的逻辑
    const renderWorkerScript = `
        let isRunning = false;
        let renderRate = 60;
        let maxDataPoints = 10000;
        let dataBuffer = [];
        let lastRenderTime = 0;
        let renderCount = 0;
        let startTime = 0;
        let totalDataPoints = 0;
        let timeWindow = 5;
        let historyPosition = 1.0;
        
        function processData() {
            if (!isRunning) return;
            
            const now = performance.now();
            if (now - lastRenderTime >= 1000 / renderRate) {
                const chartData = prepareChartData();
                postMessage({
                    type: 'render',
                    data: chartData,
                    stats: {
                        renderRate: renderCount / ((now - startTime) / 1000),
                        totalDataPoints: totalDataPoints,
                        dataBufferSize: dataBuffer.length
                    }
                });
                
                lastRenderTime = now;
                renderCount++;
            }
            
            if (isRunning) {
                setTimeout(processData, 1000 / renderRate);
            }
        }
        
        function prepareChartData() {
            if (dataBuffer.length === 0) return [];
            
            // 根据历史位置计算要显示的数据范围
            const totalPoints = dataBuffer.length;
            const startIndex = Math.floor(totalPoints * (1 - historyPosition));
            const endIndex = totalPoints;
            
            // 获取指定范围的数据
            const filteredData = dataBuffer.slice(startIndex, endIndex);
            
            // 返回数据，时间轴显示实际时间（秒）- 完全采用test.html的方式
            const result = filteredData.map(item => ({
                time: (item.timestamp - dataBuffer[0].timestamp) / 1000,
                value: item.value
            }));
            
            // 移除调试信息，时间轴已经正常工作
            
            return result;
        }
        
        function compressData() {
            // 限制数据点数量，避免内存占用过大
            if (dataBuffer.length > maxDataPoints) {
                const step = Math.ceil(dataBuffer.length / maxDataPoints);
                dataBuffer = dataBuffer.filter((_, index) => index % step === 0);
            }
        }
        
        function cleanupOldData() {
            const now = performance.now();
            // 保留最近60秒的数据，避免内存无限增长
            const cutoffTime = now - 60000;
            dataBuffer = dataBuffer.filter(item => item.timestamp >= cutoffTime);
        }
        
        onmessage = function(e) {
            switch (e.data.type) {
                case 'start':
                    isRunning = true;
                    renderRate = e.data.renderRate || 60;
                    maxDataPoints = e.data.maxDataPoints || 10000;
                    timeWindow = e.data.timeWindow || 5;
                    historyPosition = e.data.historyPosition || 1.0;
                    startTime = performance.now();
                    renderCount = 0;
                    totalDataPoints = 0;
                    dataBuffer = [];
                    lastRenderTime = 0;
                    processData();
                    break;
                    
                case 'stop':
                    isRunning = false;
                    break;
                    
                case 'data_batch':
                    dataBuffer.push(...e.data.data);
                    totalDataPoints += e.data.data.length;
                    compressData();
                    cleanupOldData();
                    break;
                    
                case 'config':
                    historyPosition = e.data.historyPosition || historyPosition;
                    break;
                    
                case 'clear':
                    dataBuffer = [];
                    totalDataPoints = 0;
                    break;
                    
                case 'get_stats':
                    postMessage({
                        type: 'stats',
                        stats: {
                            renderRate: renderCount / ((performance.now() - startTime) / 1000),
                            totalDataPoints: totalDataPoints,
                            dataBufferSize: dataBuffer.length
                        }
                    });
                    break;
            }
        };
    `;
    
    const dataWorkerBlob = new Blob([dataWorkerScript], { type: 'application/javascript' });
    const renderWorkerBlob = new Blob([renderWorkerScript], { type: 'application/javascript' });
    
    chartDataWorker = new Worker(URL.createObjectURL(dataWorkerBlob));
    chartRenderWorker = new Worker(URL.createObjectURL(renderWorkerBlob));
    
    // 设置Worker消息处理
    chartDataWorker.onmessage = (e) => {
        if (e.data.type === 'data_batch') {
            chartRenderWorker.postMessage({
                type: 'data_batch',
                data: e.data.data
            });
        }
    };
    
    chartRenderWorker.onmessage = (e) => {
        if (e.data.type === 'render') {
            updateChartFromWorker(e.data.data);
        }
    };
    
    console.log('[Worker] 图表Worker已初始化');
}

// 停止图表绘制
function stopChartDrawing() {
    chartDrawEnabled = false;
  window.chartDrawEnabled = chartDrawEnabled;
    
    // 停止Worker
    if (chartDataWorker) {
        chartDataWorker.postMessage({ type: 'stop' });
    }
    if (chartRenderWorker) {
        chartRenderWorker.postMessage({ type: 'stop' });
    }
    
    // 重置性能统计
    performanceStats = {
        dataPointsReceived: 0,
        renderCount: 0,
        lastRenderTime: 0,
        averageRenderTime: 0
    };
    
    // 重置数据质量统计
    dataQualityStats = {
        totalBytesReceived: 0,
        validDataPoints: 0,
        errorCount: 0,
        recoveryCount: 0,
        lastReportTime: 0
    };
    
    // 更新按钮状态
    const startBtn = document.getElementById('startChartBtn');
    if (startBtn) {
        startBtn.textContent = '开始绘制';
        startBtn.className = 'btn btn-success';
    }
    
    console.log('[性能保护] 已自动停止图表绘制');
}

// 性能监控变量
let performanceStats = {
    dataPointsReceived: 0,
    renderCount: 0,
    lastRenderTime: 0,
    averageRenderTime: 0
};

// 数据质量统计
let dataQualityStats = {
    totalBytesReceived: 0,
    validDataPoints: 0,
    errorCount: 0,
    recoveryCount: 0,
    lastReportTime: 0
};

// 从Worker更新图表
// 采用test.html的updateChart逻辑
function updateChartFromWorker(data) {
    if (!chartInstance || !chartDrawEnabled) return;
    
    const startTime = performance.now();
    
    // 采用test.html的数据格式：直接使用time和value
    const chartData = data.map(d => [d.time, d.value]);
    
    // 更新性能统计
    performanceStats.dataPointsReceived += data.length;
    performanceStats.renderCount++;
    const renderTime = performance.now() - startTime;
    performanceStats.averageRenderTime = (performanceStats.averageRenderTime * (performanceStats.renderCount - 1) + renderTime) / performanceStats.renderCount;
    
    // 每10次渲染输出一次性能统计
    //if (performanceStats.renderCount % 10 === 0) {
        //console.log(`[性能统计] 数据点: ${performanceStats.dataPointsReceived}, 渲染次数: ${performanceStats.renderCount}, 平均渲染时间: ${performanceStats.averageRenderTime.toFixed(2)}ms`);
    //}
    
    // 性能保护：如果渲染时间过长，自动停止
    if (renderTime > 100) {
        console.warn(`[性能警告] 渲染时间过长: ${renderTime.toFixed(2)}ms，自动停止绘制`);
        stopChartDrawing();
        return;
    }
    
    // 自适应性能调节
    if (renderTime > 50 && performanceStats.renderCount > 20) {
        console.warn(`[性能调节] 渲染时间较长: ${renderTime.toFixed(2)}ms，建议降低采样率或关闭完整性检查`);
    }
    
    // 完全采用test.html的updateChart逻辑
    const chartDataArray = data.map(d => [d.time, d.value]);
    
    const option = {
        series: [{
            data: chartDataArray
        }]
    };
    
    // 采用新工程的x轴配置方式，不设置min/max，让ECharts自动计算合适的范围
    if (data.length > 0) {
        option.xAxis = {
            type: 'value',
            name: '时间 (秒)',
            nameLocation: 'middle',
            nameGap: 30
        };
    }
    
    chartInstance.setOption(option);
}

function handleRealtimeHexDataChunked(data) {
    if (!chartDrawEnabled) return;
    if (!(data instanceof Uint8Array)) return;
    
    const currentTime = Date.now();
    
    // 更新数据质量统计
    dataQualityStats.totalBytesReceived += data.length;
    
    // 采用test.html的数组方式处理数据
    // 将Uint8Array转换为数组，便于使用push和splice
    if (!Array.isArray(chartDataBuffer)) {
        chartDataBuffer = Array.from(chartDataBuffer);
    }
    
    // 采用test.html的数据转换方式：逐个字节转换
    const values = [];
    for (let i = 0; i < data.length; i++) {
        values.push(data[i]);
    }
    
    // 将新数据添加到缓冲区
    chartDataBuffer.push(...values);
    
    // 调试输出：显示拼接后的数据
    if (chartDataBuffer.length > 0) {
        const firstBytes = chartDataBuffer.slice(0, Math.min(8, chartDataBuffer.length))
            .map(b => b.toString(16).padStart(2, '0')).join(' ');
        console.log(`[数据拼接] 缓冲区大小: ${chartDataBuffer.length}, 前8字节: ${firstBytes}`);
        
        // 简化调试输出，只在调试模式下显示详细信息
        const debugCheckbox = document.getElementById('debugMode');
        const enableDebug = debugCheckbox && debugCheckbox.checked;
        
        if (enableDebug) {
            // 如果剩余字节不是4的倍数，记录警告
            if (chartDataBuffer.length % 4 !== 0) {
                console.warn(`[边界警告] 缓冲区大小 ${chartDataBuffer.length} 不是4的倍数`);
            }
        }
    }
    
    console.log(`[曲线缓冲] 缓冲区大小: ${chartDataBuffer.length} 字节`);
    
    // 采用test.html的即时处理策略：每次接收到数据就立即处理
    // 处理完整的4字节组
    while (chartDataBuffer.length >= 4) {
        // 采用test.html的简单策略：不做任何数据验证，直接处理所有数据
        const value = (chartDataBuffer[0] | (chartDataBuffer[1]<<8) | (chartDataBuffer[2]<<16) | (chartDataBuffer[3]<<24)) >>> 0;
        const seg = [chartDataBuffer[0], chartDataBuffer[1], chartDataBuffer[2], chartDataBuffer[3]];
        
        // 发送数据到Worker
        if (chartDataWorker) {
            chartDataWorker.postMessage({
                type: 'serial_data',
                values: [value]
            });
        }
        
        // 在控制台打印详细信息
        console.log(`[曲线解析] ✅ 4字节: ${seg.map(x=>x.toString(16).padStart(2,'0')).join(' ')} -> 小端解析: 0x${value.toString(16).padStart(8,'0')} (${value})`);
        
        // 更新最后处理时间
        lastProcessedTime = currentTime;
        
        // 重置错误计数
        dataIntegrityErrors = 0;
        
        // 更新统计
        dataQualityStats.validDataPoints++;
        
        // 移除已处理的4字节（参考test.html的splice方法）
        chartDataBuffer.splice(0, 4);
    }
        
        // 移除已处理的4字节（参考test.html的splice方法）
        chartDataBuffer.splice(0, 4);
    }
    
    // 检查缓冲区是否积压过多（可能数据丢失）
    if (chartDataBuffer.length > 20) {
        console.warn(`[曲线警告] 缓冲区积压过多 (${chartDataBuffer.length} 字节)，可能数据丢失`);
        chartDataBuffer = [];
        dataIntegrityErrors++;
    }
    
    // 定期输出数据质量统计
    const now = performance.now();
    if (now - dataQualityStats.lastReportTime > 5000) { // 每5秒输出一次
        const errorRate = dataQualityStats.totalBytesReceived > 0 ? 
            (dataQualityStats.errorCount / dataQualityStats.totalBytesReceived * 100).toFixed(2) : '0.00';
        const recoveryRate = dataQualityStats.errorCount > 0 ? 
            (dataQualityStats.recoveryCount / dataQualityStats.errorCount * 100).toFixed(2) : '0.00';
        
        console.log(`[数据质量统计] 总字节: ${dataQualityStats.totalBytesReceived}, 有效数据点: ${dataQualityStats.validDataPoints}, 错误: ${dataQualityStats.errorCount}, 恢复: ${dataQualityStats.recoveryCount}, 错误率: ${errorRate}%, 恢复率: ${recoveryRate}%`);
        
        dataQualityStats.lastReportTime = now;
    }
    
    console.log(`[曲线处理] 完成处理，剩余缓冲区: ${chartDataBuffer.length} 字节，错误次数: ${dataIntegrityErrors}`);


// 智能数据恢复函数
function attemptSmartRecovery() {
    if (chartDataBuffer.length < 8) {
        return false; // 缓冲区数据不足
    }
    
    console.log(`[数据恢复] 尝试智能恢复，缓冲区大小: ${chartDataBuffer.length}`);
    
    // 尝试不同的偏移量
    for (let offset = 1; offset <= 4 && chartDataBuffer.length >= 4 + offset; offset++) {
        const testBytes = [
            chartDataBuffer[offset], 
            chartDataBuffer[offset+1], 
            chartDataBuffer[offset+2], 
            chartDataBuffer[offset+3]
        ];
        const testValue = (testBytes[0] | (testBytes[1]<<8) | (testBytes[2]<<16) | (testBytes[3]<<24)) >>> 0;
        
        // 检查这个偏移量是否产生合理的数据
        if (testValue > 0 && testValue <= 1000000) {
            console.log(`[数据恢复] 找到有效偏移量: ${offset}，新值: ${testValue}`);
            chartDataBuffer = chartDataBuffer.slice(offset);
            dataQualityStats.recoveryCount++;
            return true;
        }
    }
    
    // 如果找不到有效偏移量，丢弃前4字节
    console.log(`[数据恢复] 未找到有效偏移量，丢弃前4字节`);
    chartDataBuffer = chartDataBuffer.slice(4);
    return false;
}

// 数据完整性检查函数
function checkDataIntegrity(bytes, value) {
    // 检查1: 数值合理性（不能为0或过大）
    if (value === 0) {
        console.warn(`[完整性检查] 数值为0，可能数据丢失`);
        return false;
    }
    
    if (value > 0x7FFFFFFF) {
        console.warn(`[完整性检查] 数值过大 (${value})，可能数据错位`);
        return false;
    }
    
    // 检查2: 时间间隔合理性（放宽限制）
    const currentTime = Date.now();
    if (lastProcessedTime > 0 && (currentTime - lastProcessedTime) > 30000) { // 改为30秒
        console.warn(`[完整性检查] 数据间隔过长 (${currentTime - lastProcessedTime}ms)，可能数据丢失`);
        return false;
    }
    
    // 检查3: 数据模式一致性（只在有足够数据且模式稳定时检查）
    if (expectedDataPattern !== null && window.chartData && window.chartData.length > 10) {
        const patternMatch = checkDataPattern(bytes);
        if (!patternMatch) {
            console.warn(`[完整性检查] 数据模式不匹配，可能数据错位`);
            return false;
        }
    }
    
    // 建立数据模式（前几个数据点）
    if (window.chartData && window.chartData.length < 3) {
        establishDataPattern(bytes);
    }
    
    return true;
}

// 建立数据模式
function establishDataPattern(bytes) {
    if (expectedDataPattern === null) {
        expectedDataPattern = {
            firstByte: bytes[0],
            secondByte: bytes[1],
            pattern: []
        };
        console.log(`[模式建立] 建立数据模式: 首字节=${bytes[0].toString(16).padStart(2,'0')}, 次字节=${bytes[1].toString(16).padStart(2,'0')}`);
    }
}

// 检查数据模式
function checkDataPattern(bytes) {
    if (expectedDataPattern === null) return true;
    
    // 放宽模式检查：只要前两个字节不是完全相同的固定值就认为有效
    // 这样可以适应数据变化的情况
    if (bytes[0] !== bytes[1]) {
        return true;
    }
    
    // 如果前两个字节相同，可能是固定模式，需要进一步检查
    if (bytes[0] === expectedDataPattern.firstByte && bytes[1] === expectedDataPattern.secondByte) {
        return true;
    }
    
    return false;
}

// 尝试数据恢复
function attemptDataRecovery() {
    console.log(`[数据恢复] 尝试在缓冲区中寻找有效数据边界...`);
    
    // 寻找可能的4字节边界
    for (let i = 1; i < chartDataBuffer.length - 3; i++) {
        const testBytes = [chartDataBuffer[i], chartDataBuffer[i+1], chartDataBuffer[i+2], chartDataBuffer[i+3]];
        const testValue = (testBytes[0] | (testBytes[1]<<8) | (testBytes[2]<<16) | (testBytes[3]<<24)) >>> 0;
        
        // 检查这个位置是否可能是有效数据
        if (testValue > 0 && testValue <= 0x7FFFFFFF) {
            console.log(`[数据恢复] 在位置 ${i} 找到可能的有效数据边界`);
            chartDataBuffer = chartDataBuffer.slice(i);
            return true;
        }
    }
    
    return false;
}

// 旧的handleRealtimeHexData函数已删除，现在使用Worker方式处理数据
// ... existing code ...
function setupTerminalHexChartSync() {
    // 这个函数现在不再需要，因为数据直接在handleReceivedData中处理
    // 保留函数以避免调用错误，但不执行任何操作
    console.log('[曲线同步] 已禁用终端监控同步，数据直接通过串口处理');
}
// ... existing code ...

// ... existing code ...
// 全屏样式
(function(){
    const style = document.createElement('style');
    style.innerHTML += `\n.realtime-chart-fullscreen {\n    position: fixed !important;\n    top: 0; left: 0; right: 0; bottom: 0;\n    z-index: 9999;\n    background: #fff;\n    margin: 0 !important;\n    padding: 20px !important;\n    border-radius: 0 !important;\n    width: 100vw !important;\n    height: 100vh !important;\n    box-shadow: 0 0 0 9999px rgba(0,0,0,0.15);\n    display: flex;\n    flex-direction: column;\n    align-items: stretch;\n    justify-content: flex-start;\n}\n.realtime-chart-fullscreen #realtimeChart {\n    flex: 1;\n    height: auto !important;\n    min-height: 0 !important;\n}`;
    document.head.appendChild(style);
})();
// ... existing code ...

// ... existing code ...



// ... existing code ...

// ... existing code ...
// 页面加载时初始化曲线图
if (document.readyState === 'complete' || document.readyState === 'interactive') {
    setTimeout(setupRealtimeChart, 300);
} else {
    document.addEventListener('DOMContentLoaded', setupRealtimeChart);
}

// 立即绑定终止绘制按钮事件，确保按钮可用
function bindStopChartButton() {
    const stopChartBtn = document.getElementById('stopChartBtn');
    if (stopChartBtn) {
        stopChartBtn.onclick = function() {
            console.log('[终止绘制] 按钮被点击');
            
            // 1. 清空曲线数据
            window.chartData = [];
            window.chartTimeData = []; // 清除时间数据
            window.chartDataBuffer = []; // 清除缓冲区
            
            // 2. 停止曲线绘制
            if (window.chartDrawEnabled) {
                window.chartDrawEnabled = false;
                
                // 更新按钮状态
                const startBtn = document.getElementById('startChartBtn');
                if (startBtn) {
                    startBtn.textContent = '开始绘制';
                    startBtn.className = 'btn btn-success';
                }
                
                // 停止Worker
                if (window.chartDataWorker) {
                    window.chartDataWorker.postMessage({ type: 'stop' });
                }
                if (window.chartRenderWorker) {
                    window.chartRenderWorker.postMessage({ type: 'stop' });
                }
                
                console.log('[终止绘制] 曲线绘制已停止');
            }
            
            // 3. 清空多变量图表
            if (window.multiChartManager) {
                window.multiChartManager.clearAllCharts();
            }
            
            // 4. 清空图表显示
            if (window.chartInstance) {
                window.chartInstance.setOption({
                    series: [{
                        data: []
                    }]
                });
            }
            
            // 5. 发送终止绘制命令（如果串口已连接）
            const stopCommand = 'cmd.read_ram(0x20000000,1,0)';
            if (window.microLinkTerminal && window.microLinkTerminal.isConnected) {
                window.microLinkTerminal.sendCommand(stopCommand);
                console.log('[终止绘制] 已发送停止命令:', stopCommand);
            } else {
                console.log('[终止绘制] 串口未连接，跳过发送命令');
            }
            
            // 6. 关闭监听框的HEX模式
            const hexModeCheckbox = document.getElementById('hexMode');
            if (hexModeCheckbox && hexModeCheckbox.checked) {
                hexModeCheckbox.checked = false;
                hexModeCheckbox.dispatchEvent(new Event('change'));
                console.log('[终止绘制] 已关闭HEX模式');
            }
            
            console.log('[终止绘制] 操作完成 - 已清空曲线并停止绘制');
        };
        console.log('[终止绘制] 按钮事件已绑定');
    } else {
        console.log('[终止绘制] 按钮未找到，将在setupRealtimeChart中绑定');
    }
}

// 立即尝试绑定按钮
bindStopChartButton();

// 如果DOM还没准备好，等待后再次尝试
if (document.readyState !== 'complete') {
    document.addEventListener('DOMContentLoaded', bindStopChartButton);
}
// 切换到变量分析tab时也初始化曲线图，防止切换后全局变量丢失
function setupVarPanelRealtimeChartInit() {
    setTimeout(setupRealtimeChart, 200);
}
const varPanelBtn = document.querySelector('.sidebar-btn[data-panel="varPanel"]');
if (varPanelBtn) {
    varPanelBtn.addEventListener('click', setupVarPanelRealtimeChartInit);
}


// ... existing code ...

// ... existing code ...
// 删除旧的串口发送区逻辑，因为HTML结构已经改变
// ... existing code ...

// ... existing code ...
    // 删除重复的开始绘制按钮逻辑，已在setupRealtimeChart中处理
// ... existing code ...

// 获取主终端输出区域
function appendToTerminalOutput(html) {
    const terminalOutput = document.getElementById('terminalOutput');
    if (terminalOutput) {
        terminalOutput.insertAdjacentHTML('beforeend', html);
        terminalOutput.scrollTop = terminalOutput.scrollHeight;
    }
}

// 删除重复的全局日志函数定义

// test.html的简化数据处理逻辑
function parseSerialData(buffer) {
    const view = new Uint8Array(buffer);
    const values = [];
    // 每个字节作为一个独立的十进制值
    for (let i = 0; i < view.length; i++) {
        values.push(view[i]);
    }
    return values;
}

function processSerialBuffer() {
    // 如果缓冲区有足够的数据，按4字节一组处理
    while (chartDataBuffer.length >= 4) {
        const group = chartDataBuffer.splice(0, 4);
        console.log('处理4字节组:', group);
        
        // 将4字节组合成32位整数 (小端序 - 从后往前读)
        const value = group[0] | (group[1] << 8) | (group[2] << 16) | (group[3] << 24);
        console.log('32位整数值 (小端序):', value);
        
        // 发送数据到Worker - 添加时间戳，完全采用test.html的方式
        if (chartDataWorker) {
            const timestamp = performance.now();
            chartDataWorker.postMessage({
                type: 'serial_data',
                values: [value],
                timestamp: timestamp
            });
        }
    }
}

// 新的简化版本，支持AA和55数据帧头帧尾解析
function handleRealtimeHexDataChunkedSimple(data) {
    if (!chartDrawEnabled) return;
    if (!(data instanceof Uint8Array)) return;
    
    // 将新数据添加到缓冲区
    if (!Array.isArray(chartDataBuffer)) {
        chartDataBuffer = [];
    }
    chartDataBuffer.push(...Array.from(data));
    
    // console.log('接收到串口数据块:', data.length, '字节, 缓冲区大小:', chartDataBuffer.length);
    
    // 查找并解析AA和55之间的数据帧
    parseDataFrames();
}

// 解析AA和55之间的数据帧
function parseDataFrames() {
    const frameStart = 0xAA;  // 帧头
    const frameEnd = 0x55;    // 帧尾
    
    while (chartDataBuffer.length >= 2) {
        // 查找帧头AA
        const startIndex = chartDataBuffer.indexOf(frameStart);
        if (startIndex === -1) {
            // 没有找到帧头，清空缓冲区
            chartDataBuffer.splice(0, chartDataBuffer.length);
            return;
        }
        
        // 查找帧尾55（从帧头之后开始查找）
        const endIndex = chartDataBuffer.indexOf(frameEnd, startIndex + 1);
        if (endIndex === -1) {
            // 没有找到帧尾，保留从帧头开始的数据，等待更多数据
            chartDataBuffer.splice(0, startIndex);
            return;
        }
        
        // 提取帧头帧尾之间的数据
        const frameData = chartDataBuffer.slice(startIndex + 1, endIndex);
        // console.log('找到数据帧:', {
        //     start: startIndex,
        //     end: endIndex,
        //     frameLength: frameData.length,
        //     frameData: Array.from(frameData).map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase())
        // });
        
        // 处理提取的数据帧
        processDataFrame(frameData);
        
        // 移除已处理的数据（包括帧头、帧尾和中间数据）
        chartDataBuffer.splice(0, endIndex + 1);
    }
}

// 处理提取的数据帧
function processDataFrame(frameData) {
    if (frameData.length === 0) return;
    
    // console.log('处理数据帧:', frameData.length, '字节:', Array.from(frameData).map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase()));
    
            // 检查是否有选中的变量
        if (!window.selectedVariables || window.selectedVariables.size === 0) {
            console.log('没有选中的变量，跳过数据处理');
            return;
        }
        
        // 获取选中的变量列表
        const selectedVars = Array.from(window.selectedVariables);
        const maxVars = Math.min(selectedVars.length, 9); // 最多9个变量
        
        console.log(`[数据处理] 开始处理数据帧:`, {
            '帧长度': frameData.length,
            '选中变量数量': selectedVars.length,
            '最大处理数量': maxVars,
            '选中变量': selectedVars,
            '数据内容': Array.from(frameData).map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase())
        });
    
    // 根据变量大小动态解析数据
    let dataIndex = 0;
    let frameIndex = 0;
    
    // 特殊处理：如果数据帧很小，调整处理策略
    if (frameData.length === 1) {
        console.log(`[数据处理] 检测到1字节数据帧，启用特殊处理模式`);
        // 对于1字节数据帧，强制第一个变量为1字节
        if (selectedVars.length > 0) {
            const varName = selectedVars[0];
            console.log(`[数据处理] 1字节模式：将变量 ${varName} 设置为1字节`);
        }
    }
    
    while (frameIndex < frameData.length && dataIndex < maxVars) {
        const varName = selectedVars[dataIndex];
        
        // 获取变量的字节大小 - 优先使用选择的变量实际大小
        let varSize = 4; // 默认4字节
        
        // 优先从变量信息中获取实际大小
        if (window.variableInfo && window.variableInfo[varName]) {
            varSize = window.variableInfo[varName].size || 4;
            console.log(`[数据处理] 使用变量 ${varName} 的实际大小: ${varSize}字节`);
        } else {
            // 如果无法获取变量大小，尝试从命令中推断
            const remainingBytes = frameData.length - frameIndex;
            
            // 特殊处理：如果数据帧只有1字节且是第一个变量，强制使用1字节
            if (frameData.length === 1 && dataIndex === 0) {
                varSize = 1;
                console.log(`[数据处理] 1字节数据帧模式：强制变量 ${varName} 为1字节`);
            } else if (remainingBytes >= 4) {
                // 数据充足，可以使用4字节
                varSize = 4;
            } else if (remainingBytes >= 2) {
                // 数据足够2字节
                varSize = 2;
            } else if (remainingBytes >= 1) {
                // 只有1字节数据，强制使用1字节
                varSize = 1;
                console.log(`[数据处理] 数据不足，将变量 ${varName} 调整为1字节大小`);
            } else {
                // 没有数据了
                console.log(`[数据处理] 没有剩余数据，跳过变量 ${varName}`);
                break;
            }
            
            console.log(`[数据处理] 智能推断变量 ${varName} 大小: ${varSize}字节 (剩余数据: ${remainingBytes}字节)`);
        }
        
        // 检查剩余数据是否足够（这个检查现在由智能推断逻辑处理）
        // 如果智能推断后仍然不足，记录警告但继续处理
        if (frameData.length - frameIndex < varSize) {
            console.warn(`[数据处理] 警告：变量 ${varName} 需要 ${varSize} 字节，但剩余 ${frameData.length - frameIndex} 字节，尝试调整大小`);
            // 强制调整为可用大小
            varSize = Math.min(varSize, frameData.length - frameIndex);
            if (varSize <= 0) {
                console.log(`[数据处理] 无法调整大小，跳过变量 ${varName}`);
                break;
            }
            console.log(`[数据处理] 已将变量 ${varName} 大小调整为 ${varSize} 字节`);
        }
        
        // 提取指定大小的数据
        const group = frameData.slice(frameIndex, frameIndex + varSize);
        frameIndex += varSize;
        
        // 根据大小解析数据 - 使用无符号整数(uint)处理
        let value;
        if (varSize === 1) {
            value = group[0]; // 直接使用无符号8位整数 (0-255)
        } else if (varSize === 2) {
            value = group[0] | (group[1] << 8); // 小端序16位无符号整数 (0-65535)
        } else if (varSize === 4) {
            value = group[0] | (group[1] << 8) | (group[2] << 16) | (group[3] << 24); // 小端序32位无符号整数 (0-4294967295)
        } else {
            // 对于其他大小，转换为无符号数值
            value = parseInt(Array.from(group).map(b => b.toString(16).padStart(2, '0')).join(''), 16);
        }
        
        console.log(`[数据处理] 变量 ${varName} (${varSize}字节):`, value, '原始字节:', group.map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase()));
        
        // 特殊显示1字节变量的信息
        if (varSize === 1) {
            console.log(`[数据处理] 1字节变量 ${varName} 处理完成:`, {
                '原始值': group[0],
                '无符号值': value,
                '范围': `${value >= 0 && value <= 255 ? '有效' : '超出范围'}`,
                '二进制': '0b' + group[0].toString(2).padStart(8, '0')
            });
        }
        
        // 发送数据到多变量图表管理器
        if (window.multiChartManager && window.chartDrawEnabled) {
            // 检查图表是否存在，如果不存在则跳过
            if (window.multiChartManager.hasChart(varName)) {
                console.log(`[数据处理] 更新图表 - 变量: ${varName}, 值: ${value}`);
                window.multiChartManager.updateChartData(varName, [value]);
            } else {
                console.log(`[数据处理] 跳过图表更新 - 变量 ${varName} 的图表不存在，尝试创建图表`);
                // 尝试创建图表
                window.multiChartManager.createChart(varName);
                // 延迟更新数据
                setTimeout(() => {
                    if (window.multiChartManager.hasChart(varName)) {
                        window.multiChartManager.updateChartData(varName, [value]);
                    }
                }, 100);
            }
        }
        
        dataIndex++;
    }
    
    // 如果剩余数据不足4字节，记录日志
    if (frameData.length > 0) {
        // console.log('数据帧剩余不足4字节的数据:', frameData.length, '字节:', Array.from(frameData).map(b => '0x' + b.toString(16).padStart(2, '0').toUpperCase()));
    }
}

// 多变量图表管理器
class MultiChartManager {
    constructor() {
        this.container = null;
        this.charts = new Map(); // 存储变量名到图表实例的映射
        this.chartData = new Map(); // 存储变量名到数据的映射
        this.chartTimeData = new Map(); // 存储变量名到时间数据的映射
        this.varTimestamps = new Map(); // 存储每个变量的时间戳序列
        this.defaultChart = null;
        this.samplingRate = 50; // 默认采样率50Hz
        this.timeDisplayRange = 10; // 默认时间显示范围10秒
        this.debugMode = false; // 调试模式开关，默认关闭
        this.lastUpdateTime = new Map(); // 每个变量的上次更新时间，用于节流绘制
        console.log(`[MultiChartManager] 初始化完成 - 默认采样率: ${this.samplingRate}Hz, 显示范围: ${this.timeDisplayRange}秒`);
        
        // 延迟初始化，等待DOM元素创建完成
        this.init();
    }
    
    init() {
        this.container = document.getElementById('multiChartContainer');
        this.defaultChart = document.getElementById('realtimeChart');
        
        if (!this.container) {
            console.error('多变量图表管理器容器未找到，将在100ms后重试');
            setTimeout(() => this.init(), 100);
            return;
        }
        
        console.log('多变量图表管理器已初始化');
    }
    
    /**
     * 为指定变量创建图表
     */
    createChart(varName) {
        console.log(`[MultiChartManager] 尝试创建图表: ${varName}`);
        
        if (this.charts.has(varName)) {
            console.log(`[MultiChartManager] 变量 ${varName} 的图表已存在，跳过创建`);
            return;
        }
        
        // 检查容器是否准备好
        if (!this.container) {
            console.log(`[MultiChartManager] 容器未准备好，延迟创建图表: ${varName}`);
            setTimeout(() => this.createChart(varName), 100);
            return;
        }
        
        console.log(`[MultiChartManager] 开始创建图表: ${varName}, 容器状态:`, !!this.container);
        
        // 隐藏默认图表
        if (this.defaultChart) {
            this.defaultChart.style.display = 'none';
        }
        
        // 创建图表容器
        const chartDiv = document.createElement('div');
        chartDiv.className = 'variable-chart';
        chartDiv.setAttribute('data-var', varName);
        chartDiv.innerHTML = `
            <div class="variable-chart-header">
                <div class="variable-chart-title">${varName}</div>
                <div class="variable-chart-controls">
                    <button class="btn btn-sm btn-outline-info" onclick="window.multiChartManager.fullscreenChart('${varName}')" title="全屏显示">
                        <span style="font-size:12px;">⛶</span>
                    </button>
                </div>
            </div>
            <div class="variable-chart-content" style="width:100%;height:400px;"></div>
        `;
        
        // 调试：显示图表容器信息
        console.log(`[MultiChartManager] 创建图表容器 - 变量 ${varName}:`, {
            '容器类名': chartDiv.className,
            '容器属性': chartDiv.getAttribute('data-var'),
            '容器HTML': chartDiv.innerHTML.substring(0, 100) + '...',
            '容器高度': chartDiv.querySelector('.variable-chart-content').style.height
        });
        
        // 确保容器存在且可访问
        if (!this.container || !this.container.appendChild) {
            console.error(`[MultiChartManager] 容器不可用，无法创建图表 - 变量 ${varName}`);
            return;
        }
        
        this.container.appendChild(chartDiv);
        
        // 初始化ECharts实例
        const chartDom = chartDiv.querySelector('.variable-chart-content');
        let chartInstance;
        
        try {
            chartInstance = echarts.init(chartDom);
            console.log(`[MultiChartManager] ECharts实例创建成功 - 变量 ${varName}`);
        } catch (error) {
            console.error(`[MultiChartManager] ECharts实例创建失败 - 变量 ${varName}:`, error);
            // 移除失败的DOM元素
            chartDiv.remove();
            return;
        }
        
        // 设置图表配置，与默认图表保持一致
        try {
            chartInstance.setOption({
                xAxis: { 
                    type: 'value',
                    name: '时间 (秒)',
                    nameLocation: 'middle',
                    nameGap: 30,
                    min: 0,
                    max: this.timeDisplayRange, // 使用动态监测时间
                    axisLabel: {
                        formatter: function(value) {
                            return value.toFixed(1) + 's';
                        }
                    }
                },
                yAxis: { 
                    type: 'value', 
                    scale: true,
                    name: '数值',
                    nameLocation: 'middle',
                    nameGap: 40,
                    min: -130,
                    max: 130,
                    axisLabel: {
                        formatter: function(value) {
                            // 如果是1字节范围，显示整数
                            if (value >= -128 && value <= 127) {
                                return Math.round(value);
                            }
                            return value.toFixed(1);
                        }
                    }
                },
                series: [{ 
                    type: 'line', 
                    data: [], 
                    smooth: true, 
                    symbol: 'none', 
                    lineStyle: { width: 2, color: '#3498db' }, 
                    name: '实时数据'
                }],
                grid: { 
                    left: 80, 
                    right: 40, 
                    top: 50, 
                    bottom: 80,
                    containLabel: true
                },
                animation: false,
                tooltip: {
                    trigger: 'axis',
                    formatter: function(params) {
                        const data = params[0];
                        const timeStr = params[0].axisValue || '未知时间';
                        // 确保数值正确显示
                        let valueStr;
                        if (Array.isArray(data.value)) {
                            // 如果data.value是数组[time, value]，取第二个元素作为数值
                            valueStr = data.value[1];
                        } else {
                            // 如果data.value是单个数值
                            valueStr = data.value;
                        }
                        return `时间: ${timeStr}<br/>数值: ${valueStr}`;
                    }
                }
            });
        } catch (error) {
            console.error(`[MultiChartManager] 图表配置设置失败 - 变量 ${varName}:`, error);
            // 销毁失败的实例并移除DOM
            chartInstance.dispose();
            chartDiv.remove();
            return;
        }
        
        // 存储图表实例和数据
        this.charts.set(varName, chartInstance);
        this.chartData.set(varName, []);
        
        // 初始化时间戳数组
        this.varTimestamps.set(varName, []);
        
        // 初始化该变量的更新时间戳
        this.lastUpdateTime.set(varName, 0);
        
        // 为Y轴范围计算提供chartData引用
        chartInstance.chartData = this.chartData.get(varName);
        
        // 强制resize
        setTimeout(() => {
            chartInstance.resize();
        }, 100);
        
        console.log(`[MultiChartManager] 为变量 ${varName} 创建了图表:`, {
            '当前图表数量': this.charts.size,
            '图表实例': !!chartInstance,
            '图表DOM': !!chartDom,
            '数据数组': this.chartData.get(varName),
            '时间戳数组': this.varTimestamps.get(varName),
            '容器子元素数量': this.container.children.length,
            '图表容器可见性': chartDiv.style.display,
            '图表内容高度': chartDom.style.height,
            '图表容器位置': chartDiv.offsetTop + 'px'
        });
    }
    
    /**
     * 检查指定变量是否有图表
     */
    hasChart(varName) {
        const hasChart = this.charts.has(varName);
        const chartInstance = this.charts.get(varName);
        const chartData = this.chartData.get(varName);
        
        console.log(`[MultiChartManager] 检查图表 ${varName}:`, {
            'hasChart': hasChart,
            'chartInstance': !!chartInstance,
            'chartData': !!chartData,
            '数据长度': chartData ? chartData.length : 0
        });
        
        return hasChart;
    }
    
    /**
     * 更新指定变量的图表数据
     */
    updateChartData(varName, newData) {
        // 检查容器是否准备好
        if (!this.container) {
            console.log(`[MultiChartManager] 容器未准备好，延迟更新数据: ${varName}`);
            setTimeout(() => this.updateChartData(varName, newData, timestamp), 100);
            return;
        }
        
        const chartInstance = this.charts.get(varName);
        const chartData = this.chartData.get(varName);
        
        if (!chartInstance || !chartData) {
            // console.log(`[MultiChartManager] 变量 ${varName} 的图表或数据未找到 - chartInstance:`, !!chartInstance, 'chartData:', !!chartData);
            return;
        }
        
        // console.log(`[MultiChartManager] 更新变量 ${varName} 的数据:`, newData, '当前数据长度:', chartData.length);
        
        // 环形缓冲区：固定大小，自动覆盖旧数据
        const bufferSize = 6000;
        
        // 添加新数据到数组
        chartData.push(...newData);
        
        // 如果数据超过缓冲区大小，自动移除最旧的数据
        if (chartData.length > bufferSize) {
            const removeCount = chartData.length - bufferSize;
            chartData.splice(0, removeCount);
        }
        
        // 更新图表实例的chartData引用，确保Y轴范围计算能正常工作
        chartInstance.chartData = chartData;
        
        // 调试：显示数据累积状态
        console.log(`[MultiChartManager] 数据累积 - 变量 ${varName}:`, {
            '新数据': newData,
            '累积后长度': chartData.length,
            '缓冲区大小': bufferSize,
            '数据示例': chartData.slice(-3) // 显示最后3个数据点
        });
        

        
        // 计算时间数据，使用真实的时间戳
        let timeData;
        
        // 为每个数据点生成时间戳（如果没有时间戳数组，则创建）
        if (!this.varTimestamps.has(varName)) {
            this.varTimestamps.set(varName, []);
        }
        
        const timestamps = this.varTimestamps.get(varName);
        
        // 为每个新数据点添加时间戳
        for (let i = 0; i < newData.length; i++) {
            const currentTime = performance.now();
            timestamps.push(currentTime);
        }
        
        // 限制时间戳数量，与数据保持一致
        if (timestamps.length > bufferSize) {
            const removeCount = timestamps.length - bufferSize;
            timestamps.splice(0, removeCount);
        }
        
        // 计算相对时间（只显示可视区间内的数据）
        if (timestamps.length > 0) {
            const currentTime = performance.now();
            const visibleStartTime = currentTime - (this.timeDisplayRange * 1000); // 可视区间的开始时间
            
            // 只保留可视区间内的数据点
            const visibleTimestamps = timestamps.filter(ts => ts >= visibleStartTime);
            const visibleChartData = chartData.slice(-visibleTimestamps.length);
            
            // 计算相对时间（从可视区间开始时间开始）
            timeData = visibleTimestamps.map(ts => (ts - visibleStartTime) / 1000);
            
            // 更新图表数据为可视区间内的数据
            chartData.length = 0;
            chartData.push(...visibleChartData);
            
            // 更新时间戳为可视区间内的时间戳
            timestamps.length = 0;
            timestamps.push(...visibleTimestamps);
        } else {
            timeData = [];
        }
        
        // 调试信息：显示时间数据（限制频率）
        // if (timeData.length > 1 && timeData.length % 50 === 0) {
        //     const lastTwoTime = timeData.slice(-2);
        //     console.log(`[MultiChartManager] 变量 ${varName} 时间数据:`, 
        //         `前一个: ${lastTwoTime[0].toFixed(3)}s, 当前: ${lastTwoTime[1].toFixed(3)}s, 间隔: ${(lastTwoTime[1] - lastTwoTime[0]).toFixed(3)}s`);
        // }
        
        // 调试信息：显示滑动窗口信息（限制频率）
        // if (timestamps.length > 1000 && timestamps.length % 100 === 0) {
        //     const windowStart = timestamps[0];
        //     const windowEnd = timestamps[timestamps.length - 1];
        //     const windowDuration = (windowEnd - windowStart) / 1000;
        //     console.log(`[MultiChartManager] 变量 ${varName} 滑动窗口:`, 
        //         `起始: ${(windowStart / 1000).toFixed(3)}s, 结束: ${(windowEnd / 1000).toFixed(3)}s, 窗口长度: ${windowDuration.toFixed(3)}s`);
        // }
        
        // 更新图表 - 使用批量更新和节流绘制
        // 确保数据格式正确，避免白屏
        if (!timeData || timeData.length === 0 || !chartData || chartData.length === 0) {
            console.warn(`[MultiChartManager] 数据不完整，跳过图表更新 - 变量 ${varName}:`, {
                'timeData长度': timeData ? timeData.length : 0,
                'chartData长度': chartData ? chartData.length : 0
            });
            return;
        }
        
        const chartDataArray = chartData.map((value, index) => [timeData[index], value]);
        
        // 调试：显示数据格式
        console.log(`[MultiChartManager] 图表数据准备 - 变量 ${varName}:`, {
            '数据点数量': timeData.length,
            '数据数组长度': chartData.length,
            '时间数据长度': timeData.length,
            '图表数据数组': chartDataArray.slice(-3), // 显示最后3个数据点
            '时间范围': timeData.length > 0 ? `${Math.min(...timeData).toFixed(3)}s - ${Math.max(...timeData).toFixed(3)}s` : '无'
        });
        
        // 节流绘制：限制更新频率，避免1000Hz时卡死
        const now = performance.now();
        const lastUpdateTime = this.lastUpdateTime.get(varName) || 0;
        const updateInterval = Math.max(50, 1000 / Math.min(this.samplingRate, 60)); // 最少50ms更新一次，最多60Hz
        
        console.log(`[MultiChartManager] 节流检查 - 变量 ${varName}:`, {
            '当前时间': now.toFixed(1),
            '上次更新时间': lastUpdateTime.toFixed(1),
            '时间差': (now - lastUpdateTime).toFixed(1) + 'ms',
            '更新间隔': updateInterval.toFixed(1) + 'ms',
            '是否允许更新': (now - lastUpdateTime >= updateInterval)
        });
        
        if (now - lastUpdateTime >= updateInterval) {
            // 使用增量更新，只更新数据，不重新配置整个图表
            const option = {
                series: [{
                    data: chartDataArray,
                    type: 'line',
                    smooth: true,
                    symbol: 'none',
                    lineStyle: { width: 2, color: '#3498db' },
                    name: '实时数据'
                }],
                // 确保包含所有必要的配置，避免白屏
                animation: false,
                // 确保网格配置存在
                grid: { 
                    left: 80, 
                    right: 40, 
                    top: 50, 
                    bottom: 80,
                    containLabel: true
                },
                // 保持X轴配置，但更新数据范围
                xAxis: {
                    type: 'value',
                    name: '时间 (秒)',
                    nameLocation: 'middle',
                    nameGap: 30,
                    // 设置X轴范围，固定显示0到timeDisplayRange
                    min: 0,
                    max: this.timeDisplayRange,
                    // 启用时间轴滚动
                    axisLabel: {
                        formatter: function(value) {
                            return value.toFixed(1) + 's';
                        }
                    }
                },
                // 保持Y轴配置
                yAxis: { 
                    type: 'value', 
                    scale: true,
                    name: '数值',
                    nameLocation: 'middle',
                    nameGap: 40,
                    // 动态计算Y轴范围
                    min: (() => {
                        if (chartData.length > 0) {
                            const minVal = Math.min(...chartData);
                            const maxVal = Math.max(...chartData);
                            const range = maxVal - minVal;
                            
                            // 如果是1字节数据，设置合适的范围
                            if (minVal >= -128 && maxVal <= 127) {
                                return Math.max(-130, minVal - 2);
                            }
                            
                            // 对于大数值，使用更合理的余量计算
                            if (range > 0) {
                                // 使用数据范围的5%作为余量，但最小1，最大1000
                                const margin = Math.max(1, Math.min(1000, range * 0.05));
                                return minVal - margin;
                            } else {
                                // 如果数据没有变化，使用固定余量
                                return minVal - Math.abs(minVal) * 0.01;
                            }
                        }
                        return 0;
                    })(),
                    max: (() => {
                        if (chartData.length > 0) {
                            const minVal = Math.min(...chartData);
                            const maxVal = Math.max(...chartData);
                            const range = maxVal - minVal;
                            
                            // 如果是1字节数据，设置合适的范围
                            if (minVal >= -128 && maxVal <= 127) {
                                return Math.min(130, maxVal + 2);
                            }
                            
                            // 对于大数值，使用更合理的余量计算
                            if (range > 0) {
                                // 使用数据范围的5%作为余量，但最小1，最大1000
                                const margin = Math.max(1, Math.min(1000, range * 0.05));
                                return maxVal + margin;
                            } else {
                                // 如果数据没有变化，使用固定余量
                                return maxVal + Math.abs(maxVal) * 0.01;
                            }
                        }
                        return 100;
                    })(),
                    // 动态设置Y轴标签格式
                    axisLabel: {
                        formatter: function(value) {
                            // 如果是1字节范围，显示整数
                            if (value >= -128 && value <= 127) {
                                return Math.round(value);
                            }
                            // 对于大数值，使用更简洁的格式
                            if (Math.abs(value) >= 1000) {
                                return value.toFixed(0);
                            }
                            // 对于中等数值，保留1位小数
                            if (Math.abs(value) >= 10) {
                                return value.toFixed(1);
                            }
                            // 对于小数值，保留2位小数
                            return value.toFixed(2);
                        }
                    }
                },
                // 保持网格配置
                grid: { 
                    left: 80, 
                    right: 40, 
                    top: 50, 
                    bottom: 80,
                    containLabel: true
                },
                // 保持工具提示配置
                tooltip: {
                    trigger: 'axis',
                    formatter: function(params) {
                        const data = params[0];
                        const timeStr = params[0].axisValue || '未知时间';
                        // 确保数值正确显示
                        let valueStr;
                        if (Array.isArray(data.value)) {
                            // 如果data.value是数组[time, value]，取第二个元素作为数值
                            valueStr = data.value[1];
                        } else {
                            // 如果data.value是单个数值
                            valueStr = data.value;
                        }
                        return `时间: ${timeStr}<br/>数值: ${valueStr}`;
                    }
                },
                // 确保图表类型配置存在
                type: 'line'
            };
            
            // 使用增量更新，提高性能
            try {
                chartInstance.setOption(option, true);
                console.log(`[MultiChartManager] 图表更新成功 - 变量 ${varName}:`, {
                    '选项配置': option,
                    '图表实例状态': !!chartInstance,
                    '图表DOM状态': !!chartInstance.getDom()
                });
            } catch (error) {
                console.error(`[MultiChartManager] 图表更新失败 - 变量 ${varName}:`, error);
                // 如果增量更新失败，尝试完全重新设置
                try {
                    chartInstance.setOption(option, false);
                    console.log(`[MultiChartManager] 图表重新设置成功 - 变量 ${varName}`);
                } catch (retryError) {
                    console.error(`[MultiChartManager] 图表重新设置也失败 - 变量 ${varName}:`, retryError);
                }
            }
            
            // 记录更新时间（为每个变量单独记录）
            this.lastUpdateTime.set(varName, now);
            
                    // 调试：显示绘制结果
        console.log(`[MultiChartManager] 图表更新完成 - 变量 ${varName}:`, {
            '数据点数量': timeData.length,
            '更新间隔': updateInterval.toFixed(1) + 'ms',
            '采样率': this.samplingRate + 'Hz',
            '绘制频率': (1000 / updateInterval).toFixed(1) + 'Hz',
            '图表实例状态': !!chartInstance,
            '图表数据长度': chartInstance.getOption().series[0].data.length,
            '容器子元素数量': this.container.children.length,
            '图表DOM元素': !!this.container.querySelector(`[data-var="${varName}"]`)
        });
        }
        
        // 调试信息：显示X轴范围（限制频率，避免过多日志）
        // if (timeData.length > 0 && timeData.length % 100 === 0) { // 每100个数据点输出一次
        //     const xMin = Math.min(...timeData);
        //     const xMax = Math.max(...timeData);
        //     console.log(`[MultiChartManager] 变量 ${varName} 图表更新完成，数据点: ${timeData.length}, X轴范围: ${xMin.toFixed(3)}s - ${xMax.toFixed(3)}s`);
            
        //     // 显示实际时间范围变化
        //     if (timestamps.length > 1) {
        //         const firstTime = (timestamps[0] / 1000).toFixed(3);
        //         const lastTime = (timestamps[timestamps.length - 1] / 1000).toFixed(3);
        //         console.log(`[MultiChartManager] 变量 ${varName} 图表更新完成，数据点: ${timeData.length}, X轴范围: ${xMin.toFixed(3)}s - ${xMax.toFixed(3)}s`);
        //     }
        // }
    }
    
    /**
     * 删除指定变量的图表
     */
    removeChart(varName) {
        const chartInstance = this.charts.get(varName);
        if (chartInstance) {
            chartInstance.dispose();
        }
        
        this.charts.delete(varName);
        this.chartData.delete(varName);
        this.chartTimeData.delete(varName);
        this.varTimestamps.delete(varName);
        
        // 移除DOM元素
        const chartElement = this.container.querySelector(`[data-var="${varName}"]`);
        if (chartElement) {
            chartElement.remove();
        }
        
        // 如果没有其他图表，显示默认图表
        if (this.charts.size === 0 && this.defaultChart) {
            this.defaultChart.style.display = 'block';
        }
        
        // console.log(`删除了变量 ${varName} 的图表`);
    }
    
    /**
     * 清空所有图表
     */
    clearAllCharts() {
        // 销毁所有图表实例
        this.charts.forEach((chartInstance, varName) => {
            chartInstance.dispose();
        });
        this.charts.clear();
        this.chartData.clear();
        this.chartTimeData.clear();
        this.varTimestamps.clear();
        
        // 清空容器中的所有图表DOM元素
        const chartElements = this.container.querySelectorAll('.variable-chart');
        chartElements.forEach(element => {
            element.remove();
        });
        

        
        // 确保默认图表可见
        if (this.defaultChart) {
            this.defaultChart.style.display = 'block';
        }
        
        // console.log('已清空所有多变量图表');
    }
    
    /**
     * 设置采样率
     */
    setSamplingRate(rate) {
        this.samplingRate = rate;
        console.log(`[MultiChartManager] 采样率设置为 ${rate}Hz`);
    }
    
    /**
     * 设置时间显示范围
     */
    setTimeDisplayRange(range) {
        this.timeDisplayRange = range;
        console.log(`[MultiChartManager] 时间显示范围设置为 ${range}秒`);
        
        // 立即更新所有图表的X轴范围
        this.charts.forEach((chartInstance, varName) => {
            if (chartInstance && chartInstance.getOption) {
                const option = chartInstance.getOption();
                const chartData = this.chartData.get(varName);
                const timestamps = this.varTimestamps.get(varName);
                
                if (chartData && timestamps && timestamps.length > 0) {
                    const startTime = timestamps[0];
                    const timeData = timestamps.map(ts => (ts - startTime) / 1000);
                    const maxTime = Math.max(...timeData);
                    
                    // 更新X轴范围
                    option.xAxis = {
                        ...option.xAxis,
                        min: 0,
                        max: this.timeDisplayRange
                    };
                    
                    chartInstance.setOption(option);
                }
            }
        });
    }
    

    
    /**
     * 全屏显示指定变量的图表
     */
    fullscreenChart(varName) {
        const chartInstance = this.charts.get(varName);
        if (!chartInstance) {
            console.warn(`[MultiChartManager] 变量 ${varName} 的图表不存在`);
            return;
        }
        
        // 获取图表DOM元素
        const chartElement = this.container.querySelector(`[data-var="${varName}"]`);
        if (!chartElement) {
            console.warn(`[MultiChartManager] 变量 ${varName} 的图表DOM元素不存在`);
            return;
        }
        
        // 创建全屏容器
        const fullscreenContainer = document.createElement('div');
        fullscreenContainer.id = 'fullscreenChartContainer';
        fullscreenContainer.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            width: 100vw;
            height: 100vh;
            background: rgba(0, 0, 0, 0.9);
            z-index: 9999;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        `;
        
        // 创建全屏内容
        fullscreenContainer.innerHTML = `
            <div style="width: 95vw; height: 90vh; background: white; border-radius: 8px; position: relative;">
                <div style="position: absolute; top: 10px; right: 10px; z-index: 10000;">
                    <button class="btn btn-danger btn-sm" onclick="document.getElementById('fullscreenChartContainer').remove()" style="font-size: 16px; padding: 8px 12px;">
                        ✕ 关闭全屏
                    </button>
                </div>
                <div id="fullscreenChartContent" style="width: 100%; height: 100%; padding: 20px;"></div>
            </div>
        `;
        
        // 添加到页面
        document.body.appendChild(fullscreenContainer);
        
        // 创建新的ECharts实例
        const fullscreenChartDom = document.getElementById('fullscreenChartContent');
        const fullscreenChartInstance = echarts.init(fullscreenChartDom);
        
        // 复制原图表的配置和数据
        const originalOption = chartInstance.getOption();
        fullscreenChartInstance.setOption(originalOption);
        
        // 监听窗口大小变化，自动调整图表大小
        const resizeHandler = () => {
            fullscreenChartInstance.resize();
        };
        window.addEventListener('resize', resizeHandler);
        
        // 监听全屏容器移除事件，清理事件监听器
        const observer = new MutationObserver((mutations) => {
            mutations.forEach((mutation) => {
                if (mutation.type === 'childList') {
                    mutation.removedNodes.forEach((node) => {
                        if (node.id === 'fullscreenChartContainer') {
                            window.removeEventListener('resize', resizeHandler);
                            observer.disconnect();
                        }
                    });
                }
            });
        });
        observer.observe(document.body, { childList: true });
        
        console.log(`[MultiChartManager] 变量 ${varName} 图表已全屏显示`);
    }
}
