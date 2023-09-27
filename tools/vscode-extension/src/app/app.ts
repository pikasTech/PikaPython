import * as vscode from 'vscode';
import * as path from 'path';

function createAndShowRunButton() {
    let runButton = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left);
    runButton.text = "$(triangle-right) PikaPython Run Current File";
    runButton.tooltip = "Run the program";
    runButton.command = "extension.runCurrentFlie";
    runButton.show();
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
        createAndShowRunButton();
        this.registerRunCommandHandler(context);
        this.listenTerminalCloseEvent(context);
        this.registerRunSelectedTextHandler(context);
        this.registerRunLineHandler(context);
    }

    private registerRunCommandHandler(context: vscode.ExtensionContext) {
        let runProgramCommand = vscode.commands.registerCommand('extension.runCurrentFlie', () => {
            if (!hasActiveTextEditor()) {
                return;
            }

            let currentFilePath = getActiveFilePath();
            let command = buildRunCommand(currentFilePath);
            this.runCommandInTerminal(command);
        });

        context.subscriptions.push(runProgramCommand);
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


    private runCommandInTerminal(command: string) {
        if (!this.terminal) {
            this.terminal = vscode.window.createTerminal({ name: 'PikaPython' });
        }
        this.terminal.sendText(command);
        this.terminal.show();
    }
};
