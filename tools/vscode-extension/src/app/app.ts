import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import * as os from 'os';

function createAndShowButtons() {
    // Run Button
    let runButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
    runButton.text = "▶ PikaPython Run Current File";
    runButton.tooltip = "Run the program";
    runButton.command = "extension.runCurrentFile"; // Please ensure this command ID matches your implementation
    runButton.show();

    // Continue Running Button
    let continueButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
    continueButton.text = "▶▶ Continue";
    continueButton.tooltip = "Continue running the program";
    continueButton.command = "extension.continueRunning"; // Ensure you implement and register this command in your extension
    continueButton.show();

    // Run to the End
    let runToEndButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
    runToEndButton.text = "▶▶▶ Run to the End";
    runToEndButton.tooltip = "Run to the end of the program";
    runToEndButton.command = "extension.runToEnd"; // Ensure you implement and register this command in your extension
    runToEndButton.show();

    // Exit Running Button
    let exitButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
    exitButton.text = "■ Exit";
    exitButton.tooltip = "Exit running program";
    exitButton.command = "extension.exitRunning"; // Ensure you implement and register this command in your extension
    exitButton.show();
}

function hasActiveTextEditor(): boolean {
    if (!vscode.window.activeTextEditor) {
        vscode.window.showErrorMessage('No active text editor');
        return false;
    }
    return true;
}

function getActiveFilePath(): string {
    return vscode.window.activeTextEditor!.document.uri.fsPath;
}

function getPikaPythonExecutablePath(): string {
    return vscode.workspace.getConfiguration('pikapython').get<string>('executablePath')!;
}

function buildRunCommand(currentFilePath: string): string {
    const extensionPath = getPikaPythonExecutablePath();
    return `${path.resolve(extensionPath)} ${currentFilePath}`;
}

function getSelectedText(): string {
    let editor = vscode.window.activeTextEditor;
    if (!editor) {
        return '';
    }

    return editor.document.getText(editor.selection);
}

function getCurrentLineText(): string {
    let editor = vscode.window.activeTextEditor;
    if (!editor) {
        return '';
    }

    let cursorPosition = editor.selection.active;
    return editor.document.lineAt(cursorPosition.line).text;
}

