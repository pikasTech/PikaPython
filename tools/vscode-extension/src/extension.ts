import * as vscode from 'vscode';
import app from "./app/app";
import * as fs from 'fs';
import * as path from 'path';

function extractFilename(filePath: string): string {
	return path.basename(filePath, path.extname(filePath));
}

async function computePc(fileUri: vscode.Uri, lineNumber: number): Promise<number> {
	const precompilerPath = vscode.workspace.getConfiguration().get<string>('pikapython.precompilerPath');
	if (precompilerPath) {
		const cmd = `${precompilerPath} --break-point "${fileUri.fsPath}" ${lineNumber}`;
		const ret = await app.runCommandInNewTerminal(cmd);
		// Assuming ret = "{'pc': number}" or similar JSON structure
		const pc = JSON.parse(ret)['pc'];
		return pc;
	}
	throw new Error("Precompiler path not found in configuration");
}

export function activate(context: vscode.ExtensionContext) {
	console.log('Congratulations, your extension "PikaPython" is now active!');
	context.subscriptions.push(vscode.debug.onDidChangeBreakpoints(event => {
		event.added.forEach(async breakpoint => {
			if (breakpoint instanceof vscode.SourceBreakpoint) {
				const fileUri = breakpoint.location.uri;
				const lineNumber = breakpoint.location.range.start.line + 1; // Line numbers are 0-based in VS Code API
				const pc = await computePc(fileUri, lineNumber);
				app.runCommandInTerminal(`pdb.set_break("${extractFilename(fileUri.fsPath)}", ${pc})`);
			}
		});

		event.removed.forEach(async breakpoint => {
			if (breakpoint instanceof vscode.SourceBreakpoint) {
				const fileUri = breakpoint.location.uri;
				const lineNumber = breakpoint.location.range.start.line + 1;
				const pc = await computePc(fileUri, lineNumber);
				app.runCommandInTerminal(`pdb.reset_break("${extractFilename(fileUri.fsPath)}", ${pc})`);
			}
		});
	}));

	const workspaceFolder = getPrimaryWorkspaceFolder();
	if (workspaceFolder) {
		ensurePikaPythonSettings(workspaceFolder);
	}

	app.registerExtensionContext(context);
	context.subscriptions.push(vscode.languages.registerCodeLensProvider({ pattern: '**/*.py' }, new RunLineCodeLensProvider()));
}

function getPrimaryWorkspaceFolder(): vscode.WorkspaceFolder | undefined {
	const workspaceFolders = vscode.workspace.workspaceFolders;
	return workspaceFolders && workspaceFolders[0];
}

function ensurePikaPythonSettings(workspaceFolder: vscode.WorkspaceFolder): void {
	const settingsPath = path.join(workspaceFolder.uri.fsPath, '.vscode', 'settings.json');
	const defaultSettings = {
		// eslint-disable-next-line @typescript-eslint/naming-convention
		"pikapython.executablePath": "../PikaPython-win32.exe",
		// eslint-disable-next-line @typescript-eslint/naming-convention
		"pikapython.precompilerPath": "./rust-msc-latest-win10.exe"
	};

	if (!fs.existsSync(settingsPath)) {
		createSettingsFile(workspaceFolder.uri.fsPath, defaultSettings);
	} else {
		updateOrRecreateSettingsFile(settingsPath, defaultSettings);
	}
}

function createSettingsFile(workspaceFolderPath: string, defaultSettings: object): void {
	fs.mkdirSync(path.join(workspaceFolderPath, '.vscode'), { recursive: true });
	fs.writeFileSync(path.join(workspaceFolderPath, '.vscode', 'settings.json'), JSON.stringify(defaultSettings, null, 4));
}

function updateOrRecreateSettingsFile(settingsPath: string, defaultSettings: object): void {
	try {
		const settings = JSON.parse(fs.readFileSync(settingsPath, 'utf-8'));
		if (!settings['pikapython.executablePath']) {
			settings['pikapython.executablePath'] = "../PikaPython-win32.exe";
			fs.writeFileSync(settingsPath, JSON.stringify(settings, null, 4));
		}
		if (!settings['pikapython.precompilerPath']) {
			settings['pikapython.precompilerPath'] = "./rust-msc-latest-win10.exe";
			fs.writeFileSync(settingsPath, JSON.stringify(settings, null, 4));
		}
	} catch (error) {
		fs.writeFileSync(settingsPath, JSON.stringify(defaultSettings, null, 4));
	}
}

class RunLineCodeLensProvider implements vscode.CodeLensProvider {
	public provideCodeLenses(document: vscode.TextDocument, token: vscode.CancellationToken): vscode.CodeLens[] | Thenable<vscode.CodeLens[]> {
		const codeLenses = [];

		for (let line = 0; line < document.lineCount; line++) {
			const lineText = document.lineAt(line);
			if (lineText.text.trim() !== "" && !lineText.text.startsWith(" ")) { // ignore empty lines and indented lines
				const range = new vscode.Range(line, 0, line, lineText.text.length);
				const command: vscode.Command = {
					title: "$(triangle-right) Run With PikaPython",
					command: "extension.runLine",
					arguments: [line]  // 传递行号，而不是行内容
				};
				codeLenses.push(new vscode.CodeLens(range, command));
			}
		}

		return codeLenses;
	}
}

export function deactivate() { }
