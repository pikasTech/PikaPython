import * as vscode from 'vscode';
import app from "./app/app";
import * as fs from 'fs';
import * as path from 'path';

export function activate(context: vscode.ExtensionContext) {
	console.log('Congratulations, your extension "PikaPython" is now active!');
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
		"pikapython.executablePath": "../PikaPython-win32.exe"
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