export default new class App {
    private terminal: vscode.Terminal | undefined;

    public registerExtensionContext(context: vscode.ExtensionContext) {
        console.log('registerExtensionContext');
        createAndShowButtons();
        this.registerRunCommandHandler(context);
        this.listenTerminalCloseEvent(context);
        this.registerRunSelectedTextHandler(context);
        this.registerRunLineHandler(context);
    }

    private registerRunCommandHandler(context: vscode.ExtensionContext) {
        let runProgramCommand = vscode.commands.registerCommand('extension.runCurrentFile', () => {
            if (!hasActiveTextEditor()) {
                return;
            }
            let currentFilePath = getActiveFilePath();
            let command = buildRunCommand(currentFilePath);
            this.runCommandInTerminal(command);
        });
        let continueRunningCommand = vscode.commands.registerCommand('extension.continueRunning', () => {
            if (!hasActiveTextEditor()) {
                return;
            }
            this.runCommandInTerminal('c');
        });
        let runToEndCommand = vscode.commands.registerCommand('extension.runToEnd', () => {
            if (!hasActiveTextEditor()) {
                return;
            }
            this.runCommandInTerminal('q');
        });
        let exitRunningCommand = vscode.commands.registerCommand('extension.exitRunning', () => {
            if (!hasActiveTextEditor()) {
                return;
            }
            this.runCommandInTerminal('exit()');
            // sleep 1s to wait for the process to exit
        });
        context.subscriptions.push(runProgramCommand);
        context.subscriptions.push(continueRunningCommand);
        context.subscriptions.push(runToEndCommand);
        context.subscriptions.push(exitRunningCommand);
    }

    private listenTerminalCloseEvent(context: vscode.ExtensionContext) {
        context.subscriptions.push(vscode.window.onDidCloseTerminal((closedTerminal) => {
            if (this.terminal === closedTerminal) {
                this.terminal = undefined;
            }
        }));
    }

    private getIndentationLevel(line: string): number {
        const match = line.match(/^(\s*)/);
        return match ? match[1].length / 4 : 0;  // Assuming 4 spaces for indentation
    }

    private getEntireCodeBlock(lineNumber: number, document: vscode.TextDocument): string {
        let startLine = lineNumber;
        let endLine = lineNumber;
        const clickedLineIndentation = this.getIndentationLevel(document.lineAt(lineNumber).text);

        // Find the start of the block
        while (startLine > 0 && this.getIndentationLevel(document.lineAt(startLine).text) !== clickedLineIndentation) {
            startLine--;
        }

        // Find the end of the block
        while (endLine < document.lineCount - 1) {
            const nextLineIndentation = this.getIndentationLevel(document.lineAt(endLine + 1).text);
            if (nextLineIndentation <= clickedLineIndentation) {
                break;
            }
            endLine++;
        }

        // Extract block lines
        let blockLines = [];
        for (let i = startLine; i <= endLine; i++) {
            blockLines.push(document.lineAt(i).text);
        }

        // If the block has multiple lines, add a newline at the end to ensure it executes in REPL
        if (blockLines.length > 1) {
            blockLines.push('\n');
        }

        return blockLines.join('\n');
    }


    private registerRunLineHandler(context: vscode.ExtensionContext) {
        let runLineCommand = vscode.commands.registerCommand('extension.runLine', (lineNumber: number) => {
            if (!hasActiveTextEditor()) {
                return;
            }
            const activeEditor = vscode.window.activeTextEditor;
            const blockText = this.getEntireCodeBlock(lineNumber, activeEditor!.document);
            this.runCommandInTerminal(blockText);
        });

        context.subscriptions.push(runLineCommand);
    }

    private registerRunSelectedTextHandler(context: vscode.ExtensionContext) {
        let runSelectedTextCommand = vscode.commands.registerCommand('extension.runSelectedText', () => {
            if (!hasActiveTextEditor()) {
                return;
            }

            let selectedText = getSelectedText();
            if (!selectedText) {
                const activeEditor = vscode.window.activeTextEditor;
                const lineNumber = activeEditor!.selection.start.line;
                selectedText = this.getEntireCodeBlock(lineNumber, activeEditor!.document);
            }
            this.runCommandInTerminal(selectedText);
        });

        context.subscriptions.push(runSelectedTextCommand);
    }


    public runCommandInTerminal(command: string) {
        if (!this.terminal) {
            this.terminal = vscode.window.createTerminal({ name: 'PikaPython' });
        }
        this.terminal.sendText(command);
        this.terminal.show();
    }

    public async runCommandInNewTerminal(command: string): Promise<string> {
        return new Promise((resolve, reject) => {
            // 创建一个临时文件来捕获输出
            const tempFilePath = path.join(os.tmpdir(), 'pikapython_output.txt');

            // 在新terminal中运行命令并将输出重定向到临时文件
            const terminal = vscode.window.createTerminal('PikaPython');
            terminal.sendText(`${command} > "${tempFilePath}"`);

            let checkCount = 0;
            const maxChecks = 20; // 检查次数，因为我们每100ms检查一次，所以20次就是2s
            const intervalId = setInterval(() => {
                checkCount++;
                if (fs.existsSync(tempFilePath)) {
                    clearInterval(intervalId); // 停止定时器

                    const output = fs.readFileSync(tempFilePath, 'utf-8');
                    fs.unlinkSync(tempFilePath); // 删除临时文件
                    terminal.dispose(); // 关闭terminal

                    resolve(output);
                } else if (checkCount >= maxChecks) {
                    clearInterval(intervalId); // 停止定时器

                    terminal.dispose();
                    reject(new Error('Failed to capture terminal output within 2 seconds.'));
                }
            }, 100); // 每100ms执行一次
        });
    }
};
